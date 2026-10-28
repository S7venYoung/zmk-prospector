#pragma once
#include <stddef.h>
#include <cormoran/zmk/custom_settings.h>
struct zmk_custom_setting_changed {const struct zmk_custom_setting *setting;};
typedef struct {int type;struct zmk_custom_setting_changed changed;} zmk_event_t;
static inline const void *as_zmk_custom_settings_initialized(const zmk_event_t*e){return e&&e->type==1?e:NULL;}
static inline const struct zmk_custom_setting_changed *as_zmk_custom_setting_changed(const zmk_event_t*e){return e&&e->type==2?&e->changed:NULL;}
#define ZMK_EV_EVENT_BUBBLE 0
#define ZMK_LISTENER(...)
#define ZMK_SUBSCRIPTION(...)
