#ifndef TP_H_
#define TP_H_

#define TP_DEBUG 0
#define NUM_THREADS 12
#define CHUNK_SIZE 1024 * 1024

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
        LOG("Before ask_for_offset\n");
        pthread_mutex_lock(mutex);
        ret = *next_index;
#if defined(CHUNK_SIZE) && CHUNK_SIZE > 1
        *next_index += (CHUNK_SIZE);
#else
        ++*next_index;
#endif
        pthread_mutex_unlock(mutex);
        LOG("After ask_for_offset\n");

        if (ret < arr_size) {
                LOG("ret: %zd\n", ret);
                return ret;
        } else {
                LOG("ret: %zd < %zd\n", ret, arr_size);
                return -1;
        }
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
                LOG("iter++\n");

                if (data.size < arr_ptr + (CHUNK_SIZE))
                        end = data.size - arr_ptr;
                LOG("Before for\n");
                for (ssize_t offset = 0; offset < end; offset++)
                        (data.func)((void *) ((char *) data.array + (arr_ptr + offset) * data.elemsize));
#else
                (data.func)((void *) (data.array + arr_ptr * data.elemsize));
#endif
                LOG("After for\n");
        }

        LOG("returning from tp_thread_routine\n");
        return NULL;
}

void
thread_pool(void *arr, ssize_t n, ssize_t esize, void (*f)(void *))
{
        ssize_t next_index = 0;
        pthread_t threads[NUM_THREADS];
        pthread_mutex_t next_ptr_mutex;
        TPData data;
        ssize_t i;
        pthread_mutex_init(&next_ptr_mutex, NULL);
        data = (TPData) {
                .array = (char *) arr,
                .func = f,
                .size = n,
                .elemsize = esize,
                .next_index = &next_index,
                .mutex = &next_ptr_mutex,
        };
        for (i = 0; i < NUM_THREADS - 1; i++) {
                LOG("[CREATE] create new thread %zu\n", i);
                assert(pthread_create(threads + i, NULL, tp_thread_routine, &data) == 0);
        }
        tp_thread_routine(&data);
        for (i = 0; i < NUM_THREADS - 1; i++) {
                assert(pthread_join(threads[i], NULL) == 0);
                LOG("[JOINED] thread %zu\n", i);
        }
        LOG("[JOINED] ALL\n");
        pthread_mutex_destroy(&next_ptr_mutex);
        LOG("pthread_mutex_destroy \n");
}

#endif // TP_IMPLEMENTATION
#endif // TP_H_
