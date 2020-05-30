#include "tty.h"

#include <cstdio>

#if __has_include(<unistd.h>)
#include <unistd.h>
#elif _WIN32
#include <io.h>
#define isatty _isatty
#endif

bool input_is_tty() {
    static const bool value = isatty(fileno(stdin));
    return value;
}
