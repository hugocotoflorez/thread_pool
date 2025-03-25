
#define TP_TEST
#ifdef TP_TEST

#include <math.h>
#include <time.h>

#define TP_IMPLEMENTATION
#include "tp.h"

#define TYPE int
void
_sqrt(TYPE *n)
{
        usleep(2);
        *n = (int) sqrt((double) (intptr_t) *n);
}

struct timespec tp;

#define TIME_START()                                \
        do {                                        \
                clock_gettime(CLOCK_REALTIME, &tp); \
        } while (0)

#define TIME_REPORT(...)                                                                    \
        do {                                                                                \
                struct timespec tp2;                                                        \
                clock_gettime(CLOCK_REALTIME, &tp2);                                        \
                double time = tp2.tv_sec - tp.tv_sec + (tp2.tv_nsec - tp.tv_nsec) / 1e9;    \
                printf("[TIME]: " __VA_OPT__("%s: ") "%lf seconds\n", ##__VA_ARGS__, time); \
        } while (0)

int
main()
{
        int ARR_SIZE = 1024;
        TYPE arr[ARR_SIZE];
        for (int i = 0; i < ARR_SIZE; i++) {
                arr[i] = (TYPE) i;
        }

        TIME_START();
        thread_pool(arr, ARR_SIZE, sizeof(TYPE), _sqrt);
        TIME_REPORT();

        return 0;
}

#endif
