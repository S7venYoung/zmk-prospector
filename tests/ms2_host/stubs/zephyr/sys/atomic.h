#pragma once
#include <zephyr/kernel.h>
typedef int atomic_t;
#define ATOMIC_INIT(x) (x)
static inline void atomic_set(atomic_t *p,int v){*p=v;}
static inline bool atomic_cas(atomic_t*p,int old,int v){if(*p!=old)return false;*p=v;return true;}
