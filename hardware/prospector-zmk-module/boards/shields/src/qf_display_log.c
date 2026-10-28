/*
 * Copyright (c) 2026 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

/*
 * qf_display_log.c - Zephyr 日志采集层（供状态屏的"日志页"使用）
 *
 * 只在"日志页"模式下编译（CONFIG_PROSPECTOR_DISPLAY_LOG_PAGE=y）。本文件注册
 * 一个额外的日志后端（LOG_BACKEND_DEFINE + autostart），与 USB/UART 后端并存
 * —— 屏上和串口上是同一批日志。后端只把字符写进本文件的环形行缓冲，时间戳/
 * 等级/模块名等格式化交给 Zephyr 的 log_output 完成。
 *
 * 屏上显示不在这里：正常固件里屏归 LVGL，日志页（custom_status_screen_log.c）
 * 用 lv_label 把行缓冲的内容画出来，接口见 qf_display_log.h。本文件不碰
 * display API，也不依赖 LVGL。
 *
 * 线程模型
 * ========
 * 字符接收 qf_log_char() 可能来自任何上下文（日志处理线程、系统工作队列、
 * 中断），所以临界区一律用 irq_lock()，里面只做内存拷贝。每攒满一整行就置
 * s_dirty，日志页的 LVGL 定时器读走它并重排文本 —— 没有新日志时不碰 LVGL。
 */

#include <zephyr/kernel.h>
#include <zephyr/irq.h>
#include <zephyr/sys/util.h>
#include <zephyr/logging/log.h>
#include <zephyr/logging/log_msg.h>
#include <zephyr/logging/log_output.h>
#include <zephyr/logging/log_backend.h>
#include <zephyr/logging/log_backend_std.h>
#include <string.h>

LOG_MODULE_REGISTER(qf_display_log, LOG_LEVEL_INF);

/* 可打印字符范围：只有这些字符会进缓冲（字形由日志页的 LVGL 字体提供）。 */
#define QF_LOG_FIRST 0x20
#define QF_LOG_LAST 0x7e

/* ========== 文本缓冲（写者只碰这些，全部在 irq_lock 保护区内） ========== */
#define QF_LOG_COLS CONFIG_PROSPECTOR_DISPLAY_LOG_MAX_COLS
#define QF_LOG_LINES CONFIG_PROSPECTOR_DISPLAY_LOG_LINES

static char s_lines[QF_LOG_LINES][QF_LOG_COLS + 1];
static char s_cur[QF_LOG_COLS + 1]; /* 正在拼装的当前行 */
static uint8_t s_cur_len;
static uint16_t s_head;   /* s_lines 中最新一行的槽位 */
static uint16_t s_filled; /* 已写入行数（不足一屏时屏顶留空） */
static volatile bool s_dirty; /* 有新内容还没被日志页取走 */

/* 有新行时置脏标志；日志页的 LVGL 定时器读走它并重排文本。 */
static void qf_request_refresh(void) {
    s_dirty = true;
}

/* ========== 字符入口（任意上下文，含中断） ========== */

/* 在 irq_lock 保护区内调用 */
static void qf_commit_line(void) {
    s_head++;
    if (s_head >= QF_LOG_LINES) {
        s_head = 0;
    }

    memcpy(s_lines[s_head], s_cur, s_cur_len);
    s_lines[s_head][s_cur_len] = '\0';

    if (s_filled < QF_LOG_LINES) {
        s_filled++;
    }

    s_cur_len = 0;
    s_dirty = true;
}

/* 在 irq_lock 保护区内调用：超宽自动折行，和串口终端一样。
 * 返回值表示这次追加顺手把上一行提交了（有新行，需通知日志页）。 */
static bool qf_append_char(char c) {
    bool committed = false;

    if (s_cur_len >= QF_LOG_COLS) {
        qf_commit_line();
        committed = true;
    }

    s_cur[s_cur_len] = c;
    s_cur_len++;

    return committed;
}

/**
 * @brief 接收一个日志字符（日志后端的输出都汇到这里）
 *
 * 只做内存拷贝，供任意上下文调用；有新行时置脏标志，真正显示交给日志页。
 */
static void qf_log_char(char c) {
    unsigned int key = irq_lock();
    bool line_done = false;

    if (c == '\n') {
        qf_commit_line();
        line_done = true;
    } else if (c == '\r') {
        /* CRLF 里的 CR：忽略（行结束由 \n 提交） */
    } else if (c == '\t') {
        for (int i = 0; i < 4; i++) {
            if (qf_append_char(' ')) {
                line_done = true;
            }
        }
    } else if (c >= QF_LOG_FIRST && c <= QF_LOG_LAST) {
        line_done = qf_append_char(c);
    }
    /* 其它字符直接丢弃 */

    irq_unlock(key);

    if (line_done) {
        /* 攒满一整行就置一次脏标志，日志页据此重排文本。 */
        qf_request_refresh();
    }
}

/* ========== 日志采集（额外的日志后端） ========== */

static uint8_t s_out_buf[64];

/* log_output 的分块回调：把格式化好的文本喂给行缓冲 */
static int qf_output_func(uint8_t *buf, size_t size, void *ctx) {
    ARG_UNUSED(ctx);

    for (size_t i = 0; i < size; i++) {
        qf_log_char((char)buf[i]);
    }

    return (int)size;
}

LOG_OUTPUT_DEFINE(qf_display_log_output, qf_output_func, s_out_buf, sizeof(s_out_buf));

static void qf_backend_process(const struct log_backend *const backend,
                               union log_msg_generic *msg) {
    /* 不传 LOG_OUTPUT_FLAG_COLORS：屏上要的是纯文本，不要 ANSI 转义序列 */
    /* 屏上一行只有 28 个字符，前缀能省就省：默认不打 "<inf> " 这种等级前缀。 */
    uint32_t flags = LOG_OUTPUT_FLAG_CRLF_LFONLY;

    ARG_UNUSED(backend);

    if (IS_ENABLED(CONFIG_PROSPECTOR_DISPLAY_LOG_LEVEL_PREFIX)) {
        flags |= LOG_OUTPUT_FLAG_LEVEL;
    }

    if (IS_ENABLED(CONFIG_PROSPECTOR_DISPLAY_LOG_TIMESTAMP)) {
        flags |= LOG_OUTPUT_FLAG_TIMESTAMP;
    }

    log_output_msg_process(&qf_display_log_output, &msg->log, flags);
}

static void qf_backend_panic(const struct log_backend *const backend) {
    ARG_UNUSED(backend);

    log_backend_std_panic(&qf_display_log_output);
}

static void qf_backend_dropped(const struct log_backend *const backend, uint32_t cnt) {
    ARG_UNUSED(backend);

    log_backend_std_dropped(&qf_display_log_output, cnt);
}

static const struct log_backend_api qf_backend_api = {
    .process = qf_backend_process,
    .panic = qf_backend_panic,
    .dropped = qf_backend_dropped,
};

/* 与 USB/UART 后端并存：屏上和串口上是同一批日志 */
LOG_BACKEND_DEFINE(qf_display_log_backend, qf_backend_api, true);

/* ========== 只读快照：给日志页用（见 qf_display_log.h） ========== */
/* 下面几个接口只读写内存，不碰 display API，任意上下文可调用。 */

/* 行缓冲的可见窗口：最旧一行的槽位 + 可用行数（环形缓冲，未满一屏时屏顶留空）。 */
static void qf_visible_window(uint16_t *head_out, uint16_t *count_out) {
    unsigned int key = irq_lock();
    uint16_t head = s_head;
    uint16_t filled = s_filled;

    if (filled > 0) {
        head = (head + QF_LOG_LINES - (filled - 1)) % QF_LOG_LINES;
    }

    irq_unlock(key);

    *head_out = head;
    *count_out = filled;
}

int qf_display_log_snapshot(char *dst, size_t dst_size) {
    uint16_t head;
    uint16_t count;
    size_t pos = 0;
    int lines = 0;

    if (dst == NULL || dst_size < 2) {
        return 0;
    }

    dst[0] = '\0';

    qf_visible_window(&head, &count);

    for (uint16_t i = 0; i < count; i++) {
        uint16_t slot = (head + i) % QF_LOG_LINES;
        size_t room;
        unsigned int key;
        uint8_t len;

        if (pos >= dst_size - 1) {
            break; /* 缓冲不够了：保留已经写进去的行 */
        }

        /* 本行能放多少字符：留出结尾 '\0'，不是最后一行再多留 1 个 '\n' */
        room = dst_size - pos - 1;

        if ((i + 1 < count) && room > 0) {
            room--;
        }

        if (room > QF_LOG_COLS) {
            room = QF_LOG_COLS;
        }

        key = irq_lock();
        len = (uint8_t)strnlen(s_lines[slot], QF_LOG_COLS);

        if (len > room) {
            len = (uint8_t)room;
        }

        memcpy(&dst[pos], s_lines[slot], len);
        irq_unlock(key);

        pos += len;

        if ((i + 1 < count) && (pos < dst_size - 1)) {
            dst[pos++] = '\n';
        }

        lines++;
    }

    dst[pos] = '\0';

    return lines;
}

bool qf_display_log_take_dirty(void) {
    unsigned int key = irq_lock();
    bool dirty = s_dirty;

    s_dirty = false;
    irq_unlock(key);

    return dirty;
}

void qf_display_log_clear(void) {
    unsigned int key = irq_lock();

    s_head = 0;
    s_filled = 0;
    s_cur_len = 0;
    s_cur[0] = '\0';
    memset(s_lines, 0, sizeof(s_lines));
    s_dirty = false;
    irq_unlock(key);
}
