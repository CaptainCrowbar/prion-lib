#include "rs-core/meta.hpp"
#include "rs-core/string.hpp"
#include "rs-core/unit-test.hpp"
#include <forward_list>
#include <iterator>
#include <list>
#include <map>
#include <memory>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

using namespace RS;
using namespace RS::Meta;

// Used in introspection tests
// (global to work around unused function warning bug in gcc)
int foo() RS_ATTR_UNUSED;
int foo(int) RS_ATTR_UNUSED;
int foo(int, int) RS_ATTR_UNUSED;

namespace {

    // Sort types into void < integers by size < pointers < everything else
    template <typename T1, typename T2> struct Compare {
        enum {
            t1_is_void     = std::is_void<T1>::value,
            t1_is_integer  = std::is_integral<T1>::value,
            t1_is_pointer  = std::is_pointer<T1>::value,
            t1_key         = 4 * t1_is_void + 2 * t1_is_integer + t1_is_pointer,
            t2_is_void     = std::is_void<T2>::value,
            t2_is_integer  = std::is_integral<T2>::value,
            t2_is_pointer  = std::is_pointer<T2>::value,
            t2_key         = 4 * t2_is_void + 2 * t2_is_integer + t2_is_pointer,
            value          = t1_key > t2_key || (t1_key == t2_key && sizeof(T1) < sizeof(T2))
        };
    };

    template <typename T1, typename T2> using MakePair = std::pair<T1, T2>;
    template <typename T> using MakePtr = T*;

    template <typename T> struct Pattern;
    template <> struct Pattern<Nil>          { static std::string str() { return "*"; } };
    template <> struct Pattern<int>          { static std::string str() { return "i"; } };
    template <> struct Pattern<void*>        { static std::string str() { return "p"; } };
    template <> struct Pattern<std::string>  { static std::string str() { return "s"; } };
    template <typename T1, typename T2> struct Pattern<std::pair<T1, T2>>
        { static std::string str() { return "(" + Pattern<T1>::str() + Pattern<T2>::str() + ")"; } };

    template <typename T1, typename T2> struct UnifyType;
    template <typename T> struct UnifyType<T, T> { using type = T; };
    template <typename T1, typename T2> using Unify = typename UnifyType<T1, T2>::type;

    Ustring thing_str;

    template <char C>
    struct Thing {
        Thing(): list() { thing_str += catstr(C, "; "); }
        Thing(Uview a, Uview b): list{Ustring(a), Ustring(b)} { thing_str += catstr(C, "(", a, ",", b, "); "); }
        void operator()() const { thing_str += catstr(C, to_str(list), "; "); }
        void operator()(Uview a, Uview b) const { std::vector<Uview> v{a, b}; thing_str += catstr(C, to_str(v), "; "); }
        Strings list;
    };

    using ThingA = Thing<'A'>;
    using ThingB = Thing<'B'>;
    using ThingC = Thing<'C'>;
    using ThingList = Typelist<ThingA, ThingB, ThingC>;

    struct ThingF1 {
        template <char C> void operator()(const Thing<C>& t) const {
            thing_str += catstr("F1 ", C, to_str(t.list), "; ");
        }
    };

    struct ThingF2 {
        template <char C> void operator()(const Thing<C>& t, Uview a, Uview b) const {
            std::vector<Uview> v{a, b};
            thing_str += catstr("F2 ", C, to_str(t.list), " ", to_str(v), "; ");
        }
    };

    struct NoBar { int foo; };
    struct Bar0 { int foo; int bar() const; };
    struct Bar1 { int foo; int bar(int) const; };
    struct Bar2 { int foo; int bar(int, int) const; };

    RS_DETECT_FUNCTION(foo);
    RS_DETECT_STD_FUNCTION(begin);
    RS_DETECT_METHOD(bar);
    RS_DETECT_MEMBER(foo);
    RS_DETECT_MEMBER_TYPE(value_type);

}

void test_core_meta_logic() {

    TEST((! Not<std::is_signed<int>>::value));
    TEST((Not<std::is_signed<unsigned>>::value));

    TEST((And<std::is_signed<int>, std::is_integral<int>>::value));
    TEST((! And<std::is_signed<unsigned>, std::is_integral<unsigned>>::value));
    TEST((! And<std::is_signed<unsigned>, std::is_floating_point<unsigned>>::value));
    TEST((And<std::is_signed<int>, std::is_integral<int>, std::is_arithmetic<int>>::value));
    TEST((! And<std::is_signed<unsigned>, std::is_integral<unsigned>, std::is_arithmetic<unsigned>>::value));
    TEST((! And<std::is_signed<unsigned>, std::is_floating_point<unsigned>, std::is_pointer<unsigned>>::value));

    TEST((Or<std::is_signed<int>, std::is_integral<int>>::value));
    TEST((Or<std::is_signed<unsigned>, std::is_integral<unsigned>>::value));
    TEST((! Or<std::is_signed<unsigned>, std::is_floating_point<unsigned>>::value));
    TEST((Or<std::is_signed<int>, std::is_integral<int>, std::is_arithmetic<int>>::value));
    TEST((Or<std::is_signed<unsigned>, std::is_integral<unsigned>, std::is_arithmetic<unsigned>>::value));
    TEST((! Or<std::is_signed<unsigned>, std::is_floating_point<unsigned>, std::is_pointer<unsigned>>::value));

    TEST((! Xor<std::is_signed<int>, std::is_integral<int>>::value));
    TEST((Xor<std::is_signed<unsigned>, std::is_integral<unsigned>>::value));
    TEST((! Xor<std::is_signed<unsigned>, std::is_floating_point<unsigned>>::value));

    TEST((! Not<And<std::is_signed<int>, std::is_integral<int>>>::value));
    TEST((Not<And<std::is_signed<unsigned>, std::is_integral<unsigned>>>::value));
    TEST((Not<And<std::is_signed<unsigned>, std::is_floating_point<unsigned>>>::value));

    TEST((! Not<Or<std::is_signed<int>, std::is_integral<int>>>::value));
    TEST((! Not<Or<std::is_signed<unsigned>, std::is_integral<unsigned>>>::value));
    TEST((Not<Or<std::is_signed<unsigned>, std::is_floating_point<unsigned>>>::value));

    TEST((Not<Xor<std::is_signed<int>, std::is_integral<int>>>::value));
    TEST((! Not<Xor<std::is_signed<unsigned>, std::is_integral<unsigned>>>::value));
    TEST((Not<Xor<std::is_signed<unsigned>, std::is_floating_point<unsigned>>>::value));

    TEST((! not_<std::is_signed<int>>));
    TEST((not_<std::is_signed<unsigned>>));

    TEST((and_<std::is_signed<int>, std::is_integral<int>>));
    TEST((! and_<std::is_signed<unsigned>, std::is_integral<unsigned>>));
    TEST((! and_<std::is_signed<unsigned>, std::is_floating_point<unsigned>>));
    TEST((and_<std::is_signed<int>, std::is_integral<int>, std::is_arithmetic<int>>));
    TEST((! and_<std::is_signed<unsigned>, std::is_integral<unsigned>, std::is_arithmetic<unsigned>>));
    TEST((! and_<std::is_signed<unsigned>, std::is_floating_point<unsigned>, std::is_pointer<unsigned>>));

    TEST((or_<std::is_signed<int>, std::is_integral<int>>));
    TEST((or_<std::is_signed<unsigned>, std::is_integral<unsigned>>));
    TEST((! or_<std::is_signed<unsigned>, std::is_floating_point<unsigned>>));
    TEST((or_<std::is_signed<int>, std::is_integral<int>, std::is_arithmetic<int>>));
    TEST((or_<std::is_signed<unsigned>, std::is_integral<unsigned>, std::is_arithmetic<unsigned>>));
    TEST((! or_<std::is_signed<unsigned>, std::is_floating_point<unsigned>, std::is_pointer<unsigned>>));

    TEST((! xor_<std::is_signed<int>, std::is_integral<int>>));
    TEST((xor_<std::is_signed<unsigned>, std::is_integral<unsigned>>));
    TEST((! xor_<std::is_signed<unsigned>, std::is_floating_point<unsigned>>));

}

void test_core_meta_append() {

    using list_i     = Typelist<int>;
    using list_is    = Typelist<int, std::string>;
    using list_isp   = Typelist<int, std::string, void*>;
    using list_isps  = Typelist<int, std::string, void*, std::string>;

    {  using type = Append<Nil, int>;               TEST_TYPE(type, list_i);     }
    {  using type = Append<list_i, std::string>;    TEST_TYPE(type, list_is);    }
    {  using type = Append<list_is, void*>;         TEST_TYPE(type, list_isp);   }
    {  using type = Append<list_isp, std::string>;  TEST_TYPE(type, list_isps);  }
    {  using type = Append<Nil, Nil>;               TEST_TYPE(type, Nil);        }
    {  using type = Append<list_isp, Nil>;          TEST_TYPE(type, list_isp);   }

}

void test_core_meta_concat() {

    using list_i       = Typelist<int>;
    using list_s       = Typelist<std::string>;
    using list_p       = Typelist<void*>;
    using list_is      = Typelist<int, std::string>;
    using list_sp      = Typelist<std::string, void*>;
    using list_ss      = Typelist<std::string, std::string>;
    using list_isp     = Typelist<int, std::string, void*>;
    using list_spi     = Typelist<std::string, void*, int>;
    using list_issp    = Typelist<int, std::string, std::string, void*>;
    using list_spsp    = Typelist<std::string, void*, std::string, void*>;
    using list_isssp   = Typelist<int, std::string, std::string, std::string, void*>;
    using list_issssp  = Typelist<int, std::string, std::string, std::string, std::string, void*>;

    {  using type = Concat<>;                                               TEST_TYPE(type, Nil);          }
    {  using type = Concat<Nil>;                                            TEST_TYPE(type, Nil);          }
    {  using type = Concat<Nil, Nil>;                                       TEST_TYPE(type, Nil);          }
    {  using type = Concat<Nil, list_i>;                                    TEST_TYPE(type, list_i);       }
    {  using type = Concat<Nil, list_is>;                                   TEST_TYPE(type, list_is);      }
    {  using type = Concat<list_i, Nil>;                                    TEST_TYPE(type, list_i);       }
    {  using type = Concat<list_is, Nil>;                                   TEST_TYPE(type, list_is);      }
    {  using type = Concat<list_i, list_s>;                                 TEST_TYPE(type, list_is);      }
    {  using type = Concat<list_i, list_sp>;                                TEST_TYPE(type, list_isp);     }
    {  using type = Concat<list_sp, list_i>;                                TEST_TYPE(type, list_spi);     }
    {  using type = Concat<list_sp, list_sp>;                               TEST_TYPE(type, list_spsp);    }
    {  using type = Concat<list_i, list_s, list_p>;                         TEST_TYPE(type, list_isp);     }
    {  using type = Concat<list_is, list_ss, list_sp>;                      TEST_TYPE(type, list_issssp);  }
    {  using type = Concat<Nil, list_is, Nil, list_ss, Nil, list_sp, Nil>;  TEST_TYPE(type, list_issssp);  }
    {  using type = Concat<int>;                                            TEST_TYPE(type, list_i);       }
    {  using type = Concat<int, Nil>;                                       TEST_TYPE(type, list_i);       }
    {  using type = Concat<Nil, int>;                                       TEST_TYPE(type, list_i);       }
    {  using type = Concat<int, list_s>;                                    TEST_TYPE(type, list_is);      }
    {  using type = Concat<list_i, std::string>;                            TEST_TYPE(type, list_is);      }
    {  using type = Concat<int, list_ss, list_sp>;                          TEST_TYPE(type, list_isssp);   }
    {  using type = Concat<list_is, std::string, list_sp>;                  TEST_TYPE(type, list_isssp);   }
    {  using type = Concat<list_is, list_ss, void*>;                        TEST_TYPE(type, list_isssp);   }
    {  using type = Concat<int, std::string, list_sp>;                      TEST_TYPE(type, list_issp);    }
    {  using type = Concat<int, list_ss, void*>;                            TEST_TYPE(type, list_issp);    }
    {  using type = Concat<list_is, std::string, void*>;                    TEST_TYPE(type, list_issp);    }
    {  using type = Concat<int, std::string, void*>;                        TEST_TYPE(type, list_isp);     }

}

void test_core_meta_insert() {

    using list_i     = Typelist<int>;
    using list_s     = Typelist<std::string>;
    using list_is    = Typelist<int, std::string>;
    using list_ss    = Typelist<std::string, std::string>;
    using list_ips   = Typelist<int, void*, std::string>;
    using list_iips  = Typelist<int, int, void*, std::string>;
    using list_ipps  = Typelist<int, void*, void*, std::string>;
    using list_ipss  = Typelist<int, void*, std::string, std::string>;

    {  using type = Insert<Nil, int, Compare>;               TEST_TYPE(type, list_i);     }
    {  using type = Insert<list_s, std::string, Compare>;    TEST_TYPE(type, list_ss);    }
    {  using type = Insert<list_s, int, Compare>;            TEST_TYPE(type, list_is);    }
    {  using type = Insert<list_i, std::string, Compare>;    TEST_TYPE(type, list_is);    }
    {  using type = Insert<list_ips, int, Compare>;          TEST_TYPE(type, list_iips);  }
    {  using type = Insert<list_ips, void*, Compare>;        TEST_TYPE(type, list_ipps);  }
    {  using type = Insert<list_ips, std::string, Compare>;  TEST_TYPE(type, list_ipss);  }

}

void test_core_meta_insert_at() {

    using list_c     = Typelist<char>;
    using list_i     = Typelist<int>;
    using list_ci    = Typelist<char, int>;
    using list_ic    = Typelist<int, char>;
    using list_is    = Typelist<int, std::string>;
    using list_cis   = Typelist<char, int, std::string>;
    using list_ics   = Typelist<int, char, std::string>;
    using list_isc   = Typelist<int, std::string, char>;
    using list_isp   = Typelist<int, std::string, void*>;
    using list_cisp  = Typelist<char, int, std::string, void*>;
    using list_icsp  = Typelist<int, char, std::string, void*>;
    using list_iscp  = Typelist<int, std::string, char, void*>;
    using list_ispc  = Typelist<int, std::string, void*, char>;

    {  using type = InsertAt<Nil, char, -1>;       TEST_TYPE(type, list_c);     }
    {  using type = InsertAt<Nil, char, 0>;        TEST_TYPE(type, list_c);     }
    {  using type = InsertAt<Nil, char, 1>;        TEST_TYPE(type, list_c);     }
    {  using type = InsertAt<list_i, char, -1>;    TEST_TYPE(type, list_ci);    }
    {  using type = InsertAt<list_i, char, 0>;     TEST_TYPE(type, list_ci);    }
    {  using type = InsertAt<list_i, char, 1>;     TEST_TYPE(type, list_ic);    }
    {  using type = InsertAt<list_i, char, 2>;     TEST_TYPE(type, list_ic);    }
    {  using type = InsertAt<list_is, char, -1>;   TEST_TYPE(type, list_cis);   }
    {  using type = InsertAt<list_is, char, 0>;    TEST_TYPE(type, list_cis);   }
    {  using type = InsertAt<list_is, char, 1>;    TEST_TYPE(type, list_ics);   }
    {  using type = InsertAt<list_is, char, 2>;    TEST_TYPE(type, list_isc);   }
    {  using type = InsertAt<list_is, char, 3>;    TEST_TYPE(type, list_isc);   }
    {  using type = InsertAt<list_isp, char, -1>;  TEST_TYPE(type, list_cisp);  }
    {  using type = InsertAt<list_isp, char, 0>;   TEST_TYPE(type, list_cisp);  }
    {  using type = InsertAt<list_isp, char, 1>;   TEST_TYPE(type, list_icsp);  }
    {  using type = InsertAt<list_isp, char, 2>;   TEST_TYPE(type, list_iscp);  }
    {  using type = InsertAt<list_isp, char, 3>;   TEST_TYPE(type, list_ispc);  }
    {  using type = InsertAt<list_isp, char, 4>;   TEST_TYPE(type, list_ispc);  }

}

void test_core_meta_most() {

    using list_i    = Typelist<int>;
    using list_is   = Typelist<int, std::string>;
    using list_isp  = Typelist<int, std::string, void*>;

    TEST_TYPE(Most<Nil>,       Nil);
    TEST_TYPE(Most<list_i>,    Nil);
    TEST_TYPE(Most<list_is>,   list_i);
    TEST_TYPE(Most<list_isp>,  list_is);

}

void test_core_meta_prefix() {

    using list_i    = Typelist<int>;
    using list_s    = Typelist<std::string>;
    using list_is   = Typelist<int, std::string>;
    using list_sp   = Typelist<std::string, void*>;
    using list_isp  = Typelist<int, std::string, void*>;

    {  using type = Prefix<int, Nil>;      TEST_TYPE(type, list_i);    }
    {  using type = Prefix<int, list_s>;   TEST_TYPE(type, list_is);   }
    {  using type = Prefix<int, list_sp>;  TEST_TYPE(type, list_isp);  }

}

void test_core_meta_rep_list() {

    using list_s       = Typelist<std::string>;
    using list_sp      = Typelist<std::string, void*>;
    using list_ss      = Typelist<std::string, std::string>;
    using list_sss     = Typelist<std::string, std::string, std::string>;
    using list_spsp    = Typelist<std::string, void*, std::string, void*>;
    using list_spspsp  = Typelist<std::string, void*, std::string, void*, std::string, void*>;

    {  using type = RepList<Nil, 0>;          TEST_TYPE(type, Nil);          }
    {  using type = RepList<Nil, 1>;          TEST_TYPE(type, Nil);          }
    {  using type = RepList<Nil, 2>;          TEST_TYPE(type, Nil);          }
    {  using type = RepList<Nil, 3>;          TEST_TYPE(type, Nil);          }
    {  using type = RepList<std::string, 0>;  TEST_TYPE(type, Nil);          }
    {  using type = RepList<std::string, 1>;  TEST_TYPE(type, list_s);       }
    {  using type = RepList<std::string, 2>;  TEST_TYPE(type, list_ss);      }
    {  using type = RepList<std::string, 3>;  TEST_TYPE(type, list_sss);     }
    {  using type = RepList<list_s, 0>;       TEST_TYPE(type, Nil);          }
    {  using type = RepList<list_s, 1>;       TEST_TYPE(type, list_s);       }
    {  using type = RepList<list_s, 2>;       TEST_TYPE(type, list_ss);      }
    {  using type = RepList<list_s, 3>;       TEST_TYPE(type, list_sss);     }
    {  using type = RepList<list_sp, 0>;      TEST_TYPE(type, Nil);          }
    {  using type = RepList<list_sp, 1>;      TEST_TYPE(type, list_sp);      }
    {  using type = RepList<list_sp, 2>;      TEST_TYPE(type, list_spsp);    }
    {  using type = RepList<list_sp, 3>;      TEST_TYPE(type, list_spspsp);  }

}

void test_core_meta_rep_type() {

    using list_s     = Typelist<std::string>;
    using list_sp    = Typelist<std::string, void*>;
    using list_ss    = Typelist<std::string, std::string>;
    using list_sss   = Typelist<std::string, std::string, std::string>;
    using list_sp_1  = Typelist<list_sp>;
    using list_sp_2  = Typelist<list_sp, list_sp>;
    using list_sp_3  = Typelist<list_sp, list_sp, list_sp>;

    {  using type = RepType<std::string, 0>;  TEST_TYPE(type, Nil);        }
    {  using type = RepType<std::string, 1>;  TEST_TYPE(type, list_s);     }
    {  using type = RepType<std::string, 2>;  TEST_TYPE(type, list_ss);    }
    {  using type = RepType<std::string, 3>;  TEST_TYPE(type, list_sss);   }
    {  using type = RepType<list_sp, 0>;      TEST_TYPE(type, Nil);        }
    {  using type = RepType<list_sp, 1>;      TEST_TYPE(type, list_sp_1);  }
    {  using type = RepType<list_sp, 2>;      TEST_TYPE(type, list_sp_2);  }
    {  using type = RepType<list_sp, 3>;      TEST_TYPE(type, list_sp_3);  }

}

void test_core_meta_resize() {

    using list_c      = Typelist<char>;
    using list_i      = Typelist<int>;
    using list_cc     = Typelist<char, char>;
    using list_ic     = Typelist<int, char>;
    using list_is     = Typelist<int, std::string>;
    using list_icc    = Typelist<int, char, char>;
    using list_isc    = Typelist<int, std::string, char>;
    using list_isp    = Typelist<int, std::string, void*>;
    using list_iscc   = Typelist<int, std::string, char, char>;
    using list_ispc   = Typelist<int, std::string, void*, char>;
    using list_ispcc  = Typelist<int, std::string, void*, char, char>;

    {  using type = Resize<Nil, 0, char>;       TEST_TYPE(type, Nil);         }
    {  using type = Resize<Nil, 1, char>;       TEST_TYPE(type, list_c);      }
    {  using type = Resize<Nil, 2, char>;       TEST_TYPE(type, list_cc);     }
    {  using type = Resize<list_i, 0, char>;    TEST_TYPE(type, Nil);         }
    {  using type = Resize<list_i, 1, char>;    TEST_TYPE(type, list_i);      }
    {  using type = Resize<list_i, 2, char>;    TEST_TYPE(type, list_ic);     }
    {  using type = Resize<list_i, 3, char>;    TEST_TYPE(type, list_icc);    }
    {  using type = Resize<list_is, 0, char>;   TEST_TYPE(type, Nil);         }
    {  using type = Resize<list_is, 1, char>;   TEST_TYPE(type, list_i);      }
    {  using type = Resize<list_is, 2, char>;   TEST_TYPE(type, list_is);     }
    {  using type = Resize<list_is, 3, char>;   TEST_TYPE(type, list_isc);    }
    {  using type = Resize<list_is, 4, char>;   TEST_TYPE(type, list_iscc);   }
    {  using type = Resize<list_isp, 0, char>;  TEST_TYPE(type, Nil);         }
    {  using type = Resize<list_isp, 1, char>;  TEST_TYPE(type, list_i);      }
    {  using type = Resize<list_isp, 2, char>;  TEST_TYPE(type, list_is);     }
    {  using type = Resize<list_isp, 3, char>;  TEST_TYPE(type, list_isp);    }
    {  using type = Resize<list_isp, 4, char>;  TEST_TYPE(type, list_ispc);   }
    {  using type = Resize<list_isp, 5, char>;  TEST_TYPE(type, list_ispcc);  }

}

void test_core_meta_skip() {

    using list_i    = Typelist<int>;
    using list_s    = Typelist<std::string>;
    using list_p    = Typelist<void*>;
    using list_is   = Typelist<int, std::string>;
    using list_sp   = Typelist<std::string, void*>;
    using list_isp  = Typelist<int, std::string, void*>;

    {  using type = Skip<Nil, 0>;       TEST_TYPE(type, Nil);       }
    {  using type = Skip<Nil, 1>;       TEST_TYPE(type, Nil);       }
    {  using type = Skip<Nil, 2>;       TEST_TYPE(type, Nil);       }
    {  using type = Skip<list_i, 0>;    TEST_TYPE(type, list_i);    }
    {  using type = Skip<list_i, 1>;    TEST_TYPE(type, Nil);       }
    {  using type = Skip<list_i, 2>;    TEST_TYPE(type, Nil);       }
    {  using type = Skip<list_is, 0>;   TEST_TYPE(type, list_is);   }
    {  using type = Skip<list_is, 1>;   TEST_TYPE(type, list_s);    }
    {  using type = Skip<list_is, 2>;   TEST_TYPE(type, Nil);       }
    {  using type = Skip<list_is, 3>;   TEST_TYPE(type, Nil);       }
    {  using type = Skip<list_isp, 0>;  TEST_TYPE(type, list_isp);  }
    {  using type = Skip<list_isp, 1>;  TEST_TYPE(type, list_sp);   }
    {  using type = Skip<list_isp, 2>;  TEST_TYPE(type, list_p);    }
    {  using type = Skip<list_isp, 3>;  TEST_TYPE(type, Nil);       }
    {  using type = Skip<list_isp, 4>;  TEST_TYPE(type, Nil);       }

}

void test_core_meta_sublist() {

    using list_i    = Typelist<int>;
    using list_s    = Typelist<std::string>;
    using list_p    = Typelist<void*>;
    using list_is   = Typelist<int, std::string>;
    using list_sp   = Typelist<std::string, void*>;
    using list_isp  = Typelist<int, std::string, void*>;

    {  using type = Sublist<Nil, 0, 0>;       TEST_TYPE(type, Nil);       }
    {  using type = Sublist<Nil, 0, 1>;       TEST_TYPE(type, Nil);       }
    {  using type = Sublist<Nil, 0, 2>;       TEST_TYPE(type, Nil);       }
    {  using type = Sublist<Nil, 1, 0>;       TEST_TYPE(type, Nil);       }
    {  using type = Sublist<Nil, 1, 1>;       TEST_TYPE(type, Nil);       }
    {  using type = Sublist<Nil, 1, 2>;       TEST_TYPE(type, Nil);       }
    {  using type = Sublist<Nil, 2, 0>;       TEST_TYPE(type, Nil);       }
    {  using type = Sublist<Nil, 2, 1>;       TEST_TYPE(type, Nil);       }
    {  using type = Sublist<Nil, 2, 2>;       TEST_TYPE(type, Nil);       }
    {  using type = Sublist<list_i, 0, 0>;    TEST_TYPE(type, Nil);       }
    {  using type = Sublist<list_i, 0, 1>;    TEST_TYPE(type, list_i);    }
    {  using type = Sublist<list_i, 0, 2>;    TEST_TYPE(type, list_i);    }
    {  using type = Sublist<list_i, 1, 0>;    TEST_TYPE(type, Nil);       }
    {  using type = Sublist<list_i, 1, 1>;    TEST_TYPE(type, Nil);       }
    {  using type = Sublist<list_i, 1, 2>;    TEST_TYPE(type, Nil);       }
    {  using type = Sublist<list_i, 2, 0>;    TEST_TYPE(type, Nil);       }
    {  using type = Sublist<list_i, 2, 1>;    TEST_TYPE(type, Nil);       }
    {  using type = Sublist<list_i, 2, 2>;    TEST_TYPE(type, Nil);       }
    {  using type = Sublist<list_is, 0, 0>;   TEST_TYPE(type, Nil);       }
    {  using type = Sublist<list_is, 0, 1>;   TEST_TYPE(type, list_i);    }
    {  using type = Sublist<list_is, 0, 2>;   TEST_TYPE(type, list_is);   }
    {  using type = Sublist<list_is, 0, 3>;   TEST_TYPE(type, list_is);   }
    {  using type = Sublist<list_is, 1, 0>;   TEST_TYPE(type, Nil);       }
    {  using type = Sublist<list_is, 1, 1>;   TEST_TYPE(type, list_s);    }
    {  using type = Sublist<list_is, 1, 2>;   TEST_TYPE(type, list_s);    }
    {  using type = Sublist<list_is, 1, 3>;   TEST_TYPE(type, list_s);    }
    {  using type = Sublist<list_is, 2, 0>;   TEST_TYPE(type, Nil);       }
    {  using type = Sublist<list_is, 2, 1>;   TEST_TYPE(type, Nil);       }
    {  using type = Sublist<list_is, 2, 2>;   TEST_TYPE(type, Nil);       }
    {  using type = Sublist<list_is, 2, 3>;   TEST_TYPE(type, Nil);       }
    {  using type = Sublist<list_is, 3, 0>;   TEST_TYPE(type, Nil);       }
    {  using type = Sublist<list_is, 3, 1>;   TEST_TYPE(type, Nil);       }
    {  using type = Sublist<list_is, 3, 2>;   TEST_TYPE(type, Nil);       }
    {  using type = Sublist<list_is, 3, 3>;   TEST_TYPE(type, Nil);       }
    {  using type = Sublist<list_isp, 0, 0>;  TEST_TYPE(type, Nil);       }
    {  using type = Sublist<list_isp, 0, 1>;  TEST_TYPE(type, list_i);    }
    {  using type = Sublist<list_isp, 0, 2>;  TEST_TYPE(type, list_is);   }
    {  using type = Sublist<list_isp, 0, 3>;  TEST_TYPE(type, list_isp);  }
    {  using type = Sublist<list_isp, 0, 4>;  TEST_TYPE(type, list_isp);  }
    {  using type = Sublist<list_isp, 1, 0>;  TEST_TYPE(type, Nil);       }
    {  using type = Sublist<list_isp, 1, 1>;  TEST_TYPE(type, list_s);    }
    {  using type = Sublist<list_isp, 1, 2>;  TEST_TYPE(type, list_sp);   }
    {  using type = Sublist<list_isp, 1, 3>;  TEST_TYPE(type, list_sp);   }
    {  using type = Sublist<list_isp, 1, 4>;  TEST_TYPE(type, list_sp);   }
    {  using type = Sublist<list_isp, 2, 0>;  TEST_TYPE(type, Nil);       }
    {  using type = Sublist<list_isp, 2, 1>;  TEST_TYPE(type, list_p);    }
    {  using type = Sublist<list_isp, 2, 2>;  TEST_TYPE(type, list_p);    }
    {  using type = Sublist<list_isp, 2, 3>;  TEST_TYPE(type, list_p);    }
    {  using type = Sublist<list_isp, 2, 4>;  TEST_TYPE(type, list_p);    }
    {  using type = Sublist<list_isp, 3, 0>;  TEST_TYPE(type, Nil);       }
    {  using type = Sublist<list_isp, 3, 1>;  TEST_TYPE(type, Nil);       }
    {  using type = Sublist<list_isp, 3, 2>;  TEST_TYPE(type, Nil);       }
    {  using type = Sublist<list_isp, 3, 3>;  TEST_TYPE(type, Nil);       }
    {  using type = Sublist<list_isp, 3, 4>;  TEST_TYPE(type, Nil);       }
    {  using type = Sublist<list_isp, 4, 0>;  TEST_TYPE(type, Nil);       }
    {  using type = Sublist<list_isp, 4, 1>;  TEST_TYPE(type, Nil);       }
    {  using type = Sublist<list_isp, 4, 2>;  TEST_TYPE(type, Nil);       }
    {  using type = Sublist<list_isp, 4, 3>;  TEST_TYPE(type, Nil);       }
    {  using type = Sublist<list_isp, 4, 4>;  TEST_TYPE(type, Nil);       }

}

void test_core_meta_tail() {

    using list_i    = Typelist<int>;
    using list_s    = Typelist<std::string>;
    using list_is   = Typelist<int, std::string>;
    using list_sp   = Typelist<std::string, void*>;
    using list_isp  = Typelist<int, std::string, void*>;

    TEST_TYPE(Tail<Nil>,       Nil);
    TEST_TYPE(Tail<list_i>,    Nil);
    TEST_TYPE(Tail<list_is>,   list_s);
    TEST_TYPE(Tail<list_isp>,  list_sp);

}

void test_core_meta_take() {

    using list_i    = Typelist<int>;
    using list_is   = Typelist<int, std::string>;
    using list_isp  = Typelist<int, std::string, void*>;

    {  using type = Take<Nil, 0>;       TEST_TYPE(type, Nil);       }
    {  using type = Take<Nil, 1>;       TEST_TYPE(type, Nil);       }
    {  using type = Take<Nil, 2>;       TEST_TYPE(type, Nil);       }
    {  using type = Take<list_i, 0>;    TEST_TYPE(type, Nil);       }
    {  using type = Take<list_i, 1>;    TEST_TYPE(type, list_i);    }
    {  using type = Take<list_i, 2>;    TEST_TYPE(type, list_i);    }
    {  using type = Take<list_is, 0>;   TEST_TYPE(type, Nil);       }
    {  using type = Take<list_is, 1>;   TEST_TYPE(type, list_i);    }
    {  using type = Take<list_is, 2>;   TEST_TYPE(type, list_is);   }
    {  using type = Take<list_is, 3>;   TEST_TYPE(type, list_is);   }
    {  using type = Take<list_isp, 0>;  TEST_TYPE(type, Nil);       }
    {  using type = Take<list_isp, 1>;  TEST_TYPE(type, list_i);    }
    {  using type = Take<list_isp, 2>;  TEST_TYPE(type, list_is);   }
    {  using type = Take<list_isp, 3>;  TEST_TYPE(type, list_isp);  }
    {  using type = Take<list_isp, 4>;  TEST_TYPE(type, list_isp);  }

}

void test_core_meta_head() {

    using list_i    = Typelist<int>;
    using list_is   = Typelist<int, std::string>;
    using list_isp  = Typelist<int, std::string, void*>;

    TEST_TYPE(Head<Nil>,       Nil);
    TEST_TYPE(Head<list_i>,    int);
    TEST_TYPE(Head<list_is>,   int);
    TEST_TYPE(Head<list_isp>,  int);

}

void test_core_meta_last() {

    using list_i    = Typelist<int>;
    using list_is   = Typelist<int, std::string>;
    using list_isp  = Typelist<int, std::string, void*>;

    TEST_TYPE(Last<Nil>,       Nil);
    TEST_TYPE(Last<list_i>,    int);
    TEST_TYPE(Last<list_is>,   std::string);
    TEST_TYPE(Last<list_isp>,  void*);

}

void test_core_meta_max_type() {

    using list_i     = Typelist<int>;
    using list_is    = Typelist<int, std::string>;
    using list_isp   = Typelist<int, std::string, void*>;
    using list_isps  = Typelist<int, std::string, void*, std::string>;

    {  using type = MaxType<Nil, Compare>;        TEST_TYPE(type, Nil);          }
    {  using type = MaxType<list_i, Compare>;     TEST_TYPE(type, int);          }
    {  using type = MaxType<list_is, Compare>;    TEST_TYPE(type, std::string);  }
    {  using type = MaxType<list_isp, Compare>;   TEST_TYPE(type, std::string);  }
    {  using type = MaxType<list_isps, Compare>;  TEST_TYPE(type, std::string);  }

    {  using type = MinType<Nil, Compare>;        TEST_TYPE(type, Nil);  }
    {  using type = MinType<list_i, Compare>;     TEST_TYPE(type, int);  }
    {  using type = MinType<list_is, Compare>;    TEST_TYPE(type, int);  }
    {  using type = MinType<list_isp, Compare>;   TEST_TYPE(type, int);  }
    {  using type = MinType<list_isps, Compare>;  TEST_TYPE(type, int);  }

}

void test_core_meta_type_at() {

    using list_i     = Typelist<int>;
    using list_is    = Typelist<int, std::string>;
    using list_isp   = Typelist<int, std::string, void*>;
    using list_isps  = Typelist<int, std::string, void*, std::string>;

    {  using type = TypeAt<Nil, -1>;        TEST_TYPE(type, Nil);          }
    {  using type = TypeAt<Nil, 0>;         TEST_TYPE(type, Nil);          }
    {  using type = TypeAt<Nil, 1>;         TEST_TYPE(type, Nil);          }
    {  using type = TypeAt<list_i, -1>;     TEST_TYPE(type, Nil);          }
    {  using type = TypeAt<list_i, 0>;      TEST_TYPE(type, int);          }
    {  using type = TypeAt<list_i, 1>;      TEST_TYPE(type, Nil);          }
    {  using type = TypeAt<list_i, 2>;      TEST_TYPE(type, Nil);          }
    {  using type = TypeAt<list_is, -1>;    TEST_TYPE(type, Nil);          }
    {  using type = TypeAt<list_is, 0>;     TEST_TYPE(type, int);          }
    {  using type = TypeAt<list_is, 1>;     TEST_TYPE(type, std::string);  }
    {  using type = TypeAt<list_is, 2>;     TEST_TYPE(type, Nil);          }
    {  using type = TypeAt<list_is, 3>;     TEST_TYPE(type, Nil);          }
    {  using type = TypeAt<list_isp, -1>;   TEST_TYPE(type, Nil);          }
    {  using type = TypeAt<list_isp, 0>;    TEST_TYPE(type, int);          }
    {  using type = TypeAt<list_isp, 1>;    TEST_TYPE(type, std::string);  }
    {  using type = TypeAt<list_isp, 2>;    TEST_TYPE(type, void*);        }
    {  using type = TypeAt<list_isp, 3>;    TEST_TYPE(type, Nil);          }
    {  using type = TypeAt<list_isp, 4>;    TEST_TYPE(type, Nil);          }
    {  using type = TypeAt<list_isps, -1>;  TEST_TYPE(type, Nil);          }
    {  using type = TypeAt<list_isps, 0>;   TEST_TYPE(type, int);          }
    {  using type = TypeAt<list_isps, 1>;   TEST_TYPE(type, std::string);  }
    {  using type = TypeAt<list_isps, 2>;   TEST_TYPE(type, void*);        }
    {  using type = TypeAt<list_isps, 3>;   TEST_TYPE(type, std::string);  }
    {  using type = TypeAt<list_isps, 4>;   TEST_TYPE(type, Nil);          }
    {  using type = TypeAt<list_isps, 5>;   TEST_TYPE(type, Nil);          }

}

void test_core_meta_fold() {

    using list_i     = Typelist<int>;
    using list_is    = Typelist<int, std::string>;
    using list_isp   = Typelist<int, std::string, void*>;
    using list_isps  = Typelist<int, std::string, void*, std::string>;

    TEST_EQUAL((Pattern<FoldLeft<Nil, MakePair, Nil>>::str()),        "*");
    TEST_EQUAL((Pattern<FoldLeft<list_i, MakePair, Nil>>::str()),     "(*i)");
    TEST_EQUAL((Pattern<FoldLeft<list_is, MakePair, Nil>>::str()),    "((*i)s)");
    TEST_EQUAL((Pattern<FoldLeft<list_isp, MakePair, Nil>>::str()),   "(((*i)s)p)");
    TEST_EQUAL((Pattern<FoldLeft<list_isps, MakePair, Nil>>::str()),  "((((*i)s)p)s)");

    TEST_EQUAL((Pattern<FoldRight<Nil, MakePair, Nil>>::str()),        "*");
    TEST_EQUAL((Pattern<FoldRight<list_i, MakePair, Nil>>::str()),     "(i*)");
    TEST_EQUAL((Pattern<FoldRight<list_is, MakePair, Nil>>::str()),    "(i(s*))");
    TEST_EQUAL((Pattern<FoldRight<list_isp, MakePair, Nil>>::str()),   "(i(s(p*)))");
    TEST_EQUAL((Pattern<FoldRight<list_isps, MakePair, Nil>>::str()),  "(i(s(p(s*))))");

    TEST_EQUAL((Pattern<Reduce<Nil, MakePair>>::str()),        "*");
    TEST_EQUAL((Pattern<Reduce<list_i, MakePair>>::str()),     "i");
    TEST_EQUAL((Pattern<Reduce<list_is, MakePair>>::str()),    "(is)");
    TEST_EQUAL((Pattern<Reduce<list_isp, MakePair>>::str()),   "((is)p)");
    TEST_EQUAL((Pattern<Reduce<list_isps, MakePair>>::str()),  "(((is)p)s)");

}

void test_core_meta_make_set() {

    using list_i     = Typelist<int>;
    using list_ips   = Typelist<int, void*, std::string>;
    using list_isp   = Typelist<int, std::string, void*>;
    using list_iisp  = Typelist<int, int, std::string, void*>;
    using list_issp  = Typelist<int, std::string, std::string, void*>;
    using list_ispp  = Typelist<int, std::string, void*, void*>;

    {  using type = MakeSet<Nil, Compare>;        TEST_TYPE(type, Nil);       }
    {  using type = MakeSet<list_i, Compare>;     TEST_TYPE(type, list_i);    }
    {  using type = MakeSet<list_isp, Compare>;   TEST_TYPE(type, list_ips);  }
    {  using type = MakeSet<list_iisp, Compare>;  TEST_TYPE(type, list_ips);  }
    {  using type = MakeSet<list_issp, Compare>;  TEST_TYPE(type, list_ips);  }
    {  using type = MakeSet<list_ispp, Compare>;  TEST_TYPE(type, list_ips);  }

}

void test_core_meta_map() {

    using list_i     = Typelist<int>;
    using list_is    = Typelist<int, std::string>;
    using list_isp   = Typelist<int, std::string, void*>;
    using list_isps  = Typelist<int, std::string, void*, std::string>;
    using ptrs_i     = Typelist<int*>;
    using ptrs_is    = Typelist<int*, std::string*>;
    using ptrs_isp   = Typelist<int*, std::string*, void**>;
    using ptrs_isps  = Typelist<int*, std::string*, void**, std::string*>;

    {  using type = Map<Nil, MakePtr>;        TEST_TYPE(type, Nil);        }
    {  using type = Map<list_i, MakePtr>;     TEST_TYPE(type, ptrs_i);     }
    {  using type = Map<list_is, MakePtr>;    TEST_TYPE(type, ptrs_is);    }
    {  using type = Map<list_isp, MakePtr>;   TEST_TYPE(type, ptrs_isp);   }
    {  using type = Map<list_isps, MakePtr>;  TEST_TYPE(type, ptrs_isps);  }

}

void test_core_meta_partial_reduce() {

    using list_i      = Typelist<int>;
    using list_s      = Typelist<std::string>;
    using list_ss     = Typelist<std::string, std::string>;
    using list_isp    = Typelist<int, std::string, void*>;
    using list_iisp   = Typelist<int, int, std::string, void*>;
    using list_ispp   = Typelist<int, std::string, void*, void*>;
    using list_issp   = Typelist<int, std::string, std::string, void*>;
    using list_spsp   = Typelist<std::string, void*, std::string, void*>;
    using list_iiisp  = Typelist<int, int, int, std::string, void*>;
    using list_isppp  = Typelist<int, std::string, void*, void*, void*>;
    using list_isssp  = Typelist<int, std::string, std::string, std::string, void*>;

    {  using type = PartialReduce<Nil, std::is_same, Unify>;         TEST_TYPE(type, Nil);        }
    {  using type = PartialReduce<list_i, std::is_same, Unify>;      TEST_TYPE(type, list_i);     }
    {  using type = PartialReduce<list_ss, std::is_same, Unify>;     TEST_TYPE(type, list_s);     }
    {  using type = PartialReduce<list_isp, std::is_same, Unify>;    TEST_TYPE(type, list_isp);   }
    {  using type = PartialReduce<list_iisp, std::is_same, Unify>;   TEST_TYPE(type, list_isp);   }
    {  using type = PartialReduce<list_iiisp, std::is_same, Unify>;  TEST_TYPE(type, list_isp);   }
    {  using type = PartialReduce<list_issp, std::is_same, Unify>;   TEST_TYPE(type, list_isp);   }
    {  using type = PartialReduce<list_isssp, std::is_same, Unify>;  TEST_TYPE(type, list_isp);   }
    {  using type = PartialReduce<list_ispp, std::is_same, Unify>;   TEST_TYPE(type, list_isp);   }
    {  using type = PartialReduce<list_isppp, std::is_same, Unify>;  TEST_TYPE(type, list_isp);   }
    {  using type = PartialReduce<list_spsp, std::is_same, Unify>;   TEST_TYPE(type, list_spsp);  }

}

void test_core_meta_remove() {

    using list_i     = Typelist<int>;
    using list_s     = Typelist<std::string>;
    using list_ip    = Typelist<int, void*>;
    using list_is    = Typelist<int, std::string>;
    using list_ss    = Typelist<std::string, std::string>;
    using list_sps   = Typelist<std::string, void*, std::string>;
    using list_isps  = Typelist<int, std::string, void*, std::string>;

    {  using type = Remove<Nil, int>;                TEST_TYPE(type, Nil);        }
    {  using type = Remove<list_i, int>;             TEST_TYPE(type, Nil);        }
    {  using type = Remove<list_s, int>;             TEST_TYPE(type, list_s);     }
    {  using type = Remove<list_is, int>;            TEST_TYPE(type, list_s);     }
    {  using type = Remove<list_ss, int>;            TEST_TYPE(type, list_ss);    }
    {  using type = Remove<list_ss, std::string>;    TEST_TYPE(type, Nil);        }
    {  using type = Remove<list_isps, int>;          TEST_TYPE(type, list_sps);   }
    {  using type = Remove<list_isps, std::string>;  TEST_TYPE(type, list_ip);    }
    {  using type = Remove<list_isps, float>;        TEST_TYPE(type, list_isps);  }

    {  using type = RemoveIf<Nil, std::is_scalar>;        TEST_TYPE(type, Nil);      }
    {  using type = RemoveIf<list_i, std::is_scalar>;     TEST_TYPE(type, Nil);      }
    {  using type = RemoveIf<list_s, std::is_scalar>;     TEST_TYPE(type, list_s);   }
    {  using type = RemoveIf<list_is, std::is_scalar>;    TEST_TYPE(type, list_s);   }
    {  using type = RemoveIf<list_ss, std::is_scalar>;    TEST_TYPE(type, list_ss);  }
    {  using type = RemoveIf<list_isps, std::is_scalar>;  TEST_TYPE(type, list_ss);  }

}

void test_core_meta_reverse() {

    using list_i     = Typelist<int>;
    using list_is    = Typelist<int, std::string>;
    using list_ps    = Typelist<void*, std::string>;
    using list_si    = Typelist<std::string, int>;
    using list_sp    = Typelist<std::string, void*>;
    using list_isp   = Typelist<int, std::string, void*>;
    using list_psi   = Typelist<void*, std::string, int>;
    using list_isps  = Typelist<int, std::string, void*, std::string>;
    using list_spsi  = Typelist<std::string, void*, std::string, int>;

    TEST_TYPE(Reverse<Nil>,        Nil);
    TEST_TYPE(Reverse<list_i>,     list_i);
    TEST_TYPE(Reverse<list_is>,    list_si);
    TEST_TYPE(Reverse<list_sp>,    list_ps);
    TEST_TYPE(Reverse<list_isp>,   list_psi);
    TEST_TYPE(Reverse<list_isps>,  list_spsi);

}

void test_core_meta_select() {

    using list_i     = Typelist<int>;
    using list_s     = Typelist<std::string>;
    using list_ip    = Typelist<int, void*>;
    using list_is    = Typelist<int, std::string>;
    using list_ss    = Typelist<std::string, std::string>;
    using list_isps  = Typelist<int, std::string, void*, std::string>;

    {  using type = Select<Nil, std::is_scalar>;        TEST_TYPE(type, Nil);      }
    {  using type = Select<list_i, std::is_scalar>;     TEST_TYPE(type, list_i);   }
    {  using type = Select<list_s, std::is_scalar>;     TEST_TYPE(type, Nil);      }
    {  using type = Select<list_is, std::is_scalar>;    TEST_TYPE(type, list_i);   }
    {  using type = Select<list_ss, std::is_scalar>;    TEST_TYPE(type, Nil);      }
    {  using type = Select<list_isps, std::is_scalar>;  TEST_TYPE(type, list_ip);  }

}

void test_core_meta_sort() {

    using list_i     = Typelist<int>;
    using list_ips   = Typelist<int, void*, std::string>;
    using list_isp   = Typelist<int, std::string, void*>;
    using list_iips  = Typelist<int, int, void*, std::string>;
    using list_iisp  = Typelist<int, int, std::string, void*>;
    using list_ipps  = Typelist<int, void*, void*, std::string>;
    using list_ipss  = Typelist<int, void*, std::string, std::string>;
    using list_ispp  = Typelist<int, std::string, void*, void*>;
    using list_issp  = Typelist<int, std::string, std::string, void*>;
    using list_pqr   = Typelist<void*, char*, int*>;
    using list_prq   = Typelist<void*, int*, char*>;
    using list_qpr   = Typelist<char*, void*, int*>;
    using list_qrp   = Typelist<char*, int*, void*>;
    using list_rpq   = Typelist<int*, void*, char*>;
    using list_rqp   = Typelist<int*, char*, void*>;

    {  using type = Sort<Nil, Compare>;        TEST_TYPE(type, Nil);        }
    {  using type = Sort<list_i, Compare>;     TEST_TYPE(type, list_i);     }
    {  using type = Sort<list_isp, Compare>;   TEST_TYPE(type, list_ips);   }
    {  using type = Sort<list_iisp, Compare>;  TEST_TYPE(type, list_iips);  }
    {  using type = Sort<list_issp, Compare>;  TEST_TYPE(type, list_ipss);  }
    {  using type = Sort<list_ispp, Compare>;  TEST_TYPE(type, list_ipps);  }
    {  using type = Sort<list_pqr, Compare>;   TEST_TYPE(type, list_pqr);   }
    {  using type = Sort<list_prq, Compare>;   TEST_TYPE(type, list_prq);   }
    {  using type = Sort<list_qpr, Compare>;   TEST_TYPE(type, list_qpr);   }
    {  using type = Sort<list_qrp, Compare>;   TEST_TYPE(type, list_qrp);   }
    {  using type = Sort<list_rpq, Compare>;   TEST_TYPE(type, list_rpq);   }
    {  using type = Sort<list_rqp, Compare>;   TEST_TYPE(type, list_rqp);   }

}

void test_core_meta_unique() {

    using list_i       = Typelist<int>;
    using list_s       = Typelist<std::string>;
    using list_sp      = Typelist<std::string, void*>;
    using list_ss      = Typelist<std::string, std::string>;
    using list_isp     = Typelist<int, std::string, void*>;
    using list_iisp    = Typelist<int, int, std::string, void*>;
    using list_ispp    = Typelist<int, std::string, void*, void*>;
    using list_issp    = Typelist<int, std::string, std::string, void*>;
    using list_spsp    = Typelist<std::string, void*, std::string, void*>;
    using list_iiisp   = Typelist<int, int, int, std::string, void*>;
    using list_isppp   = Typelist<int, std::string, void*, void*, void*>;
    using list_isssp   = Typelist<int, std::string, std::string, std::string, void*>;
    using list_ispisp  = Typelist<int, std::string, void*, int, std::string, void*>;

    TEST_TYPE(Unique<Nil>,          Nil);
    TEST_TYPE(Unique<list_i>,       list_i);
    TEST_TYPE(Unique<list_ss>,      list_s);
    TEST_TYPE(Unique<list_isp>,     list_isp);
    TEST_TYPE(Unique<list_iisp>,    list_isp);
    TEST_TYPE(Unique<list_iiisp>,   list_isp);
    TEST_TYPE(Unique<list_issp>,    list_isp);
    TEST_TYPE(Unique<list_isssp>,   list_isp);
    TEST_TYPE(Unique<list_ispp>,    list_isp);
    TEST_TYPE(Unique<list_isppp>,   list_isp);
    TEST_TYPE(Unique<list_spsp>,    list_spsp);
    TEST_TYPE(Unique<list_ispisp>,  list_ispisp);

    TEST_TYPE(FullyUnique<Nil>,          Nil);
    TEST_TYPE(FullyUnique<list_i>,       list_i);
    TEST_TYPE(FullyUnique<list_ss>,      list_s);
    TEST_TYPE(FullyUnique<list_isp>,     list_isp);
    TEST_TYPE(FullyUnique<list_iisp>,    list_isp);
    TEST_TYPE(FullyUnique<list_iiisp>,   list_isp);
    TEST_TYPE(FullyUnique<list_issp>,    list_isp);
    TEST_TYPE(FullyUnique<list_isssp>,   list_isp);
    TEST_TYPE(FullyUnique<list_ispp>,    list_isp);
    TEST_TYPE(FullyUnique<list_isppp>,   list_isp);
    TEST_TYPE(FullyUnique<list_spsp>,    list_sp);
    TEST_TYPE(FullyUnique<list_ispisp>,  list_isp);

}

void test_core_meta_zip() {

    using list_i       = Typelist<int>;
    using list_s       = Typelist<std::string>;
    using list_is      = Typelist<int, std::string>;
    using list_sp      = Typelist<std::string, void*>;
    using list_isp     = Typelist<int, std::string, void*>;
    using list_spi     = Typelist<std::string, void*, int>;
    using zip_i_s      = Typelist<MakePair<int, std::string>>;
    using zip_is_sp    = Typelist<MakePair<int, std::string>, MakePair<std::string, void*>>;
    using zip_isp_spi  = Typelist<MakePair<int, std::string>, MakePair<std::string, void*>, MakePair<void*, int>>;

    {  using type = Zip<Nil, Nil, MakePair>;            TEST_TYPE(type, Nil);          }
    {  using type = Zip<list_i, list_s, MakePair>;      TEST_TYPE(type, zip_i_s);      }
    {  using type = Zip<list_is, list_sp, MakePair>;    TEST_TYPE(type, zip_is_sp);    }
    {  using type = Zip<list_isp, list_spi, MakePair>;  TEST_TYPE(type, zip_isp_spi);  }
    {  using type = Zip<Nil, list_s, MakePair>;         TEST_TYPE(type, Nil);          }
    {  using type = Zip<list_i, Nil, MakePair>;         TEST_TYPE(type, Nil);          }
    {  using type = Zip<Nil, list_spi, MakePair>;       TEST_TYPE(type, Nil);          }
    {  using type = Zip<list_isp, Nil, MakePair>;       TEST_TYPE(type, Nil);          }
    {  using type = Zip<list_is, list_spi, MakePair>;   TEST_TYPE(type, zip_is_sp);    }
    {  using type = Zip<list_isp, list_sp, MakePair>;   TEST_TYPE(type, zip_is_sp);    }

}

void test_core_meta_inherit() {

    using list_s       = Typelist<std::string>;
    using list_ss      = Typelist<std::string, std::string>;
    using list_sw      = Typelist<std::string, std::wstring>;
    using list_sss     = Typelist<std::string, std::string, std::string>;
    using list_ssw     = Typelist<std::string, std::string, std::wstring>;
    using list_sws     = Typelist<std::string, std::wstring, std::string>;
    using list_wss     = Typelist<std::wstring, std::string, std::string>;
    using list_swswsw  = Typelist<std::string, std::wstring, std::string, std::wstring, std::string, std::wstring>;

    {  using type = InheritTypelist<Nil>;          TEST((! std::is_base_of<std::string, type>::value));  TEST((! std::is_base_of<std::wstring, type>::value));  }
    {  using type = InheritTypelist<list_s>;       TEST((std::is_base_of<std::string, type>::value));    TEST((! std::is_base_of<std::wstring, type>::value));  }
    {  using type = InheritTypelist<list_ss>;      TEST((std::is_base_of<std::string, type>::value));    TEST((! std::is_base_of<std::wstring, type>::value));  }
    {  using type = InheritTypelist<list_sw>;      TEST((std::is_base_of<std::string, type>::value));    TEST((std::is_base_of<std::wstring, type>::value));    }
    {  using type = InheritTypelist<list_sss>;     TEST((std::is_base_of<std::string, type>::value));    TEST((! std::is_base_of<std::wstring, type>::value));  }
    {  using type = InheritTypelist<list_ssw>;     TEST((std::is_base_of<std::string, type>::value));    TEST((std::is_base_of<std::wstring, type>::value));    }
    {  using type = InheritTypelist<list_sws>;     TEST((std::is_base_of<std::string, type>::value));    TEST((std::is_base_of<std::wstring, type>::value));    }
    {  using type = InheritTypelist<list_wss>;     TEST((std::is_base_of<std::string, type>::value));    TEST((std::is_base_of<std::wstring, type>::value));    }
    {  using type = InheritTypelist<list_swswsw>;  TEST((std::is_base_of<std::string, type>::value));    TEST((std::is_base_of<std::wstring, type>::value));    }

}

void test_core_meta_tuples() {

    using list_i     = Typelist<int>;
    using list_is    = Typelist<int, std::string>;
    using list_isp   = Typelist<int, std::string, void*>;
    using pair_is    = std::pair<int, std::string>;
    using tuple_0    = std::tuple<>;
    using tuple_i    = std::tuple<int>;
    using tuple_is   = std::tuple<int, std::string>;
    using tuple_isp  = std::tuple<int, std::string, void*>;

    { using type = TypelistToTuple<Nil>;        TEST_TYPE(type, tuple_0);    }
    { using type = TypelistToTuple<list_i>;     TEST_TYPE(type, tuple_i);    }
    { using type = TypelistToTuple<list_is>;    TEST_TYPE(type, tuple_is);   }
    { using type = TypelistToTuple<list_isp>;   TEST_TYPE(type, tuple_isp);  }
    { using type = TupleToTypelist<tuple_0>;    TEST_TYPE(type, Nil);        }
    { using type = TupleToTypelist<tuple_i>;    TEST_TYPE(type, list_i);     }
    { using type = TupleToTypelist<tuple_is>;   TEST_TYPE(type, list_is);    }
    { using type = TupleToTypelist<tuple_isp>;  TEST_TYPE(type, list_isp);   }
    { using type = TupleToTypelist<pair_is>;    TEST_TYPE(type, list_is);    }

}

void test_core_meta_all_of() {

    using list_ints  = Typelist<short, int, long>;
    using list_strs  = Typelist<std::string, std::wstring>;
    using list_both  = Typelist<short, int, std::string, long>;

    TEST((all_of<Nil, std::is_scalar>));
    TEST((all_of<list_ints, std::is_scalar>));
    TEST((! all_of<list_strs, std::is_scalar>));
    TEST((! all_of<list_both, std::is_scalar>));

    TEST((! any_of<Nil, std::is_scalar>));
    TEST((any_of<list_ints, std::is_scalar>));
    TEST((! any_of<list_strs, std::is_scalar>));
    TEST((any_of<list_both, std::is_scalar>));

    TEST((none_of<Nil, std::is_scalar>));
    TEST((! none_of<list_ints, std::is_scalar>));
    TEST((none_of<list_strs, std::is_scalar>));
    TEST((! none_of<list_both, std::is_scalar>));

}

void test_core_meta_count() {

    using list_i     = Typelist<int>;
    using list_is    = Typelist<int, std::string>;
    using list_isp   = Typelist<int, std::string, void*>;
    using list_isps  = Typelist<int, std::string, void*, std::string>;

    TEST_EQUAL((count_if<Nil, std::is_scalar>),        0);
    TEST_EQUAL((count_if<list_i, std::is_scalar>),     1);
    TEST_EQUAL((count_if<list_is, std::is_scalar>),    1);
    TEST_EQUAL((count_if<list_isp, std::is_scalar>),   2);
    TEST_EQUAL((count_if<list_isps, std::is_scalar>),  2);

    TEST_EQUAL((count_if_not<Nil, std::is_scalar>),        0);
    TEST_EQUAL((count_if_not<list_i, std::is_scalar>),     0);
    TEST_EQUAL((count_if_not<list_is, std::is_scalar>),    1);
    TEST_EQUAL((count_if_not<list_isp, std::is_scalar>),   1);
    TEST_EQUAL((count_if_not<list_isps, std::is_scalar>),  2);

    TEST_EQUAL((count_type<Nil, std::string>),        0);
    TEST_EQUAL((count_type<list_i, std::string>),     0);
    TEST_EQUAL((count_type<list_is, std::string>),    1);
    TEST_EQUAL((count_type<list_isp, std::string>),   1);
    TEST_EQUAL((count_type<list_isps, std::string>),  2);

}

void test_core_meta_find() {

    using list_i     = Typelist<int>;
    using list_s     = Typelist<std::string>;
    using list_is    = Typelist<int, std::string>;
    using list_si    = Typelist<std::string, int>;
    using list_sss   = Typelist<std::string, std::string, std::string>;
    using list_iisp  = Typelist<int, int, std::string, void*>;
    using list_issp  = Typelist<int, std::string, std::string, void*>;

    TEST_EQUAL((find_if<Nil, std::is_scalar>),       -1);
    TEST_EQUAL((find_if<list_i, std::is_scalar>),    0);
    TEST_EQUAL((find_if<list_s, std::is_scalar>),    -1);
    TEST_EQUAL((find_if<list_is, std::is_scalar>),   0);
    TEST_EQUAL((find_if<list_si, std::is_scalar>),   1);
    TEST_EQUAL((find_if<list_sss, std::is_scalar>),  -1);

    TEST_EQUAL((find_if_not<Nil, std::is_scalar>),        -1);
    TEST_EQUAL((find_if_not<list_i, std::is_scalar>),     -1);
    TEST_EQUAL((find_if_not<list_s, std::is_scalar>),     0);
    TEST_EQUAL((find_if_not<list_is, std::is_scalar>),    1);
    TEST_EQUAL((find_if_not<list_si, std::is_scalar>),    0);
    TEST_EQUAL((find_if_not<list_iisp, std::is_scalar>),  2);

    TEST_EQUAL((find_type<Nil, int>),                -1);
    TEST_EQUAL((find_type<list_i, int>),             0);
    TEST_EQUAL((find_type<list_i, std::string>),     -1);
    TEST_EQUAL((find_type<list_is, int>),            0);
    TEST_EQUAL((find_type<list_is, std::string>),    1);
    TEST_EQUAL((find_type<list_is, void*>),          -1);
    TEST_EQUAL((find_type<list_issp, int>),          0);
    TEST_EQUAL((find_type<list_issp, std::string>),  1);
    TEST_EQUAL((find_type<list_issp, void*>),        3);
    TEST_EQUAL((find_type<list_issp, char>),         -1);

}

void test_core_meta_in_list() {

    using list_i     = Typelist<int>;
    using list_is    = Typelist<int, std::string>;
    using list_issp  = Typelist<int, std::string, std::string, void*>;

    TEST((! in_list<Nil, int>));
    TEST((in_list<list_i, int>));
    TEST((! in_list<list_i, std::string>));
    TEST((in_list<list_is, int>));
    TEST((in_list<list_is, std::string>));
    TEST((! in_list<list_is, void*>));
    TEST((in_list<list_issp, int>));
    TEST((in_list<list_issp, std::string>));
    TEST((in_list<list_issp, void*>));
    TEST((! in_list<list_issp, char>));

}

void test_core_meta_is_empty() {

    using list_i     = Typelist<int>;
    using list_is    = Typelist<int, std::string>;
    using list_isp   = Typelist<int, std::string, void*>;
    using list_isps  = Typelist<int, std::string, void*, std::string>;

    TEST(is_empty<Nil>);
    TEST(! is_empty<list_i>);
    TEST(! is_empty<list_is>);
    TEST(! is_empty<list_isp>);
    TEST(! is_empty<list_isps>);

    TEST(! is_not_empty<Nil>);
    TEST(is_not_empty<list_i>);
    TEST(is_not_empty<list_is>);
    TEST(! is_not_empty<int>);

}

void test_core_meta_is_unique() {

    using list_i     = Typelist<int>;
    using list_ii    = Typelist<int, int>;
    using list_is    = Typelist<int, std::string>;
    using list_isp   = Typelist<int, std::string, void*>;
    using list_iis   = Typelist<int, int, std::string>;
    using list_isi   = Typelist<int, std::string, int>;
    using list_sii   = Typelist<std::string, int, int>;
    using list_ipsw  = Typelist<int, void*, std::string, std::wstring>;
    using list_isps  = Typelist<int, std::string, void*, std::string>;
    using list_issp  = Typelist<int, std::string, std::string, void*>;
    using list_sips  = Typelist<std::string, int, void*, std::string>;
    using list_sisp  = Typelist<std::string, int, std::string, void*>;
    using list_ipss  = Typelist<int, void*, std::string, std::string>;
    using list_ssip  = Typelist<std::string, std::string, int, void*>;

    TEST(is_unique<Nil>);
    TEST(is_unique<list_i>);
    TEST(! is_unique<list_ii>);
    TEST(is_unique<list_is>);
    TEST(is_unique<list_isp>);
    TEST(! is_unique<list_iis>);
    TEST(! is_unique<list_isi>);
    TEST(! is_unique<list_sii>);
    TEST(is_unique<list_ipsw>);
    TEST(! is_unique<list_isps>);
    TEST(! is_unique<list_issp>);
    TEST(! is_unique<list_sips>);
    TEST(! is_unique<list_sisp>);
    TEST(! is_unique<list_ipss>);
    TEST(! is_unique<list_ssip>);

}

void test_core_meta_length_of() {

    using list_i     = Typelist<int>;
    using list_is    = Typelist<int, std::string>;
    using list_isp   = Typelist<int, std::string, void*>;
    using list_isps  = Typelist<int, std::string, void*, std::string>;

    TEST_EQUAL(length_of<Nil>,        0);
    TEST_EQUAL(length_of<list_i>,     1);
    TEST_EQUAL(length_of<list_is>,    2);
    TEST_EQUAL(length_of<list_isp>,   3);
    TEST_EQUAL(length_of<list_isps>,  4);

}

void test_core_meta_typelist_function_call() {

    thing_str.clear();
    TRY(call_for_each<ThingList>());
    TEST_EQUAL(thing_str, "A; A[]; B; B[]; C; C[]; ");

    thing_str.clear();
    TRY(call_for_each<ThingList>("x", "y"));
    TEST_EQUAL(thing_str, "A(x,y); A[x,y]; B(x,y); B[x,y]; C(x,y); C[x,y]; ");

    thing_str.clear();
    TRY(call_for_each_with<ThingList>());
    TEST_EQUAL(thing_str, "A; A[]; B; B[]; C; C[]; ");

    thing_str.clear();
    TRY(call_for_each_with<ThingList>("x", "y"));
    TEST_EQUAL(thing_str, "A; A[x,y]; B; B[x,y]; C; C[x,y]; ");

    thing_str.clear();
    TRY(function_for_each<ThingList>(ThingF1()));
    TEST_EQUAL(thing_str, "A; F1 A[]; B; F1 B[]; C; F1 C[]; ");

    thing_str.clear();
    TRY(function_for_each<ThingList>(ThingF1(), "x", "y"));
    TEST_EQUAL(thing_str, "A(x,y); F1 A[x,y]; B(x,y); F1 B[x,y]; C(x,y); F1 C[x,y]; ");

    thing_str.clear();
    TRY(function_for_each_with<ThingList>(ThingF1()));
    TEST_EQUAL(thing_str, "A; F1 A[]; B; F1 B[]; C; F1 C[]; ");

    thing_str.clear();
    TRY(function_for_each_with<ThingList>(ThingF2(), "x", "y"));
    TEST_EQUAL(thing_str, "A; F2 A[] [x,y]; B; F2 B[] [x,y]; C; F2 C[] [x,y]; ");

}

void test_core_meta_operator_detection() {

    TEST((! has_pre_increment_operator<bool>));     // ++x
    TEST((! has_pre_decrement_operator<bool>));     // --x
    TEST((! has_post_increment_operator<bool>));    // x++
    TEST((! has_post_decrement_operator<bool>));    // x--
    TEST((has_unary_plus_operator<bool>));          // +x
    TEST((has_unary_minus_operator<bool>));         // -x
    TEST((! has_dereference_operator<bool>));       // *x
    TEST((has_logical_not_operator<bool>));         // !x
    TEST((has_bitwise_not_operator<bool>));         // ~x
    TEST((has_plus_operator<bool>));                // x+y
    TEST((has_minus_operator<bool>));               // x-y
    TEST((has_multiply_operator<bool>));            // x*y
    TEST((has_divide_operator<bool>));              // x/y
    TEST((has_remainder_operator<bool>));           // x%y
    TEST((has_bitwise_and_operator<bool>));         // x&y
    TEST((has_bitwise_xor_operator<bool>));         // x^y
    TEST((has_bitwise_or_operator<bool>));          // x|y
    TEST((has_left_shift_operator<bool>));          // x<<y
    TEST((has_right_shift_operator<bool>));         // x>>y
    TEST((has_equal_operator<bool>));               // x==y
    TEST((has_not_equal_operator<bool>));           // x!=y
    TEST((has_less_than_operator<bool>));           // x<y
    TEST((has_greater_than_operator<bool>));        // x>y
    TEST((has_less_or_equal_operator<bool>));       // x<=y
    TEST((has_greater_or_equal_operator<bool>));    // x>=y
    TEST((has_logical_and_operator<bool>));         // x&&y
    TEST((has_logical_or_operator<bool>));          // x||y
    TEST((has_assign_operator<bool>));              // x=y
    TEST((has_plus_assign_operator<bool>));         // x+=y
    TEST((has_minus_assign_operator<bool>));        // x-=y
    TEST((has_multiply_assign_operator<bool>));     // x*=y
    TEST((has_divide_assign_operator<bool>));       // x/=y
    TEST((has_remainder_assign_operator<bool>));    // x%=y
    TEST((has_bitwise_and_assign_operator<bool>));  // x&=y
    TEST((has_bitwise_xor_assign_operator<bool>));  // x^=y
    TEST((has_bitwise_or_assign_operator<bool>));   // x|=y
    TEST((has_right_shift_assign_operator<bool>));  // x>>=y
    TEST((has_left_shift_assign_operator<bool>));   // x<<=y
    TEST((! has_index_operator<bool, int>));        // x[y]

    TEST((has_pre_increment_operator<int>));       // ++x
    TEST((has_pre_decrement_operator<int>));       // --x
    TEST((has_post_increment_operator<int>));      // x++
    TEST((has_post_decrement_operator<int>));      // x--
    TEST((has_unary_plus_operator<int>));          // +x
    TEST((has_unary_minus_operator<int>));         // -x
    TEST((! has_dereference_operator<int>));       // *x
    TEST((has_logical_not_operator<int>));         // !x
    TEST((has_bitwise_not_operator<int>));         // ~x
    TEST((has_plus_operator<int>));                // x+y
    TEST((has_minus_operator<int>));               // x-y
    TEST((has_multiply_operator<int>));            // x*y
    TEST((has_divide_operator<int>));              // x/y
    TEST((has_remainder_operator<int>));           // x%y
    TEST((has_bitwise_and_operator<int>));         // x&y
    TEST((has_bitwise_xor_operator<int>));         // x^y
    TEST((has_bitwise_or_operator<int>));          // x|y
    TEST((has_left_shift_operator<int>));          // x<<y
    TEST((has_right_shift_operator<int>));         // x>>y
    TEST((has_equal_operator<int>));               // x==y
    TEST((has_not_equal_operator<int>));           // x!=y
    TEST((has_less_than_operator<int>));           // x<y
    TEST((has_greater_than_operator<int>));        // x>y
    TEST((has_less_or_equal_operator<int>));       // x<=y
    TEST((has_greater_or_equal_operator<int>));    // x>=y
    TEST((has_logical_and_operator<int>));         // x&&y
    TEST((has_logical_or_operator<int>));          // x||y
    TEST((has_assign_operator<int>));              // x=y
    TEST((has_plus_assign_operator<int>));         // x+=y
    TEST((has_minus_assign_operator<int>));        // x-=y
    TEST((has_multiply_assign_operator<int>));     // x*=y
    TEST((has_divide_assign_operator<int>));       // x/=y
    TEST((has_remainder_assign_operator<int>));    // x%=y
    TEST((has_bitwise_and_assign_operator<int>));  // x&=y
    TEST((has_bitwise_xor_assign_operator<int>));  // x^=y
    TEST((has_bitwise_or_assign_operator<int>));   // x|=y
    TEST((has_right_shift_assign_operator<int>));  // x>>=y
    TEST((has_left_shift_assign_operator<int>));   // x<<=y
    TEST((! has_index_operator<int, int>));        // x[y]

    TEST((! has_pre_increment_operator<std::string>));       // ++x
    TEST((! has_pre_decrement_operator<std::string>));       // --x
    TEST((! has_post_increment_operator<std::string>));      // x++
    TEST((! has_post_decrement_operator<std::string>));      // x--
    TEST((! has_unary_plus_operator<std::string>));          // +x
    TEST((! has_unary_minus_operator<std::string>));         // -x
    TEST((! has_dereference_operator<std::string>));         // *x
    TEST((! has_logical_not_operator<std::string>));         // !x
    TEST((! has_bitwise_not_operator<std::string>));         // ~x
    TEST((has_plus_operator<std::string>));                  // x+y
    TEST((! has_minus_operator<std::string>));               // x-y
    TEST((! has_multiply_operator<std::string>));            // x*y
    TEST((! has_divide_operator<std::string>));              // x/y
    TEST((! has_remainder_operator<std::string>));           // x%y
    TEST((! has_bitwise_and_operator<std::string>));         // x&y
    TEST((! has_bitwise_xor_operator<std::string>));         // x^y
    TEST((! has_bitwise_or_operator<std::string>));          // x|y
    TEST((! has_left_shift_operator<std::string>));          // x<<y
    TEST((! has_right_shift_operator<std::string>));         // x>>y
    TEST((has_equal_operator<std::string>));                 // x==y
    TEST((has_not_equal_operator<std::string>));             // x!=y
    TEST((has_less_than_operator<std::string>));             // x<y
    TEST((has_greater_than_operator<std::string>));          // x>y
    TEST((has_less_or_equal_operator<std::string>));         // x<=y
    TEST((has_greater_or_equal_operator<std::string>));      // x>=y
    TEST((! has_logical_and_operator<std::string>));         // x&&y
    TEST((! has_logical_or_operator<std::string>));          // x||y
    TEST((has_assign_operator<std::string>));                // x=y
    TEST((has_plus_assign_operator<std::string>));           // x+=y
    TEST((! has_minus_assign_operator<std::string>));        // x-=y
    TEST((! has_multiply_assign_operator<std::string>));     // x*=y
    TEST((! has_divide_assign_operator<std::string>));       // x/=y
    TEST((! has_remainder_assign_operator<std::string>));    // x%=y
    TEST((! has_bitwise_and_assign_operator<std::string>));  // x&=y
    TEST((! has_bitwise_xor_assign_operator<std::string>));  // x^=y
    TEST((! has_bitwise_or_assign_operator<std::string>));   // x|=y
    TEST((! has_right_shift_assign_operator<std::string>));  // x>>=y
    TEST((! has_left_shift_assign_operator<std::string>));   // x<<=y
    TEST((has_index_operator<std::string, int>));            // x[y]

    TEST((has_pre_increment_operator<char*>));     // ++x
    TEST((has_pre_decrement_operator<char*>));     // --x
    TEST((has_post_increment_operator<char*>));    // x++
    TEST((has_post_decrement_operator<char*>));    // x--
    TEST((has_unary_plus_operator<char*>));        // +x
    TEST((! has_unary_minus_operator<char*>));     // -x
    TEST((has_dereference_operator<char*>));       // *x
    TEST((has_logical_not_operator<char*>));       // !x
    TEST((! has_bitwise_not_operator<char*>));     // ~x
    TEST((! has_plus_operator<char*>));            // x+y
    TEST((has_minus_operator<char*>));             // x-y
    TEST((! has_multiply_operator<char*>));        // x*y
    TEST((! has_divide_operator<char*>));          // x/y
    TEST((! has_remainder_operator<char*>));       // x%y
    TEST((! has_bitwise_and_operator<char*>));     // x&y
    TEST((! has_bitwise_xor_operator<char*>));     // x^y
    TEST((! has_bitwise_or_operator<char*>));      // x|y
    TEST((! has_left_shift_operator<char*>));      // x<<y
    TEST((! has_right_shift_operator<char*>));     // x>>y
    TEST((has_equal_operator<char*>));             // x==y
    TEST((has_not_equal_operator<char*>));         // x!=y
    TEST((has_less_than_operator<char*>));         // x<y
    TEST((has_greater_than_operator<char*>));      // x>y
    TEST((has_less_or_equal_operator<char*>));     // x<=y
    TEST((has_greater_or_equal_operator<char*>));  // x>=y
    TEST((has_logical_and_operator<char*>));       // x&&y
    TEST((has_logical_or_operator<char*>));        // x||y
    TEST((has_assign_operator<char*>));            // x=y
    TEST((has_index_operator<char*, int>));        // x[y]

    TEST((has_plus_assign_operator<char*, int>));           // x+=y
    TEST((has_minus_assign_operator<char*, int>));          // x-=y
    TEST((! has_multiply_assign_operator<char*, int>));     // x*=y
    TEST((! has_divide_assign_operator<char*, int>));       // x/=y
    TEST((! has_remainder_assign_operator<char*, int>));    // x%=y
    TEST((! has_bitwise_and_assign_operator<char*, int>));  // x&=y
    TEST((! has_bitwise_xor_assign_operator<char*, int>));  // x^=y
    TEST((! has_bitwise_or_assign_operator<char*, int>));   // x|=y
    TEST((! has_right_shift_assign_operator<char*, int>));  // x>>=y
    TEST((! has_left_shift_assign_operator<char*, int>));   // x<<=y

    TEST((has_assign_operator<double, int>));                 // x=y
    TEST((has_assign_operator<std::string, const char*>));    // x=y
    TEST((! has_assign_operator<int, std::string>));          // x=y
    TEST((! has_assign_operator<const char*, std::string>));  // x=y

    TEST((has_index_operator<std::vector<std::string>, int>));            // x[y]
    TEST((has_index_operator<std::map<int, std::string>, int>));          // x[y]
    TEST((has_index_operator<std::map<std::string, int>, std::string>));  // x[y]

    TEST((has_increment_operators<int>));           // ++x x++
    TEST((has_step_operators<int>));                // ++x x++ --x x--
    TEST((has_additive_operators<int>));            // x+y x+=y x-y x-=y
    TEST((has_arithmetic_operators<int>));          // -x x+y x+=y x-y x-=y x*y x*=y x/y x/=y
    TEST((has_integer_arithmetic_operators<int>));  // -x ++x x++ --x x-- x+y x+=y x-y x-=y x*y x*=y x/y x/=y x%y x%=y
    TEST((has_bitwise_operators<int>));             // ~x x&y x&=y x|y x|=y x^y x^=y x<<y x<<=y x>>y x>>=y
    TEST((has_equality_operators<int>));            // x==y x!=y
    TEST((has_comparison_operators<int>));          // x==y x!=y x<y x<=y x>y x>=y

    TEST((! has_increment_operators<std::string>));           // ++x x++
    TEST((! has_step_operators<std::string>));                // ++x x++ --x x--
    TEST((! has_additive_operators<std::string>));            // x+y x+=y x-y x-=y
    TEST((! has_arithmetic_operators<std::string>));          // -x x+y x+=y x-y x-=y x*y x*=y x/y x/=y
    TEST((! has_integer_arithmetic_operators<std::string>));  // -x ++x x++ --x x-- x+y x+=y x-y x-=y x*y x*=y x/y x/=y x%y x%=y
    TEST((! has_bitwise_operators<std::string>));             // ~x x&y x&=y x|y x|=y x^y x^=y x<<y x<<=y x>>y x>>=y
    TEST((has_equality_operators<std::string>));              // x==y x!=y
    TEST((has_comparison_operators<std::string>));            // x==y x!=y x<y x<=y x>y x>=y

    TEST((! has_pre_increment_operator<void*>));            // ++x
    TEST((! has_pre_decrement_operator<void*>));            // --x
    TEST((! has_post_increment_operator<void*>));           // x++
    TEST((! has_post_decrement_operator<void*>));           // x--
    TEST((has_unary_plus_operator<void*>));                 // +x
    TEST((! has_unary_minus_operator<void*>));              // -x
    TEST((! has_dereference_operator<void*>));              // *x
    TEST((has_logical_not_operator<void*>));                // !x
    TEST((! has_bitwise_not_operator<void*>));              // ~x
    TEST((! has_plus_operator<void*>));                     // x+y
    TEST((! has_minus_operator<void*>));                    // x-y
    TEST((! has_multiply_operator<void*>));                 // x*y
    TEST((! has_divide_operator<void*>));                   // x/y
    TEST((! has_remainder_operator<void*>));                // x%y
    TEST((! has_bitwise_and_operator<void*>));              // x&y
    TEST((! has_bitwise_or_operator<void*>));               // x|y
    TEST((! has_bitwise_xor_operator<void*>));              // x^y
    TEST((! has_left_shift_operator<void*, int>));          // x<<y
    TEST((! has_right_shift_operator<void*, int>));         // x>>y
    TEST((has_equal_operator<void*>));                      // x==y
    TEST((has_not_equal_operator<void*>));                  // x!=y
    TEST((has_less_than_operator<void*>));                  // x<y
    TEST((has_greater_than_operator<void*>));               // x>y
    TEST((has_less_or_equal_operator<void*>));              // x<=y
    TEST((has_greater_or_equal_operator<void*>));           // x>=y
    TEST((has_logical_and_operator<void*>));                // x&&y
    TEST((has_logical_or_operator<void*>));                 // x||y
    TEST((has_assign_operator<void*>));                     // x=y
    TEST((! has_plus_assign_operator<void*>));              // x+=y
    TEST((! has_minus_assign_operator<void*>));             // x-=y
    TEST((! has_multiply_assign_operator<void*>));          // x*=y
    TEST((! has_divide_assign_operator<void*>));            // x/=y
    TEST((! has_remainder_assign_operator<void*>));         // x%=y
    TEST((! has_bitwise_and_assign_operator<void*>));       // x&=y
    TEST((! has_bitwise_or_assign_operator<void*>));        // x|=y
    TEST((! has_bitwise_xor_assign_operator<void*>));       // x^=y
    TEST((! has_left_shift_assign_operator<void*, int>));   // x<<=y
    TEST((! has_right_shift_assign_operator<void*, int>));  // x>>=y
    TEST((! has_index_operator<void*, size_t>));            // x>>=y

    TEST((! has_pre_increment_operator<const void*>));            // ++x
    TEST((! has_pre_decrement_operator<const void*>));            // --x
    TEST((! has_post_increment_operator<const void*>));           // x++
    TEST((! has_post_decrement_operator<const void*>));           // x--
    TEST((has_unary_plus_operator<const void*>));                 // +x
    TEST((! has_unary_minus_operator<const void*>));              // -x
    TEST((! has_dereference_operator<const void*>));              // *x
    TEST((has_logical_not_operator<const void*>));                // !x
    TEST((! has_bitwise_not_operator<const void*>));              // ~x
    TEST((! has_plus_operator<const void*>));                     // x+y
    TEST((! has_minus_operator<const void*>));                    // x-y
    TEST((! has_multiply_operator<const void*>));                 // x*y
    TEST((! has_divide_operator<const void*>));                   // x/y
    TEST((! has_remainder_operator<const void*>));                // x%y
    TEST((! has_bitwise_and_operator<const void*>));              // x&y
    TEST((! has_bitwise_or_operator<const void*>));               // x|y
    TEST((! has_bitwise_xor_operator<const void*>));              // x^y
    TEST((! has_left_shift_operator<const void*, int>));          // x<<y
    TEST((! has_right_shift_operator<const void*, int>));         // x>>y
    TEST((has_equal_operator<const void*>));                      // x==y
    TEST((has_not_equal_operator<const void*>));                  // x!=y
    TEST((has_less_than_operator<const void*>));                  // x<y
    TEST((has_greater_than_operator<const void*>));               // x>y
    TEST((has_less_or_equal_operator<const void*>));              // x<=y
    TEST((has_greater_or_equal_operator<const void*>));           // x>=y
    TEST((has_logical_and_operator<const void*>));                // x&&y
    TEST((has_logical_or_operator<const void*>));                 // x||y
    TEST((has_assign_operator<const void*>));                     // x=y
    TEST((! has_plus_assign_operator<const void*>));              // x+=y
    TEST((! has_minus_assign_operator<const void*>));             // x-=y
    TEST((! has_multiply_assign_operator<const void*>));          // x*=y
    TEST((! has_divide_assign_operator<const void*>));            // x/=y
    TEST((! has_remainder_assign_operator<const void*>));         // x%=y
    TEST((! has_bitwise_and_assign_operator<const void*>));       // x&=y
    TEST((! has_bitwise_or_assign_operator<const void*>));        // x|=y
    TEST((! has_bitwise_xor_assign_operator<const void*>));       // x^=y
    TEST((! has_left_shift_assign_operator<const void*, int>));   // x<<=y
    TEST((! has_right_shift_assign_operator<const void*, int>));  // x>>=y
    TEST((! has_index_operator<const void*, size_t>));            // x>>=y

}

void test_core_meta_function_detection() {

    TEST((has_function_foo<>));
    TEST((has_function_foo<int>));
    TEST((has_function_foo<int, int>));
    TEST((! has_function_foo<int, int, int>));
    TEST((! has_function_foo<std::string>));

    TEST((! has_function_std_begin<>));
    TEST((! has_function_std_begin<int>));
    TEST((has_function_std_begin<std::string>));
    TEST((has_function_std_begin<std::vector<int>>));
    TEST((! has_function_std_begin<std::string, std::string>));

    TEST((! has_method_bar<int>));
    TEST((! has_method_bar<int, int>));
    TEST((! has_method_bar<int, int, int>));
    TEST((! has_method_bar<int, std::string>));
    TEST((! has_method_bar<NoBar>));
    TEST((! has_method_bar<NoBar, int>));
    TEST((! has_method_bar<NoBar, int, int>));
    TEST((! has_method_bar<NoBar, std::string>));
    TEST((has_method_bar<Bar0>));
    TEST((! has_method_bar<Bar0, int>));
    TEST((! has_method_bar<Bar0, int, int>));
    TEST((! has_method_bar<Bar0, std::string>));
    TEST((! has_method_bar<Bar1>));
    TEST((has_method_bar<Bar1, int>));
    TEST((! has_method_bar<Bar1, int, int>));
    TEST((! has_method_bar<Bar1, std::string>));
    TEST((! has_method_bar<Bar2>));
    TEST((! has_method_bar<Bar2, int>));
    TEST((has_method_bar<Bar2, int, int>));
    TEST((! has_method_bar<Bar2, std::string>));

    TEST(! has_member_foo<int>);
    TEST(! has_member_foo<std::string>);
    TEST(has_member_foo<Bar0>);
    TEST(has_member_foo<Bar1>);
    TEST(has_member_foo<Bar2>);

    TEST((has_member_type_value_type<std::string>));
    TEST((has_member_type_value_type<std::vector<int>>));
    TEST((! has_member_type_value_type<int>));
    TEST((! has_member_type_value_type<std::shared_ptr<std::string>>));

}

void test_core_meta_type_categories() {

    TEST(is_hashable<int>);
    TEST(is_hashable<std::string>);
    TEST(! is_hashable<std::vector<int>>);

    TEST(! is_iterator<int>);
    TEST(is_iterator<int*>);
    TEST(is_iterator<const int*>);
    TEST(! is_iterator<std::string>);
    TEST(is_iterator<std::string::iterator>);
    TEST(is_iterator<std::string::const_iterator>);

    TEST(! is_forward_iterator<int>);
    TEST(is_forward_iterator<int*>);
    TEST(is_forward_iterator<const int*>);
    TEST(! is_forward_iterator<std::string>);
    TEST(is_forward_iterator<std::string::iterator>);
    TEST(is_forward_iterator<std::string::const_iterator>);
    TEST(is_forward_iterator<std::list<int>::iterator>);
    TEST(is_forward_iterator<std::list<int>::const_iterator>);
    TEST(is_forward_iterator<std::forward_list<int>::iterator>);
    TEST(is_forward_iterator<std::forward_list<int>::const_iterator>);

    TEST(! is_bidirectional_iterator<int>);
    TEST(is_bidirectional_iterator<int*>);
    TEST(is_bidirectional_iterator<const int*>);
    TEST(! is_bidirectional_iterator<std::string>);
    TEST(is_bidirectional_iterator<std::string::iterator>);
    TEST(is_bidirectional_iterator<std::string::const_iterator>);
    TEST(is_bidirectional_iterator<std::list<int>::iterator>);
    TEST(is_bidirectional_iterator<std::list<int>::const_iterator>);
    TEST(! is_bidirectional_iterator<std::forward_list<int>::iterator>);
    TEST(! is_bidirectional_iterator<std::forward_list<int>::const_iterator>);

    TEST(! is_random_access_iterator<int>);
    TEST(is_random_access_iterator<int*>);
    TEST(is_random_access_iterator<const int*>);
    TEST(! is_random_access_iterator<std::string>);
    TEST(is_random_access_iterator<std::string::iterator>);
    TEST(is_random_access_iterator<std::string::const_iterator>);
    TEST(! is_random_access_iterator<std::list<int>::iterator>);
    TEST(! is_random_access_iterator<std::list<int>::const_iterator>);
    TEST(! is_random_access_iterator<std::forward_list<int>::iterator>);
    TEST(! is_random_access_iterator<std::forward_list<int>::const_iterator>);

    TEST(! is_mutable_iterator<int>);
    TEST(is_mutable_iterator<int*>);
    TEST(! is_mutable_iterator<const int*>);
    TEST(! is_mutable_iterator<std::string>);
    TEST(is_mutable_iterator<std::string::iterator>);
    TEST(! is_mutable_iterator<std::string::const_iterator>);

    TEST(! is_range<int>);
    TEST(! is_range<int*>);
    TEST(is_range<std::string>);
    TEST(is_range<std::vector<int>>);

    TEST(! is_mutable_range<int>);
    TEST(! is_mutable_range<int*>);
    TEST(is_mutable_range<std::string>);
    TEST(is_mutable_range<std::vector<int>>);
    TEST(! is_mutable_range<const std::string>);
    TEST(! is_mutable_range<const std::vector<int>>);

    TEST(! is_container<int>);
    TEST(! is_container<int*>);
    TEST(is_container<std::string>);
    TEST(is_container<std::vector<int>>);

    TEST(! is_insertable_container<int>);
    TEST(! is_insertable_container<int*>);
    TEST(is_insertable_container<std::string>);
    TEST(is_insertable_container<std::vector<int>>);

    TEST(! is_pair<int>);
    TEST(! is_pair<std::string>);
    TEST((is_pair<std::pair<int, int>>));
    TEST((is_pair<std::pair<std::string, std::string>>));
    TEST((is_pair<Irange<int*>>));
    TEST((is_pair<Irange<std::string::iterator>>));
    TEST((is_pair<Irange<std::string::const_iterator>>));

    TEST(is_swappable<int>);
    TEST(is_swappable<char*>);
    TEST(is_swappable<std::string>);
    TEST(is_swappable<std::vector<int>>);

}

void test_core_meta_associated_types() {

    TEST_TYPE(IteratorValue<int>, void);
    TEST_TYPE(IteratorValue<int*>, int);
    TEST_TYPE(IteratorValue<const int*>, int);
    TEST_TYPE(IteratorValue<std::string>, void);
    TEST_TYPE(IteratorValue<std::string::iterator>, char);
    TEST_TYPE(IteratorValue<std::string::const_iterator>, char);

    TEST_TYPE(RangeIterator<std::string>, std::string::iterator);
    TEST_TYPE(RangeIterator<const std::string>, std::string::const_iterator);
    TEST_TYPE(RangeIterator<std::vector<int>>, std::vector<int>::iterator);
    TEST_TYPE(RangeIterator<const std::vector<int>>, std::vector<int>::const_iterator);
    TEST_TYPE(RangeIterator<Irange<int*>>, int*);
    TEST_TYPE(RangeIterator<Irange<const int*>>, const int*);

    TEST_TYPE(RangeValue<std::string>, char);
    TEST_TYPE(RangeValue<std::vector<int>>, int);
    TEST_TYPE(RangeValue<Irange<int*>>, int);
    TEST_TYPE(RangeValue<Irange<const int*>>, int);

}

void test_core_meta_iterator_utilities() {

    using istream_iter  = std::istream_iterator<char>;             // input iterator
    using flist_iter    = std::forward_list<int>::iterator;        // forward iterator
    using flist_citer   = std::forward_list<int>::const_iterator;  // forward iterator
    using list_iter     = std::list<int>::iterator;                // bidirectional iterator
    using list_citer    = std::list<int>::const_iterator;          // bidirectional iterator
    using string_iter   = std::string::iterator;                   // random access iterator
    using string_citer  = std::string::const_iterator;             // random access iterator
    using char_ptr      = char*;                                   // random access iterator
    using char_cptr     = const char*;                             // random access iterator

    TEST((category_is_equal<std::input_iterator_tag, std::input_iterator_tag>));                    TEST((! category_is_less<std::input_iterator_tag, std::input_iterator_tag>));
    TEST((! category_is_equal<std::input_iterator_tag, std::forward_iterator_tag>));                TEST((category_is_less<std::input_iterator_tag, std::forward_iterator_tag>));
    TEST((! category_is_equal<std::input_iterator_tag, std::bidirectional_iterator_tag>));          TEST((category_is_less<std::input_iterator_tag, std::bidirectional_iterator_tag>));
    TEST((! category_is_equal<std::input_iterator_tag, std::random_access_iterator_tag>));          TEST((category_is_less<std::input_iterator_tag, std::random_access_iterator_tag>));
    TEST((! category_is_equal<std::forward_iterator_tag, std::input_iterator_tag>));                TEST((! category_is_less<std::forward_iterator_tag, std::input_iterator_tag>));
    TEST((category_is_equal<std::forward_iterator_tag, std::forward_iterator_tag>));                TEST((! category_is_less<std::forward_iterator_tag, std::forward_iterator_tag>));
    TEST((! category_is_equal<std::forward_iterator_tag, std::bidirectional_iterator_tag>));        TEST((category_is_less<std::forward_iterator_tag, std::bidirectional_iterator_tag>));
    TEST((! category_is_equal<std::forward_iterator_tag, std::random_access_iterator_tag>));        TEST((category_is_less<std::forward_iterator_tag, std::random_access_iterator_tag>));
    TEST((! category_is_equal<std::bidirectional_iterator_tag, std::input_iterator_tag>));          TEST((! category_is_less<std::bidirectional_iterator_tag, std::input_iterator_tag>));
    TEST((! category_is_equal<std::bidirectional_iterator_tag, std::forward_iterator_tag>));        TEST((! category_is_less<std::bidirectional_iterator_tag, std::forward_iterator_tag>));
    TEST((category_is_equal<std::bidirectional_iterator_tag, std::bidirectional_iterator_tag>));    TEST((! category_is_less<std::bidirectional_iterator_tag, std::bidirectional_iterator_tag>));
    TEST((! category_is_equal<std::bidirectional_iterator_tag, std::random_access_iterator_tag>));  TEST((category_is_less<std::bidirectional_iterator_tag, std::random_access_iterator_tag>));
    TEST((! category_is_equal<std::random_access_iterator_tag, std::input_iterator_tag>));          TEST((! category_is_less<std::random_access_iterator_tag, std::input_iterator_tag>));
    TEST((! category_is_equal<std::random_access_iterator_tag, std::forward_iterator_tag>));        TEST((! category_is_less<std::random_access_iterator_tag, std::forward_iterator_tag>));
    TEST((! category_is_equal<std::random_access_iterator_tag, std::bidirectional_iterator_tag>));  TEST((! category_is_less<std::random_access_iterator_tag, std::bidirectional_iterator_tag>));
    TEST((category_is_equal<std::random_access_iterator_tag, std::random_access_iterator_tag>));    TEST((! category_is_less<std::random_access_iterator_tag, std::random_access_iterator_tag>));

    TEST((category_is_equal<istream_iter, istream_iter>));   TEST((! category_is_less<istream_iter, istream_iter>));
    TEST((! category_is_equal<istream_iter, flist_iter>));   TEST((category_is_less<istream_iter, flist_iter>));
    TEST((! category_is_equal<istream_iter, list_iter>));    TEST((category_is_less<istream_iter, list_iter>));
    TEST((! category_is_equal<istream_iter, string_iter>));  TEST((category_is_less<istream_iter, string_iter>));
    TEST((! category_is_equal<flist_iter, istream_iter>));   TEST((! category_is_less<flist_iter, istream_iter>));
    TEST((category_is_equal<flist_iter, flist_iter>));       TEST((! category_is_less<flist_iter, flist_iter>));
    TEST((! category_is_equal<flist_iter, list_iter>));      TEST((category_is_less<flist_iter, list_iter>));
    TEST((! category_is_equal<flist_iter, string_iter>));    TEST((category_is_less<flist_iter, string_iter>));
    TEST((! category_is_equal<list_iter, istream_iter>));    TEST((! category_is_less<list_iter, istream_iter>));
    TEST((! category_is_equal<list_iter, flist_iter>));      TEST((! category_is_less<list_iter, flist_iter>));
    TEST((category_is_equal<list_iter, list_iter>));         TEST((! category_is_less<list_iter, list_iter>));
    TEST((! category_is_equal<list_iter, string_iter>));     TEST((category_is_less<list_iter, string_iter>));
    TEST((! category_is_equal<string_iter, istream_iter>));  TEST((! category_is_less<string_iter, istream_iter>));
    TEST((! category_is_equal<string_iter, flist_iter>));    TEST((! category_is_less<string_iter, flist_iter>));
    TEST((! category_is_equal<string_iter, list_iter>));     TEST((! category_is_less<string_iter, list_iter>));
    TEST((category_is_equal<string_iter, string_iter>));     TEST((! category_is_less<string_iter, string_iter>));

    TEST_TYPE(GetIteratorCategory<std::input_iterator_tag>, std::input_iterator_tag);
    TEST_TYPE(GetIteratorCategory<std::output_iterator_tag>, std::output_iterator_tag);
    TEST_TYPE(GetIteratorCategory<std::forward_iterator_tag>, std::forward_iterator_tag);
    TEST_TYPE(GetIteratorCategory<std::bidirectional_iterator_tag>, std::bidirectional_iterator_tag);
    TEST_TYPE(GetIteratorCategory<std::random_access_iterator_tag>, std::random_access_iterator_tag);
    TEST_TYPE(GetIteratorCategory<istream_iter>, std::input_iterator_tag);
    TEST_TYPE(GetIteratorCategory<flist_iter>, std::forward_iterator_tag);
    TEST_TYPE(GetIteratorCategory<flist_citer>, std::forward_iterator_tag);
    TEST_TYPE(GetIteratorCategory<list_iter>, std::bidirectional_iterator_tag);
    TEST_TYPE(GetIteratorCategory<list_citer>, std::bidirectional_iterator_tag);
    TEST_TYPE(GetIteratorCategory<string_iter>, std::random_access_iterator_tag);
    TEST_TYPE(GetIteratorCategory<string_citer>, std::random_access_iterator_tag);
    TEST_TYPE(GetIteratorCategory<char_ptr>, std::random_access_iterator_tag);
    TEST_TYPE(GetIteratorCategory<char_cptr>, std::random_access_iterator_tag);
    TEST_TYPE(GetIteratorCategory<std::forward_list<int>>, std::forward_iterator_tag);
    TEST_TYPE(GetIteratorCategory<std::list<int>>, std::bidirectional_iterator_tag);
    TEST_TYPE(GetIteratorCategory<std::string>, std::random_access_iterator_tag);

    TEST(is_iterator_category<std::input_iterator_tag>);
    TEST(is_iterator_category<std::output_iterator_tag>);
    TEST(is_iterator_category<std::forward_iterator_tag>);
    TEST(is_iterator_category<std::bidirectional_iterator_tag>);
    TEST(is_iterator_category<std::random_access_iterator_tag>);
    TEST(! is_iterator_category<istream_iter>);
    TEST(! is_iterator_category<flist_iter>);
    TEST(! is_iterator_category<flist_citer>);
    TEST(! is_iterator_category<list_iter>);
    TEST(! is_iterator_category<list_citer>);
    TEST(! is_iterator_category<string_iter>);
    TEST(! is_iterator_category<string_citer>);
    TEST(! is_iterator_category<char_ptr>);
    TEST(! is_iterator_category<char_cptr>);

    { using T = MinCategory<std::input_iterator_tag, std::forward_iterator_tag, std::bidirectional_iterator_tag, std::random_access_iterator_tag>;  TEST_TYPE(T, std::input_iterator_tag); }
    { using T = MinCategory<std::forward_iterator_tag, std::bidirectional_iterator_tag, std::random_access_iterator_tag>;                           TEST_TYPE(T, std::forward_iterator_tag); }
    { using T = MinCategory<std::bidirectional_iterator_tag, std::random_access_iterator_tag>;                                                      TEST_TYPE(T, std::bidirectional_iterator_tag); }
    { using T = MinCategory<std::random_access_iterator_tag>;                                                                                       TEST_TYPE(T, std::random_access_iterator_tag); }

    { using T = MinCategory<istream_iter>;  TEST_TYPE(T, std::input_iterator_tag); }
    { using T = MinCategory<flist_iter>;    TEST_TYPE(T, std::forward_iterator_tag); }
    { using T = MinCategory<flist_citer>;   TEST_TYPE(T, std::forward_iterator_tag); }
    { using T = MinCategory<list_iter>;     TEST_TYPE(T, std::bidirectional_iterator_tag); }
    { using T = MinCategory<list_citer>;    TEST_TYPE(T, std::bidirectional_iterator_tag); }
    { using T = MinCategory<string_iter>;   TEST_TYPE(T, std::random_access_iterator_tag); }
    { using T = MinCategory<string_citer>;  TEST_TYPE(T, std::random_access_iterator_tag); }
    { using T = MinCategory<char_ptr>;      TEST_TYPE(T, std::random_access_iterator_tag); }
    { using T = MinCategory<char_cptr>;     TEST_TYPE(T, std::random_access_iterator_tag); }

    { using T = MinCategory<istream_iter, flist_iter>;   TEST_TYPE(T, std::input_iterator_tag); }
    { using T = MinCategory<flist_iter, flist_citer>;    TEST_TYPE(T, std::forward_iterator_tag); }
    { using T = MinCategory<flist_citer, list_iter>;     TEST_TYPE(T, std::forward_iterator_tag); }
    { using T = MinCategory<list_iter, list_citer>;      TEST_TYPE(T, std::bidirectional_iterator_tag); }
    { using T = MinCategory<list_citer, string_iter>;    TEST_TYPE(T, std::bidirectional_iterator_tag); }
    { using T = MinCategory<string_iter, string_citer>;  TEST_TYPE(T, std::random_access_iterator_tag); }
    { using T = MinCategory<string_citer, char_ptr>;     TEST_TYPE(T, std::random_access_iterator_tag); }
    { using T = MinCategory<char_ptr, char_cptr>;        TEST_TYPE(T, std::random_access_iterator_tag); }

    { using T = MinCategory<istream_iter, flist_iter, flist_citer>;  TEST_TYPE(T, std::input_iterator_tag); }
    { using T = MinCategory<flist_iter, flist_citer, list_iter>;     TEST_TYPE(T, std::forward_iterator_tag); }
    { using T = MinCategory<flist_citer, list_iter, list_citer>;     TEST_TYPE(T, std::forward_iterator_tag); }
    { using T = MinCategory<list_iter, list_citer, string_iter>;     TEST_TYPE(T, std::bidirectional_iterator_tag); }
    { using T = MinCategory<list_citer, string_iter, string_citer>;  TEST_TYPE(T, std::bidirectional_iterator_tag); }
    { using T = MinCategory<string_iter, string_citer, char_ptr>;    TEST_TYPE(T, std::random_access_iterator_tag); }
    { using T = MinCategory<string_citer, char_ptr, char_cptr>;      TEST_TYPE(T, std::random_access_iterator_tag); }

    { using T = MinCategory<istream_iter, flist_iter, flist_citer, list_iter>;  TEST_TYPE(T, std::input_iterator_tag); }
    { using T = MinCategory<flist_iter, flist_citer, list_iter, list_citer>;    TEST_TYPE(T, std::forward_iterator_tag); }
    { using T = MinCategory<flist_citer, list_iter, list_citer, string_iter>;   TEST_TYPE(T, std::forward_iterator_tag); }
    { using T = MinCategory<list_iter, list_citer, string_iter, string_citer>;  TEST_TYPE(T, std::bidirectional_iterator_tag); }
    { using T = MinCategory<list_citer, string_iter, string_citer, char_ptr>;   TEST_TYPE(T, std::bidirectional_iterator_tag); }
    { using T = MinCategory<string_iter, string_citer, char_ptr, char_cptr>;    TEST_TYPE(T, std::random_access_iterator_tag); }

    { using T = MinCategory<istream_iter, flist_iter, flist_citer, list_iter, list_citer>;   TEST_TYPE(T, std::input_iterator_tag); }
    { using T = MinCategory<flist_iter, flist_citer, list_iter, list_citer, string_iter>;    TEST_TYPE(T, std::forward_iterator_tag); }
    { using T = MinCategory<flist_citer, list_iter, list_citer, string_iter, string_citer>;  TEST_TYPE(T, std::forward_iterator_tag); }
    { using T = MinCategory<list_iter, list_citer, string_iter, string_citer, char_ptr>;     TEST_TYPE(T, std::bidirectional_iterator_tag); }
    { using T = MinCategory<list_citer, string_iter, string_citer, char_ptr, char_cptr>;     TEST_TYPE(T, std::bidirectional_iterator_tag); }

}
