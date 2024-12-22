#include <iostream>

#define DEBUG_LEVEL 0

#define LOG(level, stream) \
    if (DEBUG_LEVEL >= level) { \
        std::cout << stream; \
    }
