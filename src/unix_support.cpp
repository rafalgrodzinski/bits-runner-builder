#include "unix_support.h"

#ifndef WIN32
    #include <csignal>
    #include <cstdlib>
    #include <execinfo.h>
    #include <unistd.h>

    void crashHandler(int signal) {
        void *frames[64];
        int framesCount = backtrace(frames, 64);

        cerr << endl << "☠️ Crashed with signal " << signal << ":" << endl;
        backtrace_symbols_fd(frames, framesCount, STDERR_FILENO);

        exit(1);
    }
#endif

void unix_support_init() {
#ifndef WIN32
    signal(SIGSEGV, crashHandler);
    signal(SIGABRT, crashHandler);
    signal(SIGILL, crashHandler);
    signal(SIGFPE, crashHandler);
#endif
}