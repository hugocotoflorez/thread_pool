# Thread Pool

## Syntax
```c
void thread_pool(void **array, size_t size, void *(*func)(void *) );
```

## Description

Create NUM_THREADS threads and change every element of
ARRAY with the element returned by FUNC, with arguments
the previous element at this position. The new element
have to be returned, and arguement should not be modified.

### Disclaimer

Array have to be a `void* arr[]` and func have to be
`void* (*func)(*void)` because of this. It would be
changed in the future (or not).


