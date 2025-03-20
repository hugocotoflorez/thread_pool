#include <pthread.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

#include "tp.h"

#if defined(TP_DEBUG) && TP_DEBUG > 0
#define LOG(format, ...) printf(format, ##__VA_ARGS__)
#else
#define LOG(...)
#endif

typedef struct {
        void **array;
        size_t size;
        void *(*func)(void *);
        ptrdiff_t *next_ptr;
        pthread_mutex_t *mutex;
} TPData;


static inline ptrdiff_t
ask_for_offset(ptrdiff_t *next_ptr, pthread_mutex_t *mutex, size_t arr_size)
{
        ptrdiff_t ret;
        pthread_mutex_lock(mutex);
        ret = ++*next_ptr;
        pthread_mutex_unlock(mutex);
        return ret < arr_size ? ret : -1;
}

static void *
tp_thread_routine(void *args)
{
        TPData data = *(TPData *) args;
        ptrdiff_t arr_ptr;

        while ((arr_ptr = ask_for_offset(data.next_ptr, data.mutex, data.size)) >= 0) {
                data.array[arr_ptr] = data.func(data.array[arr_ptr]);
        }

        return NULL;
}

void
thread_pool(void **array, size_t size, void *(*func)(void *) )
{
        ptrdiff_t next_ptr = 0;
        pthread_mutex_t next_ptr_mutex;
        pthread_t threads[NUM_THREADS];

        TPData data = (TPData) {
                .array = array,
                .size = size,
                .func = func,
                .next_ptr = &next_ptr,
                .mutex = &next_ptr_mutex,
        };

        for (int i = 0; i < NUM_THREADS; i++) {
                LOG("[CREATE] thread %d\n", i);
                pthread_create(threads + i, NULL, tp_thread_routine, &data);
        }

        for (int i = 0; i < NUM_THREADS; i++) {
                LOG("[JOIN] thread %d\n", i);
                pthread_join(threads[i], NULL);
        }
}

// #define TP_TEST
#ifdef TP_TEST

#include <math.h>
#include <time.h>

void *
_sqrt(void *n)
{
        usleep(2);
        return NULL + (int) sqrt((double) (intptr_t) n);
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
        int ARR_SIZE  = 100000;
        void *arr[ARR_SIZE];
        for (int i = 0; i < ARR_SIZE; i++) {
                arr[i] = NULL + i;
        }

        TIME_START();
        thread_pool(arr, ARR_SIZE, _sqrt);
        TIME_REPORT();

        return 0;
}

#endif
