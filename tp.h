#ifndef TP_H_
#define TP_H_

#define TP_DEBUG 1
#define NUM_THREADS 12
#define CHUNK_SIZE 1024
#define STACK_SIZE 1024 * 1024

/* Looks like the chunk size do not modify eficiency. */

#include <assert.h>
#include <pthread.h>
#include <setjmp.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <threads.h>
#include <time.h>
#include <unistd.h>


#ifdef TP_IMPLEMENTATION


#if defined(TP_DEBUG) && TP_DEBUG > 0
#define LOG(format, ...) printf(format, ##__VA_ARGS__)
#else
#define LOG(...)
#endif

typedef struct {
        char *array;
        void (*func)(void *);
        ssize_t size;
        ssize_t elemsize;
        ssize_t *next_index;
        pthread_mutex_t *mutex;
} TPData;

static inline ssize_t
ask_for_offset(ssize_t *next_index, pthread_mutex_t *mutex, ssize_t arr_size)
{
        ssize_t ret;
        pthread_mutex_lock(mutex);
        ret = *next_index;
#if defined(CHUNK_SIZE) && CHUNK_SIZE > 1
        *next_index += (CHUNK_SIZE);
#else
        ++*next_index;
#endif
        pthread_mutex_unlock(mutex);
        return ret < arr_size ? ret : -1;
}


static void *
tp_thread_routine(void *args)
{
        TPData data = *(TPData *) args;
        ssize_t arr_ptr;
#if defined(CHUNK_SIZE) && CHUNK_SIZE > 1
        ssize_t end = (CHUNK_SIZE);
#endif

        while ((arr_ptr = ask_for_offset(data.next_index, data.mutex, data.size)) >= 0) {
#if defined(CHUNK_SIZE) && CHUNK_SIZE > 1

                if (data.size < arr_ptr + (CHUNK_SIZE))
                        end = data.size - arr_ptr;
                for (ssize_t offset = 0; offset < end; offset++)
                        (data.func)((void *) (data.array + (arr_ptr + offset) * data.elemsize));
#else
                data.array[arr_ptr] = data.func(data.array[arr_ptr]);
#endif
        }

        return NULL;
}

#define thread_pool(arr, n, esize, f, ...)                                                             \
        do {                                                                                      \
                __auto_type __array__ = (arr);                                                    \
                __auto_type __function__ = (f);                                                   \
                ssize_t next_index = 0;                                                           \
                ssize_t num_threads = NUM_THREADS;                                             \
                __VA_OPT__(num_threads = __VA_ARGS__);                                            \
                pthread_t threads[num_threads];                                                   \
                pthread_mutex_t next_ptr_mutex;                                                   \
                TPData data;                                                                      \
                pthread_mutex_init(&next_ptr_mutex, NULL);                                        \
                data = (TPData) {                                                                 \
                        .array = (void *) (__array__),                                            \
                        .func = (void (*)(void *))(__function__),                                 \
                        .size = (n),                                                              \
                        .elemsize = (esize),                                                      \
                        .next_index = &next_index,                                                \
                        .mutex = &next_ptr_mutex,                                                 \
                };                                                                                \
                for (int i = 0; i < num_threads; i++) {                                           \
                        LOG("[CREATE] create new thread %d\n", i);                                \
                        assert(pthread_create(threads + i, NULL, tp_thread_routine, &data) == 0); \
                }                                                                                 \
                for (int i = 0; i < num_threads; i++) {                                           \
                        assert(pthread_join(threads[i], NULL) == 0);                              \
                        LOG("[JOINED] thread %d\n", i);                                           \
                }                                                                                 \
                pthread_mutex_destroy(&next_ptr_mutex);                                           \
        } while (0);

#endif // TP_IMPLEMENTATION
#endif // TP_H_
