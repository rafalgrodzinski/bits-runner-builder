#include "win_support.h"

// windows.h has to be in separate file causes it messes with LLVM library
#ifdef WIN32
    #define WIN32_LEAN_AND_MEAN
    #include "windows.h"
#endif

void win_support_init() {
#ifdef WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif
}
