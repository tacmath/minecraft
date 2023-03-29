#ifndef GLOBAL_DEFINES_H
#define GLOBAL_DEFINES_H

#define DEBUG_MODE 1

#if DEBUG_MODE == 1
#define ON_DEBUG(x) x
#else
#define ON_DEBUG(x)
#endif // DEBUG_MODE == 1

#endif