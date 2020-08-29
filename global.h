#pragma once


#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include <sys/resource.h>


#define likely(exp)     (__builtin_expect(exp, true))
#define unlikely(exp)	(__builtin_expect(exp, false))
#define with_probability(p, exp)	(__builtin_expect_with_probability(exp, true, p))

#define sq(a)   ((a) * (a))

#define _PRE_STRINGFY(exp)	#exp
#define STRINGFY(exp)	_PRE_STRINGFY(exp)

#define LOG(format, exp)	printf("%s:" STRINGFY(__LINE__) ": " #exp " = " format "\n", __func__, (exp))

#define BENCH(str, exp)	{\
	puts(!(str[0]) ? "\nStart" : "\nStart: " str);\
\
	struct timespec cpu_time_start;\
	struct timespec real_time_start;\
	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &cpu_time_start);\
	clock_gettime(CLOCK_REALTIME, &real_time_start);\
\
	{exp}\
\
	struct timespec cpu_time_end;\
	struct timespec real_time_end;\
	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &cpu_time_end);\
	clock_gettime(CLOCK_REALTIME, &real_time_end);\
\
	printf("End: " str "\t(Ellapsed time: CPU %.4fs\tReal %.4fs)\n",\
		(cpu_time_end.tv_sec - cpu_time_start.tv_sec)\
		+ (cpu_time_end.tv_nsec - cpu_time_start.tv_nsec)/1e9,\
		(real_time_end.tv_sec - real_time_start.tv_sec)\
		+ (real_time_end.tv_nsec - real_time_start.tv_nsec)/1e9);\
}

#ifdef NTEST
#define TEST(exp) (void) (0);
#else
#define NDEBUG
#define TEST(exp) {exp}
#endif
