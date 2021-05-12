#ifndef TIME_H
#define TIME_H

#include <time.h>

#define NS_PER_SECOND (1000000000)
#define NS_PER_MS (1000000)

#if defined(TIME_UTC)
    #define _UNIXTIME_GET(tp) timespec_get(tp, TIME_UTC)
#elif defined(CLOCK_REALTIME)
    #define _UNIXTIME_GET(tp) clock_gettime(CLOCK_REALTIME, tp)
#else
    #error cannot find function to get unix time
#endif

#define NOW() ({\
    struct timespec ts;\
    _UNIXTIME_GET(&ts);\
    ((ts.tv_sec * NS_PER_SECOND) + ts.tv_nsec);})

#endif