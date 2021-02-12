#ifndef SORT_H
#define SORT_H

#include <stdlib.h>

// adapted from https://stackoverflow.com/questions/4300896/how-portable-is-the-re-entrant-qsort-r-function-compared-to-qsort

struct _sort_data {
    void *arg;
    int (*cmp)(const void *a, const void *b, void *arg);
};

static int _sort_r_arg_swap(void *s, const void *aa, const void *bb) {
    struct _sort_data *ss = (struct _sort_data*) s;
    return (ss->cmp)(aa, bb, ss->arg);
}

static inline void sort_r(
    void *base,
    size_t nel,
    size_t width,
    int (*cmp)(const void *a, const void *b, void *arg),
    void *arg
) {
#if (defined _GNU_SOURCE || defined __GNU__ || defined __linux__)
    qsort_r(base, nel, width, cmp, arg);
#elif (defined __APPLE__ || defined __MACH__ || defined __DARWIN__ || \
        defined __FREEBSD__ || defined __BSD__ || \
        defined OpenBSD3_1 || defined OpenBSD3_9)
    struct _sort_data tmp = {arg, cmp};
    qsort_r(base, nel, width, &tmp, &_sort_r_arg_swap);
#elif (defined _WIN32 || defined _WIN64 || defined __WINDOWS__)
    struct _sort_r_data tmp = {arg, cmp};
    qsort_s(*base, nel, width, &sort_r_arg_swap, &tmp);
#else
    #error cannnot detect platform for sort_r
#endif
}

#endif