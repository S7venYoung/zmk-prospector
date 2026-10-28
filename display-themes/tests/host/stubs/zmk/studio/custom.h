#pragma once
#include <stdbool.h>
typedef int zmk_custom_CallRequest;
typedef int pb_callback_t;
struct zmk_rpc_custom_subsystem_meta {int unused;int security;};
#define ZMK_RPC_CUSTOM_SUBSYSTEM_UI_URLS() .unused=0
#define ZMK_STUDIO_RPC_HANDLER_UNSECURED 0
#define ZMK_RPC_CUSTOM_SUBSYSTEM(...)
