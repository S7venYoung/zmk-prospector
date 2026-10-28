#pragma once
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))
#define MIN(a,b) ((a)<(b)?(a):(b))
#define MAX(a,b) ((a)>(b)?(a):(b))
#define CLAMP(v,a,b) MIN(MAX(v,a),b)
#define ARG_UNUSED(x) (void)(x)
extern uint32_t host_ticks;
static inline uint32_t k_uptime_get_32(void) { return host_ticks; }
static inline int64_t k_uptime_get(void) { return host_ticks; }

#define IS_ENABLED(x) (x)
