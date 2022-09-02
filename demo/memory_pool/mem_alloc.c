
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#include "mem_core.h"

static int debug = 0;

ngx_uint_t  ngx_pagesize;
ngx_uint_t  ngx_pagesize_shift;
ngx_uint_t  ngx_cacheline_size;



void *
ngx_alloc(size_t size)
{
    void  *p;

    p = malloc(size);
    if (p == NULL) {
        fprintf(stderr,"malloc(%zu) failed", size);
    }

    if(debug) fprintf(stderr, "malloc: %p:%zu", p, size);

    return p;
}


void *
ngx_calloc(size_t size)
{
    void  *p;

    p = ngx_alloc(size);

    if (p) {
        ngx_memzero(p, size);
    }

    return p;
}

