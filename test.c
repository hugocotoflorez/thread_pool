#include <stdint.h>
#include <time.h>

#define TP_IMPLEMENTATION
#include "tp.h"

typedef char TYPE;

void
_sqrt(TYPE *n)
{
        *n = 1;
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
        TYPE *arr = NULL;
        ssize_t max_value = -1 ^ ((size_t) 0x1 << (sizeof(ssize_t) * 8 - 1));
        for (ssize_t ARR_SIZE = 1; ARR_SIZE < max_value; ARR_SIZE *= 2) {
                printf("Size: %zd / %zd\n", ARR_SIZE, max_value);
                arr = realloc(arr, ARR_SIZE * sizeof(TYPE));

                TIME_START();
                thread_pool(arr, ARR_SIZE, sizeof(TYPE), _sqrt, 12);
                TIME_REPORT();

                for (ssize_t i = 0; i < ARR_SIZE; i++) {
                        assert(arr[i] == 1);
                }
        }
        free(arr);

        return 0;
}
