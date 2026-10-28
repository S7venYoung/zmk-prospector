#pragma once
#include <stdint.h>
struct zmk_custom_setting {const char *custom_subsystem_id;int32_t *value;};
#define ZMK_CUSTOM_SETTING_VALUE_INT32(x) (x)
#define ZMK_CUSTOM_SETTING_DEFINE(name,subsystem,key,type,initial,...) \
    static int32_t name##_value=(initial); \
    static const struct zmk_custom_setting name={subsystem,&name##_value}
static inline int zmk_custom_setting_get_int32(const struct zmk_custom_setting*s,int32_t*v){*v=*s->value;return 0;}
