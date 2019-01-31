#include "rs-core/english.hpp"
#include "rs-core/float.hpp"
#include "unicorn/string.hpp"
#include "unicorn/character.hpp"
#include "unicorn/format.hpp"
#include "unicorn/regex.hpp"
#include "unicorn/utf.hpp"
#include <algorithm>
#include <map>
#include <string>
#include <utility>
#include <vector>

using namespace RS::Literals;
using namespace RS::Unicorn;
using namespace std::literals;

namespace RS {

    // Case conversion functions

    namespace Detail {

        Ustring format_enum_helper(Ustring s, Uview flags) {
            static const Regex pattern("([a-z])?(\\pP|\\pS|\\pZ)?", Regex::anchor | Regex::icase | Regex::optimize);
            auto match = pattern(flags);
            char casing = 0;
            Ustring delim = " ";
            if (match.matched(1))
                casing = ascii_tolower(match[1][0]);
            if (match.matched(2))
                delim = match[2];
            s = unqualify(s);
            std::replace(s.begin(), s.end(), '_', ' ');
            switch (casing) {
                case 's':  str_initial_titlecase_in(s); break;
                case 't':  str_titlecase_in(s); break;
                case 'x':  extended_title_case_in(s); break;
                default:   break;
            }
            str_replace_in(s, " ", delim);
            return s;
        }

    }

    Ustring extended_title_case(Uview str) {
        Ustring text(str);
        extended_title_case_in(text);
        return text;
    }

    void extended_title_case_in(Ustring& str) {
        static const Regex stopwords("\\b(A|An|And|At|In|Of|Or|The)\\b", Regex::no_capture | Regex::optimize);
        if (str.empty())
            return;
        str_titlecase_in(str);
        size_t pos = 1, n = 0;
        while (pos < str.size()) {
            auto match = stopwords(str, pos);
            if (! match)
                break;
            pos = match.offset();
            n = match.count();
            str.replace(pos, n, ascii_lowercase(match));
            pos += n + 1;
        }
    }

    // List formatting functions

    namespace Detail {

        Ustring comma_list_helper(const Strings& list, Uview conj) {
            Ustring s;
            size_t comma1 = 0, comma2 = 0, n = 0;
            for (auto& x: list) {
                s += x + ", ";
                comma1 = comma2;
                comma2 = s.size() - 2;
                ++n;
            }
            if (n == 0)
                return {};
            s.resize(comma2);
            if (n == 1)
                return s;
            if (! conj.empty()) {
                s.insert(comma1 + 2, Ustring(conj) + ' ');
                if (n == 2)
                    s.erase(comma1, 1);
            }
            return s;
        }

    }


    // Number formatting functions

    namespace {

        const Strings card_ones = ",one,two,three,four,five,six,seven,eight,nine,ten,eleven,twelve,thirteen,fourteen,fifteen,sixteen,seventeen,eighteen,nineteen"_csv;
        const Strings card_tens = ",ten,twenty,thirty,forty,fifty,sixty,seventy,eighty,ninety"_csv;
        const Strings ord_ones = ",first,second,third,fourth,fifth,sixth,seventh,eighth,ninth,tenth,eleventh,twelfth,thirteenth,fourteenth,fifteenth,sixteenth,seventeenth,eighteenth,nineteenth"_csv;
        const Strings ord_tens = ",tenth,twentieth,thirtieth,fortieth,fiftieth,sixtieth,seventieth,eightieth,ninetieth"_csv;

        size_t card_add(Strings& words, size_t n, size_t scale, Uview scale_name, Uview suffix) {
            if (n < scale)
                return n;
            if (! words.empty())
                words.back() += ',';
            words.push_back(cardinal(n / scale));
            words.push_back(Ustring(scale_name));
            n %= scale;
            if (n == 0)
                words.back() += suffix;
            return n;
        }

        Ustring number_name(size_t n, const Strings& ones, const Strings& tens, Uview suffix) {
            if (n == 0)
                return "zero"s + Ustring(suffix);
            Ustring sign;
            Strings words;
            n = card_add(words, n, 1'000'000'000, "billion", suffix);
            n = card_add(words, n, 1'000'000, "million", suffix);
            n = card_add(words, n, 1000, "thousand", suffix);
            n = card_add(words, n, 100, "hundred", suffix);
            if (n > 0 && ! words.empty())
                words.push_back("and");
            Ustring extra;
            if (n % 10 == 0) {
                extra += tens[n / 10];
            } else {
                if (n >= 20) {
                    extra += card_tens[n / 10];
                    extra += '-';
                    n %= 10;
                }
                extra += ones[n];
            }
            if (! extra.empty())
                words.push_back(extra);
            return str_join(words, " ");
        }

    }

    Ustring cardinal(size_t n, size_t threshold) {
        if (n < threshold)
            return number_name(n, card_ones, card_tens, {});
        else
            return std::to_string(n);
    }

    Ustring ordinal(size_t n, size_t threshold) {
        if (n < threshold)
            return number_name(n, ord_ones, ord_tens, "th");
        Ustring suffix = "th";
        if ((n % 100) / 10 != 1) {
            switch (n % 10) {
                case 1:   suffix = "st"; break;
                case 2:   suffix = "nd"; break;
                case 3:   suffix = "rd"; break;
                default:  break;
            }
        }
        return std::to_string(n) + suffix;
    }

    Ustring format_count(double x, int prec) {
        if (x <= 0)
            return "0";
        prec = std::max(prec, 1);
        x = round_to_digits(x, prec);
        if (x < 1e4) {
            return std::to_string(iround<int>(x));
        } else if (x < 1e12) {
            double scale;
            Ustring suffix;
            if (x < 1e6) {
                scale = 1e-3;
                suffix = "thousand";
            } else if (x < 1e9) {
                scale = 1e-6;
                suffix = "million";
            } else {
                scale = 1e-9;
                suffix = "billion";
            }
            return format_type(scale * x, Format::digits, prec) + ' ' + suffix;
        } else {
            Ustring sig, exp;
            str_partition_at(format_type(x, Format::exp, prec), sig, exp, "e");
            return sig + "x10^" + exp;
        }
    }

    Ustring number_of(size_t n, Uview name, Uview plural_name, size_t threshold) {
        Ustring result;
        if (n >= 1)
            result = cardinal(n, threshold);
        else if (name.empty())
            result = "none";
        else
            result = "no";
        if (name.empty())
            return result;
        result += ' ';
        if (n == 1)
            result += name;
        else if (plural_name.empty())
            result += plural(name);
        else
            result += plural_name;
        return result;
    }

    // Pluralization functions

    Ustring plural(Uview noun) {
        // Partly based on Damian Conway's work
        // http://www.csse.monash.edu.au/~damian/papers/HTML/Plurals.html
        // Keep inflections in descending order by length (order within a length is unimportant)
        static const std::vector<std::pair<Ustring, Ustring>> inflections = {
            { "goose",  "geese" },
            { "louse",  "lice" },
            { "mouse",  "mice" },
            { "tooth",  "teeth" },
            { "drum",   "drums" },
            { "foot",   "feet" },
            { "zoon",   "zoa" },
            { "cis",    "ces" },
            { "man",    "men" },
            { "och",    "ochs" },
            { "sis",    "ses" },
            { "xis",    "xes" },
            { "ao",     "aos" },
            { "ay",     "ays" },
            { "ch",     "ches" },
            { "eo",     "eos" },
            { "ex",     "ices" },
            { "ey",     "eys" },
            { "fe",     "ves" },
            { "io",     "ios" },
            { "iy",     "iys" },
            { "oo",     "oos" },
            { "oy",     "oys" },
            { "sh",     "shes" },
            { "ss",     "sses" },
            { "um",     "a" },
            { "uo",     "uos" },
            { "us",     "i" },
            { "uy",     "uys" },
            { "a",      "ae" },
            { "f",      "ves" },
            { "o",      "oes" },
            { "s",      "ses" },
            { "x",      "xes" },
            { "y",      "ies" },
        };
        static const std::map<Ustring, Ustring> irregular = {
            { "afreet",         "afreeti" },
            { "afrit",          "afriti" },
            { "albino",         "albinos" },
            { "alto",           "altos" },
            { "aphelion",       "aphelia" },
            { "apparatus",      "apparatuses" },
            { "apsis",          "apsides" },
            { "archipelago",    "archipelagos" },
            { "armadillo",      "armadillos" },
            { "asyndeton",      "asyndeta" },
            { "basso",          "bassos" },
            { "canto",          "cantos" },
            { "cantus",         "cantuses" },
            { "cherub",         "cherubim" },
            { "child",          "children" },
            { "chrysanthemum",  "chrysanthemums" },
            { "coitus",         "coituses" },
            { "comma",          "commas" },
            { "commando",       "commandos" },
            { "contralto",      "contraltos" },
            { "crescendo",      "crescendos" },
            { "criterion",      "criteria" },
            { "ditto",          "dittos" },
            { "djinni",         "djinn" },
            { "dynamo",         "dynamos" },
            { "efreet",         "efreeti" },
            { "embryo",         "embryos" },
            { "ephemeris",      "ephemerides" },
            { "fiasco",         "fiascos" },
            { "generalissimo",  "generalissimos" },
            { "ghetto",         "ghettos" },
            { "goy",            "goyim" },
            { "guano",          "guanos" },
            { "hiatus",         "hiatuses" },
            { "hyperbaton",     "hyperbata" },
            { "impetus",        "impetuses" },
            { "inferno",        "infernos" },
            { "jumbo",          "jumbos" },
            { "lingo",          "lingos" },
            { "lumbago",        "lumbagos" },
            { "magneto",        "magnetos" },
            { "manifesto",      "manifestos" },
            { "mantis",         "mantids" },
            { "medico",         "medicos" },
            { "mongoose",       "mongooses" },
            { "mythos",         "mythoi" },
            { "nexus",          "nexuses" },
            { "noumenon",       "noumena" },
            { "octavo",         "octavos" },
            { "organon",        "organa" },
            { "ox",             "oxen" },
            { "perihelion",     "perihelia" },
            { "phenomenon",     "phenomena" },
            { "photo",          "photos" },
            { "plexus",         "plexuses" },
            { "pro",            "pros" },
            { "prolegomenon",   "prolegomena" },
            { "prospectus",     "prospectuses" },
            { "quarto",         "quartos" },
            { "rhino",          "rhinos" },
            { "seraph",         "seraphim" },
            { "sinus",          "sinuses" },
            { "soliloquy",      "soliloquies" },
            { "solo",           "solos" },
            { "soprano",        "sopranos" },
            { "sphex",          "sphexes" },
            { "status",         "statuses" },
            { "stylo",          "stylos" },
            { "tempo",          "tempos" },
            { "trilby",         "trilbys" },
        };
        // Keep uninflected words in alphabetical order
        static const auto uninflected = R"(
            bison bream breeches britches carp chassis clippers cod contretemps
            corps debris diabetes djinn eland elk fish flounder gallows graffiti
            headquarters herpes highjinks hijinks homework innings jackanapes
            mackerel measles mews mumps news pincers pliers proceedings rabies
            salmon scissors series shears species swine trout tuna whiting
            wildebeest
        )"_qw;
        static const auto is_lower = gc_predicate("Ll");
        static const auto is_upper = gc_predicate("Ltu");
        if (noun.empty())
            return {};
        Ustring s_noun(noun);
        Ustring l_noun = str_lowercase(s_noun);
        Ustring p_noun;
        if (std::binary_search(uninflected.begin(), uninflected.end(), l_noun)) {
            p_noun = l_noun;
        } else {
            auto it = irregular.find(l_noun);
            if (it == irregular.end()) {
                for (auto& [s,p]: inflections) {
                    if (str_ends_with(l_noun, s)) {
                        p_noun = l_noun.substr(0, l_noun.size() - s.size()) + p;
                        break;
                    }
                }
                if (p_noun.empty())
                    p_noun = l_noun + 's';
            } else {
                p_noun = it->second;
            }
        }
        auto [begin,end] = utf_range(s_noun);
        end = std::find_if(begin, end, is_lower);
        switch (std::count_if(begin, end, is_upper)) {
            case 0:   break;
            case 1:   str_initial_titlecase_in(p_noun); break;
            default:  str_uppercase_in(p_noun); break;
        }
        return p_noun;
    }

}
