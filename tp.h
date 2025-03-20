#ifndef TP_H
#define TP_H

#define NUM_THREADS 1

#include <stddef.h>

/* Create NUM_THREADS threads and change every element of
 * ARRAY with the element returned by FUNC, with arguments
 * the previous element at this position. The new element
 * have to be returned, and arguement should not be modified. */
void thread_pool(void **array, size_t size, void *(*func)(void *) );

#endif // TP_H
