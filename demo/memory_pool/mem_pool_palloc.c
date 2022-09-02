
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#include "mem_core.h"


static inline void *ngx_palloc_small(ngx_pool_t *pool, size_t size,
    ngx_uint_t align);
static void *ngx_palloc_block(ngx_pool_t *pool, size_t size);
static void *ngx_palloc_large(ngx_pool_t *pool, size_t size);


//创建内存池
ngx_pool_t *
ngx_create_pool(size_t size)
{
    ngx_pool_t  *p;

    //开辟空间
    p = ngx_memalign(NGX_POOL_ALIGNMENT, size);
    if (p == NULL) {
        return NULL;
    }

    p->d.last = (u_char *) p + sizeof(ngx_pool_t);//指向实际要分配内存的位置，也就是跳过了前面那些
    p->d.end = (u_char *) p + size;//指向末端

    p->d.next = NULL;
    p->d.failed = 0;

    size = size - sizeof(ngx_pool_t);
    p->max = (size < NGX_MAX_ALLOC_FROM_POOL) ? size : NGX_MAX_ALLOC_FROM_POOL;//检测剩余空间

    p->current = p;//正在工作(分配)的内存块

    p->large = NULL;

    return p;
}


//销毁内存池
void
ngx_destroy_pool(ngx_pool_t *pool)
{
    ngx_pool_t          *p, *n;
    ngx_pool_large_t    *l;
    //ngx_pool_cleanup_t  *c;

    /*for (c = pool->cleanup; c; c = c->next) {
        if (c->handler) {
            ngx_log_debug1(NGX_LOG_DEBUG_ALLOC, pool->log, 0,
                           "run cleanup: %p", c);
            c->handler(c->data);
        }
    }*/

#if (NGX_DEBUG)

    /*
     * we could allocate the pool->log from this pool
     * so we cannot use this log while free()ing the pool
     */
    for (l = pool->large; l; l = l->next) {
        fprintf(stderr,"free: %p", l->alloc);
    }

    for (p = pool, n = pool->d.next; /* void */; p = n, n = n->d.next) {
        fprintf(stderr,"free: %p, unused: %zu", p, p->d.end - p->d.last);

        if (n == NULL) {
            break;
        }
    }

#endif

    //真正的销毁处理流程
    for (l = pool->large; l; l = l->next) {//处理大块
        if (l->alloc) {
            ngx_free(l->alloc);
        }
    }

    //小块
    for (p = pool, n = pool->d.next; /* void */; p = n, n = n->d.next) {
        ngx_free(p);

        if (n == NULL) {
            break;
        }
    }
}


//内存池重置
void
ngx_reset_pool(ngx_pool_t *pool)
{
    ngx_pool_t        *p;
    ngx_pool_large_t  *l;

    //释放大块
    for (l = pool->large; l; l = l->next) {
        if (l->alloc) {
            ngx_free(l->alloc);
        }
    }

    //重置每个小块
    for (p = pool; p; p = p->d.next) {
        p->d.last = (u_char *) p + sizeof(ngx_pool_t);
        p->d.failed = 0;
    }

    pool->current = pool;
    //pool->chain = NULL;
    pool->large = NULL;
}


//从内存池中申请空间
void *
ngx_palloc(ngx_pool_t *pool, size_t size)
{
#if !(NGX_DEBUG_PALLOC)
    if (size <= pool->max) {
        return ngx_palloc_small(pool, size, 1);//如果要分配小内存块-从池子中取
    }
#endif

    return ngx_palloc_large(pool, size);//如果要分配大内存块-扩展
}


void *
ngx_pnalloc(ngx_pool_t *pool, size_t size)
{
#if !(NGX_DEBUG_PALLOC)
    if (size <= pool->max) {
        return ngx_palloc_small(pool, size, 0);
    }
#endif

    return ngx_palloc_large(pool, size);
}

//申请小内存
static inline void *
ngx_palloc_small(ngx_pool_t *pool, size_t size, ngx_uint_t align)
{
    u_char      *m;
    ngx_pool_t  *p;

    p = pool->current;

    do {
        m = p->d.last;

        if (align){//是否边界对齐
            //对齐到为是对齐数的倍数
            m = ngx_align_ptr(m, NGX_ALIGNMENT);
        }

        //剩余内存是否够用
        if ((size_t) (p->d.end - m) >= size) {
            p->d.last = m + size;

            return m;
        }
        
        p = p->d.next;//切换到下一块进行剩余容量判断

    } while (p);

    //实在不行了，重新创建内存块。
    return ngx_palloc_block(pool, size);
}


//创建新的内存块
static void *
ngx_palloc_block(ngx_pool_t *pool, size_t size)
{
    u_char      *m;
    size_t       psize;
    ngx_pool_t  *p, *new;

    psize = (size_t) (pool->d.end - (u_char *) pool);

    m = ngx_memalign(NGX_POOL_ALIGNMENT, psize);//创建内存
    if (m == NULL) {
        return NULL;
    }

    new = (ngx_pool_t *) m;

    new->d.end = m + psize;
    new->d.next = NULL;
    new->d.failed = 0;

    m += sizeof(ngx_pool_data_t);
    m = ngx_align_ptr(m, NGX_ALIGNMENT);//边界对齐
    new->d.last = m + size;

    for (p = pool->current; p->d.next; p = p->d.next) {
        if (p->d.failed++ > 4) {
            pool->current = p->d.next;// >4淘汰掉
        }
    }

    p->d.next = new;

    return m;
}

 
//大内存块
static void *
ngx_palloc_large(ngx_pool_t *pool, size_t size)
{
    void              *p;
    ngx_uint_t         n;
    ngx_pool_large_t  *large;

    p = ngx_alloc(size);//申请一块新的够用大空间
    if (p == NULL) {
        return NULL;
    }

    n = 0;
    //先找有没有可重用的结点，有就直接将刚刚创建好的内存挂到上面
    for (large = pool->large; large; large = large->next) {//遍历池子的大内存链表
        if (large->alloc == NULL) {
            large->alloc = p;
            return p;
        }

        if (n++ > 3) {//就尝试找4次
            break;
        }
    }

    //没有闲置的结点
    //创建一个大内存结点
    large = ngx_palloc_small(pool, sizeof(ngx_pool_large_t), 1);//第一次肯定会直接执行这个
    if (large == NULL) {
        ngx_free(p); 
        return NULL;
    }

    //类似链表头插法
    large->alloc = p;
    large->next = pool->large;
    pool->large = large;

    return p;
}


void *
ngx_pmemalign(ngx_pool_t *pool, size_t size, size_t alignment)
{
    void              *p;
    ngx_pool_large_t  *large;

    p = ngx_memalign(alignment, size);
    if (p == NULL) {
        return NULL;
    }

    large = ngx_palloc_small(pool, sizeof(ngx_pool_large_t), 1);
    if (large == NULL) {
        ngx_free(p);
        return NULL;
    }

    large->alloc = p;
    large->next = pool->large;
    pool->large = large;

    return p;
}


//只对大内存块进行处理
ngx_int_t
ngx_pfree(ngx_pool_t *pool, void *p)
{
    ngx_pool_large_t  *l;

    for (l = pool->large; l; l = l->next) {
        if (p == l->alloc) {
            fprintf(stderr,"free: %p", l->alloc);
            ngx_free(l->alloc);
            l->alloc = NULL;

            return NGX_OK;
        }
    }

    return NGX_DECLINED;
}



void *
ngx_pcalloc(ngx_pool_t *pool, size_t size)
{
    void *p;

    p = ngx_palloc(pool, size);
    if (p) {
        ngx_memzero(p, size);
    }

    return p;
}


