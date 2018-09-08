#ifndef TIMETAKE_H
#define TIMETAKE_H

#include <time.h>

#ifdef _WIN32
typedef __int64 TTtime;
#else
typedef time_t TTtime;
#endif

TTtime TimeTakenInMs(TTtime tBegin, TTtime tEnd);
TTtime TimeTakeNow(void);

#endif//TIMETAKE_H
