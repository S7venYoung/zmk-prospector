/*
 * Copyright (c) 2026 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

/*
 * custom_status_screen_log.h - 状态屏第 6 页（日志页）的模块内接口
 *
 * 日志页的实现从 custom_status_screen.c 拆到了 custom_status_screen_log.c
 * （那个文件快 3800 行了），两边靠本文件协作：
 *
 *   - custom_status_screen_log.c 拥有这一页的一切：页面控件、文本缓冲、刷新，
 *     以及处理 display_log_page_show() 请求的 LVGL 定时器。
 *   - custom_status_screen.c 仍然是"屏"的所有者：页号（enum screen_state）、
 *     过渡保护、各页控件的销毁/重建。切页必然要碰这些，所以由它实现下面
 *     那组 log_host_* 钩子，交给日志页调用。
 *
 * 边界就一条：谁拥有屏和页号，谁做切页；谁拥有这一页的控件，谁画这一页。
 * 因此下面两组的调用方向正好相反，日志页看不到 enum screen_state。
 *
 * 对外接口仍然是 custom_status_screen.h 里的 display_log_page_show() /
 * display_log_page_is_visible()（实现已移到 custom_status_screen_log.c）。
 */

#pragma once

#include <lvgl.h>
#include <stdbool.h>

/* ========== 日志页侧（custom_status_screen_log.c 实现）========== */

/**
 * @brief 创建日志页的 LVGL 定时器（幂等）
 *
 * 定时器负责两件事：处理切页请求、日志页可见时刷文本。
 * 由 custom_status_screen.c 在屏建好之后调用一次。
 * 没编进日志页（CONFIG_PROSPECTOR_DISPLAY_LOG_PAGE=n）时是空操作。
 */
void custom_status_screen_log_init(void);

/**
 * @brief 建日志页控件
 *
 * 由宿主的 host_enter 钩子在"已清空屏对象"之后调用。
 *
 * @param parent 屏对象（custom_status_screen.c 的 screen_obj）
 */
void custom_status_screen_log_create(lv_obj_t *parent);

/**
 * @brief 清日志页控件指针
 *
 * 控件本身随宿主的 lv_obj_clean() 一起销毁，这里只把指针置空，
 * 免得下一轮日志到齐时写到已释放的对象上。
 */
void custom_status_screen_log_destroy(void);

/**
 * @brief 有新日志就重排文本（无新日志时直接返回）
 *
 * 只在日志页可见时由上面那个定时器调用（宿主 host_visible 为真）。
 */
void custom_status_screen_log_refresh(void);

/* ========== 宿主侧（custom_status_screen.c 实现）========== */

/** @brief 当前是否停在日志页（即 display_log_page_is_visible()） */
bool custom_status_screen_log_host_visible(void);

/**
 * @brief 当前页允不允许切进日志页
 *
 * 只允许从主屏 / Prospector Display 进：触摸专用的设置页（Display/System
 * Settings、Keyboard Select）有自己的返回路径，不在这里被抢走。
 */
bool custom_status_screen_log_host_can_enter(void);

/**
 * @brief 切进日志页：置过渡标志 + 记住来路 + 销毁当前普通页控件 + 清屏 +
 *        建日志页控件 + 把页号切到 SCREEN_LOG
 *
 * @return false 表示过渡正在进行中，本次请求被丢弃
 */
bool custom_status_screen_log_host_enter(void);

/**
 * @brief 从日志页切回进来之前那一页（销毁日志页控件 + 清屏 + 重建普通页）
 */
void custom_status_screen_log_host_leave(void);
