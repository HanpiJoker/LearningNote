#ifndef __CHAPTER_1_H_
#define __CHAPTER_1_H_

#include <sys/time.h>
#include <time.h>

#define DATA_NUMS            (20000000)
#define DATA_LIMIT           (DATA_NUMS)
#define SOURCE_FILE          "./sort_source.txt"
#define RESULT_FILE          "./sort_result.txt"
#define RESULT_FILE_BITMAP   "./sort_result_bitmap.txt"

static inline double __caculate_cost_time_ms(struct timeval __start, struct timeval __end)
{
	return  (double)((__end.tv_sec - __start.tv_sec) * 1000000 + (__end.tv_usec - __start.tv_usec)) / 1000.000;
}

#define TIME_CACULATE(func) \
{ \
	struct timeval __start, __end; \
	double __cost = 0.0;           \
	gettimeofday(&__start, NULL);  \
	func;                          \
	gettimeofday(&__end, NULL);    \
	printf("current function time cost:%lfms\n", __caculate_cost_time_ms(__start, __end)); \
}

#endif /* __CHAPTER_1_H_ */
