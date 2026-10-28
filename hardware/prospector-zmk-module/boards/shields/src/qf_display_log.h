/*
 * Copyright (c) 2026 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

/*
 * qf_display_log.h - qf_display_log.c 的日志采集层对外接口
 *
 * 日志页（CONFIG_PROSPECTOR_DISPLAY_LOG_PAGE=y, CONFIG_ZMK_DISPLAY=y）用下面
 * 几个接口把环形行缓冲里的文本读走，再用 lv_label 画出来（见
 * custom_status_screen.h 的 display_log_page_show()）。
 *
 * 这里只读写内存，不碰 display API，也不依赖 LVGL，任意上下文可调用。
 */

#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* 一次快照最多需要的字节数：QF_LOG_LINES 行，每行 QF_LOG_COLS 字符 + '\n'。 */
#define QF_DISPLAY_LOG_TEXT_MAX                                                                   \
    (CONFIG_PROSPECTOR_DISPLAY_LOG_LINES * (CONFIG_PROSPECTOR_DISPLAY_LOG_MAX_COLS + 2) + 1)

/**
 * @brief 取一份日志文本快照（最旧一行在前，行间 '\n'，末尾 '\0'）
 *
 * 只读内存（内部 irq_lock），不会读到大半行。屏上能显示多少行由调用方
 * 自己按 buf_size 截断。
 *
 * @param dst      目标缓冲，建议用 struct 或 static 数组，至少
 *                 QF_DISPLAY_LOG_TEXT_MAX 字节
 * @param dst_size 目标缓冲大小
 * @return 实际写入的行数（0 表示缓冲还空着或参数非法）
 */
int qf_display_log_snapshot(char *dst, size_t dst_size);

/**
 * @brief 取"自上次调用以来有没有新日志行"，读走即清
 *
 * 供日志页的 LVGL 定时器判断要不要重排文本，避免没新日志也刷 LVGL。
 *
 * @return true 有新行
 */
bool qf_display_log_take_dirty(void);

/**
 * @brief 清空行缓冲（回到"一块空屏"的状态）
 *
 * 只在日志页的显示线程或初始化阶段调用。
 */
void qf_display_log_clear(void);
