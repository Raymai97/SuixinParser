#include "TimeTake.h"

#ifdef _WIN32

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

TTtime TimeTakenInMs(TTtime tBegin, TTtime tEnd)
{
	LARGE_INTEGER freq;
	QueryPerformanceFrequency(&freq);
	return ((tEnd - tBegin) * 1000) / freq.QuadPart;
}

TTtime TimeTakeNow(void)
{
	LARGE_INTEGER now;
	QueryPerformanceCounter(&now);
	return now.QuadPart;
}

#else // not _WIN32

#include <time.h>
#include <sys/timeb.h>

TTtime TimeTakenInMs(TTtime tBegin, TTtime tEnd)
{
	return tEnd - tBegin;
}

TTtime TimeTakeNow(void)
{
	struct timeb b;
	ftime(&b);
	return (TTtime)(b.millitm) + (b.time * 1000);
}

#endif // _WIN32
