/* SPDX-License-Identifier: MIT */
#pragma once
#include <stdbool.h>
#include <stdint.h>
void yads_input_enable(bool enabled);
void yads_input_text(uint8_t layer, const char *name, char out[32]);
