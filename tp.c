#include <math.h>
#include <time.h>

#define TP_IMPLEMENTATION
#include "tp.h"

typedef struct {
        int a, b;
} TYPE;

void
_sqrt(TYPE *n)
{
        n->a = n->a + n->b;
        usleep(2);
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
                arr[i] = (TYPE) { i, i % 3 };
        }

        TIME_START();
        thread_pool(arr, ARR_SIZE, sizeof(TYPE), _sqrt, 4);
        TIME_REPORT();

        return 0;
}
