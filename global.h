#pragma once


#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/time.h>
#include <sys/resource.h>


#define if_likely(exp)      if (__builtin_expect(exp, true))
#define if_unlikely(exp)    if (__builtin_expect(exp, false))

#define sq(a)   ((a) * (a))

#define _PRE_STRINGFY(exp)	#exp
#define STRINGFY(exp)	_PRE_STRINGFY(exp)

#define LOG(format, exp)	printf("%s:" STRINGFY(__LINE__) ": " #exp " = " format "\n", __func__, (exp))

#define BENCH(str, exp)	{\
	puts(!(str[0]) ? "\nStart" : "\nStart: " str);\
	struct rusage resource_usage;\
	getrusage(RUSAGE_SELF, &resource_usage);\
	const struct timeval usertime_start = resource_usage.ru_utime;\
\
	{exp}\
\
	getrusage(RUSAGE_SELF, &resource_usage);\
	const struct timeval usertime_end = resource_usage.ru_utime;\
\
	printf("End: " str " (Ellapsed time: %.4fs)\n", (usertime_end.tv_sec - usertime_start.tv_sec) + (usertime_end.tv_usec - usertime_start.tv_usec)/1e6f);\
}

#ifdef NDEBUG
#define TEST(exp) (void) (0);
#else
#define TEST(exp) {\
	BENCH("", exp)\
}
#endif

__always_inline
uint64_t rand_fast() {
	extern uint64_t state;
	
	state ^= state << 13;
	state ^= state >> 17;
	state ^= state << 5;

	return state;
}

__always_inline
float randf() {
	return (float) rand_fast() / UINT64_MAX;
}