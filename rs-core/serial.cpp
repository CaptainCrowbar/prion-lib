#include "rs-core/serial.hpp"
#include "rs-core/encoding.hpp"
#include "rs-core/file-system.hpp"
#include "rs-core/string.hpp"
#include <cstring>
#include <string>

using namespace RS::Unicorn;

namespace RS {

    // Serialization for core library types

    void to_json(json& j, const Blob& x) {
        Ustring s;
        Base64Encoding().encode_bytes(x.data(), x.size(), s);
        j = s;
    }

    void from_json(const json& j, Blob& x) {
        std::string dec, enc = j;
        Base64Encoding().decode(enc, dec);
        x.reset(dec.size());
        std::memcpy(x.data(), dec.data(), dec.size());
    }

    // Class PersistState

    PersistState::PersistState(const Ustring& id) {
        Strings breakdown = splitv(id, "/");
        if (breakdown.empty())
            throw std::invalid_argument("Invalid persistent storage key: " + quote(id));
        for (auto& item: breakdown)
            if (item.empty() || item.size() > 100 || ! valid_string(item)
                    || std::find_if(item.begin(), item.end(), ascii_iscntrl) != item.end()
                    || item.find_first_of("\"*/:<>?[\\]|") != npos
                    || std::find_if(item.begin(), item.end(), [] (char c) { return ascii_isalnum(c) || uint8_t(c) > 127; }) == item.end()
                    || item.front() == ' ' || item.front() == '.' || item.back() == ' ' || item.back() == '.')
                throw std::invalid_argument("Invalid persistent storage key: " + quote(id));
        state_id = id;
        global_mutex = std::make_unique<NamedMutex>(id);
        load();
    }

    PersistState::~PersistState() noexcept {
        try {
            autosave_off();
            save_state(false);
        }
        catch (...) {}
    }

    void PersistState::load() {
        auto local_lock = make_lock(local_mutex);
        auto global_lock = make_lock(*global_mutex);
        Unicorn::Path archive = archive_name();
        Unicorn::Path old_archive = archive_name("old");
        if (! archive.exists() && old_archive.exists())
            old_archive.move_to(archive);
        json j = json::object();
        Ustring content;
        archive.load(content, npos, Unicorn::Path::may_fail);
        if (! content.empty())
            j = json::parse(content);
        if (j.is_object())
            state_table = j;
        else
            state_table = json::object();
        change_flag = false;
    }

    void PersistState::autosave_off() {
        auto local_lock = make_lock(local_mutex);
        clear_autosave();
    }

    void PersistState::create(const Ustring& key, const json& value) {
        auto local_lock = make_lock(local_mutex);
        if (state_table.find(key) == state_table.end())
            state_table[key] = value;
        change_flag = true;
    }

    bool PersistState::read(const Ustring& key, json& value) {
        auto local_lock = make_lock(local_mutex);
        auto it = state_table.find(key);
        if (it == state_table.end())
            return false;
        value = it.value();
        return true;
    }

    void PersistState::update(const Ustring& key, const json& value) {
        auto local_lock = make_lock(local_mutex);
        state_table[key] = value;
        change_flag = true;
    }

    void PersistState::erase(const Ustring& key) {
        auto local_lock = make_lock(local_mutex);
        state_table.erase(key);
        change_flag = true;
    }

    Unicorn::Path PersistState::archive_name(const Ustring& tag) const {
        auto path = state_id;
        if (! tag.empty())
            path += '.' + tag;
        path += ".settings";
        return std_path(UserPath::settings) / path;
    }

    void PersistState::autosave_loop() {
        for (;;) {
            autosave_channel->wait();
            if (autosave_channel->is_closed())
                break;
            save_state(false);
        }
    }

    void PersistState::clear_autosave() {
        if (autosave_channel) {
            autosave_channel->close();
            auto cleanup = scope_exit([&] {
                autosave_channel.reset();
                autosave_thread = {};
            });
            autosave_thread.join();
            autosave_thread = {};
        }
    }

    void PersistState::save_state(bool always) {
        auto local_lock = make_lock(local_mutex);
        if (! always && ! change_flag)
            return;
        auto global_lock = make_lock(*global_mutex);
        Unicorn::Path archive = archive_name();
        Ustring content = state_table.dump(4) + '\n';
        if (archive.exists()) {
            Unicorn::Path new_archive = archive_name("new");
            Unicorn::Path old_archive = archive_name("old");
            new_archive.remove();
            old_archive.remove();
            auto cleanup = scope_exit([&] {
                new_archive.remove();
                old_archive.remove();
            });
            new_archive.save(content);
            archive.move_to(old_archive);
            auto rollback = scope_fail([&] { old_archive.move_to(archive); });
            new_archive.move_to(archive);
        } else {
            archive.split_path().first.make_directory(Unicorn::Path::recurse);
            archive.save(content);
        }
        change_flag = false;
    }

}
