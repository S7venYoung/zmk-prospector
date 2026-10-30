#pragma once
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#define IS_ENABLED(x) (x)
#define ARG_UNUSED(x) (void)(x)
#define MIN(a,b) ((a)<(b)?(a):(b))
#ifdef __weak
#undef __weak
#endif
#define __weak __attribute__((weak))
#define __DMB() __asm__ volatile("" ::: "memory")
#define K_MSEC(x) (x)
#define K_SECONDS(x) ((x)*1000)
extern uint32_t test_now;
static inline uint32_t k_uptime_get_32(void){return test_now;}
struct k_mutex {int unused;};
static inline void k_mutex_init(struct k_mutex*m){(void)m;}
static inline int k_mutex_lock(struct k_mutex*m,int timeout){(void)m;(void)timeout;return 0;}
static inline void k_mutex_unlock(struct k_mutex*m){(void)m;}
struct k_work {int unused;};
struct k_work_delayable {struct k_work work;void(*handler)(struct k_work*);};
#define K_WORK_DELAYABLE_DEFINE(n,h) struct k_work_delayable n={.handler=h}
static inline int k_work_schedule(struct k_work_delayable*w,int delay){(void)w;(void)delay;return 0;}

static inline int k_work_reschedule(struct k_work_delayable*w,int delay){(void)w;(void)delay;return 0;}
