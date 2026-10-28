/*
 * Copyright (c) 2026 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

/*
 * custom_status_screen.h - 自定义状态屏的对外接口
 *
 * 目前只暴露"日志页"的开关。日志页是本状态屏的第 6 页（SCREEN_LOG）：
 * 内容来自 qf_display_log.c 的日志采集（见 qf_display_log.h），用 lv_label
 * 配等宽字体（qf_font_log_10x16，10x16 点阵）画出来 —— 和 USB 串口上是同一批日志。
 *
 * 日志页的实现已经独立成 custom_status_screen_log.c（页面控件 + 定时器），
 * 下面这两个函数也在那个文件里；它无条件编译，所以任何配置下都能链接到。
 *
 * 默认不进日志页：开机仍然是正常的电量/状态主屏。要显示由应用调用
 * display_log_page_show(true)：比如以后加一个按键 behavior、层切换或者
 * 手势事件，切进来 / 切回去。
 *
 * 编译开关：CONFIG_PROSPECTOR_DISPLAY_LOG_PAGE（见 Kconfig）。
 * 关掉时这两个函数退化成空操作 / 恒 false，调用方不用改代码。
 */

#pragma once

#include <stdbool.h>

/**
 * @brief 请求显示/隐藏日志页（任意上下文安全，含中断）
 *
 * 只置一个待处理标志，真正的 LVGL 操作在显示线程里做（LVGL 不允许跨线程
 * 调用）。请求被忽略的情况：当前在触摸专用的设置页（Display/System
 * Settings、Keyboard Select），或者切换正在进行中。
 *
 * @param show true 切到日志页；false 切回普通页
 */
void display_log_page_show(bool show);

/**
 * @brief 日志页当前是否可见（任意上下文安全）
 */
bool display_log_page_is_visible(void);