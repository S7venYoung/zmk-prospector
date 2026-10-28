#pragma once
#include <stdatomic.h>
typedef _Atomic int atomic_t;
#define ATOMIC_INIT(x) (x)
#define atomic_get(p) atomic_load(p)
#define atomic_set(p,v) atomic_store(p,v)
#define atomic_inc(p) atomic_fetch_add(p,1)
