#include "rs-core/signal.hpp"
#include "rs-core/unit-test.hpp"
#include <chrono>
#include <csignal>
#include <vector>

using namespace RS;
using namespace std::chrono;
using namespace std::literals;

void test_core_signal_channel() {

    #ifdef _XOPEN_SOURCE

        PosixSignal chan1{SIGINT};
        PosixSignal chan2{SIGUSR1, SIGUSR2};
        std::vector<int> signals = {SIGCHLD, SIGHUP, SIGPIPE};
        PosixSignal chan3{signals};
        int s = 0;

        TEST(! chan1.wait_for(10ms));

        raise(SIGINT);
        TEST(chan1.wait_for(10ms));
        TEST(chan1.read(s));
        TEST_EQUAL(s, SIGINT);
        TEST(! chan1.wait_for(10ms));

        raise(SIGINT);
        TEST(chan1.wait_for(10ms));
        TEST(chan1.read(s));
        TEST_EQUAL(s, SIGINT);
        TEST(! chan1.wait_for(10ms));

        raise(SIGURG);
        TEST(! chan1.wait_for(1ms));
        TEST(! chan2.wait_for(1ms));
        TEST(! chan3.wait_for(1ms));

        raise(SIGUSR1);
        TEST(chan2.wait_for(10ms));
        TEST(chan2.read(s));
        TEST_EQUAL(s, SIGUSR1);
        TEST(! chan2.wait_for(10ms));

        raise(SIGUSR2);
        TEST(chan2.wait_for(10ms));
        TEST(chan2.read(s));
        TEST_EQUAL(s, SIGUSR2);
        TEST(! chan2.wait_for(10ms));

        raise(SIGHUP);
        TEST(! chan1.wait_for(1ms));
        TEST(! chan2.wait_for(1ms));
        TRY(chan2.close());
        TEST(chan2.wait_for(10ms));
        TEST(chan2.is_closed());
        TEST(chan3.wait_for(10ms));
        TEST(chan3.read(s));
        TEST_EQUAL(s, SIGHUP);

    #else

        PosixSignal chan1{SIGINT};
        PosixSignal chan2{SIGSEGV, SIGTERM};
        int s = 0;

        TEST(! chan1.wait_for(10ms));

        std::raise(SIGINT);
        TEST(chan1.wait_for(10ms));
        TEST(chan1.read(s));
        TEST_EQUAL(s, SIGINT);
        TEST(! chan1.wait_for(10ms));

        std::raise(SIGINT);
        TEST(chan1.wait_for(10ms));
        TEST(chan1.read(s));
        TEST_EQUAL(s, SIGINT);
        TEST(! chan1.wait_for(10ms));

        std::raise(SIGSEGV);
        TEST(chan2.wait_for(10ms));
        TEST(chan2.read(s));
        TEST_EQUAL(s, SIGSEGV);
        TEST(! chan2.wait_for(10ms));

        std::raise(SIGTERM);
        TEST(chan2.wait_for(10ms));
        TEST(chan2.read(s));
        TEST_EQUAL(s, SIGTERM);
        TEST(! chan2.wait_for(10ms));

        TEST(! chan1.wait_for(1ms));
        TEST(! chan2.wait_for(1ms));
        TRY(chan1.close());
        TEST(chan1.wait_for(10ms));
        TEST(chan1.is_closed());
        TRY(chan2.close());
        TEST(chan2.wait_for(10ms));
        TEST(chan2.is_closed());

    #endif

}

void test_core_signal_name() {

    #define SIGNAL_NAME(s) TEST_EQUAL(PosixSignal::name(s), #s)

    SIGNAL_NAME(SIGABRT);
    SIGNAL_NAME(SIGFPE);
    SIGNAL_NAME(SIGILL);
    SIGNAL_NAME(SIGINT);
    SIGNAL_NAME(SIGSEGV);
    SIGNAL_NAME(SIGTERM);

    #ifdef _XOPEN_SOURCE

        SIGNAL_NAME(SIGABRT);
        SIGNAL_NAME(SIGALRM);
        SIGNAL_NAME(SIGBUS);
        SIGNAL_NAME(SIGCHLD);
        SIGNAL_NAME(SIGCONT);
        SIGNAL_NAME(SIGFPE);
        SIGNAL_NAME(SIGHUP);
        SIGNAL_NAME(SIGILL);
        SIGNAL_NAME(SIGINT);
        SIGNAL_NAME(SIGKILL);
        SIGNAL_NAME(SIGPIPE);
        SIGNAL_NAME(SIGPROF);
        SIGNAL_NAME(SIGQUIT);
        SIGNAL_NAME(SIGSEGV);
        SIGNAL_NAME(SIGSTOP);
        SIGNAL_NAME(SIGSYS);
        SIGNAL_NAME(SIGTERM);
        SIGNAL_NAME(SIGTRAP);
        SIGNAL_NAME(SIGTSTP);
        SIGNAL_NAME(SIGTTIN);
        SIGNAL_NAME(SIGTTOU);
        SIGNAL_NAME(SIGURG);
        SIGNAL_NAME(SIGUSR1);
        SIGNAL_NAME(SIGUSR2);
        SIGNAL_NAME(SIGVTALRM);
        SIGNAL_NAME(SIGXCPU);
        SIGNAL_NAME(SIGXFSZ);

    #endif

}
