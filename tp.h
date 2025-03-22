#ifndef TP_H
#define TP_H

#define NUM_THREADS 12
#define CHUNK_SIZE 256
#include <sys/types.h> // ssize_t

/* Create NUM_THREADS threads and change every element of
 * ARRAY with the element returned by FUNC, with arguments
 * the previous element at this position. The new element
 * have to be returned, and arguement should not be modified. */
void thread_pool(void **array, ssize_t size, void *(*func)(void *) );

#endif // TP_H
