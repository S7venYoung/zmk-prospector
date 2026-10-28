#pragma once
#include <stdint.h>
#include <stddef.h>
#include <assert.h>
struct net_buf_simple {uint8_t *data;size_t len;};
static inline uint8_t net_buf_simple_pull_u8(struct net_buf_simple*b){assert(b->len);b->len--;return *b->data++;}
static inline void *net_buf_simple_pull(struct net_buf_simple*b,size_t n){assert(n<=b->len);void*p=b->data;b->data+=n;b->len-=n;return p;}
