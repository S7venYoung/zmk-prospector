/* SPDX-License-Identifier: MIT */
#pragma once
void prospector_dya_start(void);
void prospector_dya_stop(void);
int prospector_dya_hand_slot(int hand);

int prospector_dya_scanner_keyboard(void);

int prospector_dya_gesture(int theme_step, int brightness_step);
int prospector_dya_touch_get_brightness(void);
void prospector_dya_touch_brightness(int value);
void prospector_dya_touch_palette(void);
