# Thread Pool

## Syntax
```c
void thread_pool(T array[arrlen], size_t arrlen, size_t elemsize, void (*func)(T *));
```

## Description

Apply func to all elements of array in parallel.

## More info

Array can be of any type T, with arrlen elements of size elemsize bytes.
Function acceps different types in each call. The function acceps a forth
argument numthreads, which is the number of threads to be used. Func takes
a pointer to an element of array, that should modify in place.


