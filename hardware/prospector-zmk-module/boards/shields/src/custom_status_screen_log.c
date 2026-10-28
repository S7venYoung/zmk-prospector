/*
 * Copyright (c) 2026 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

/*
 * custom_status_screen_log.c - 状态屏第 6 页（日志页）
 *
 * 这一页原本长在 custom_status_screen.c 里，那个文件快 3800 行了，所以整块
 * 搬到这里。内容是：
 *
 *   1) 页面控件：一个铺满 280x240 的 lv_label，配 qf_font_log_10x16 等宽字体
 *   2) 文本缓冲：从 qf_display_log.c 的采集层取快照（15 行 x 28 字符）
 *   3) 刷新：有新日志行（dirty）时才重排文本，没新行不碰 LVGL
 *   4) 切页请求：display_log_page_show() 从任意上下文（含中断）只置标志，
 *      本文件的 LVGL 定时器在显示线程里把它变成真正的切页动作
 *
 * "屏"本身（页号、过渡保护、各页控件的销毁/重建）仍归 custom_status_screen.c，
 * 所以切页动作全部经 custom_status_screen_log_host_* 钩子完成，
 * 契约见 custom_status_screen_log.h。
 *
 * 本文件无条件编译（见 CMakeLists.txt）：没开日志页时只留下
 * display_log_page_show()/display_log_page_is_visible() 的空实现 ——
 * 这两个是 custom_status_screen.h 里的对外接口，任何时候都得有定义。
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <lvgl.h>
#include "custom_status_screen.h"      /* 对外接口：display_log_page_show()/is_visible() */
#include "custom_status_screen_log.h"  /* 模块内接口 + 宿主钩子 */
#include "qf_display_log.h"            /* 日志采集层（快照 / dirty 标志） */

LOG_MODULE_REGISTER(display_log_page, LOG_LEVEL_INF);

#if IS_ENABLED(CONFIG_PROSPECTOR_DISPLAY_LOG_PAGE)

/* ========== 日志页（第 6 页）==========
 *
 * 内容来自 qf_display_log.c 的采集层（和 USB 串口是同一批日志），
 * 这里只负责用 LVGL 画出来。
 *
 * 字体用 qf_font_log_10x16（见 qf_display_log_lvfont.c）：DroidSansMono 10x16
 * 点阵转成的 LVGL 静态字体，等宽，每字符 10px 宽、16px 行高，于是
 *   CONFIG_PROSPECTOR_DISPLAY_LOG_MAX_COLS（28）x 10px = 280px  铺满屏宽
 *   CONFIG_PROSPECTOR_DISPLAY_LOG_LINES   （15）x 16px = 240px  铺满屏高
 * 正好占满 280x240 的面板。正文从 (0,0) 起、上面不放标题条：标题会额外占掉
 * 一行，等于白白少一行日志，日志页也就不需要它。
 *
 * 为什么不用 LVGL 自带的两个等宽字体：lv_font_unscii_8 只有 8x9，在本机上太小
 * 看不清；lv_font_unscii_16 是 16px 全宽，一行只放得下 17 个字符，日志会被截掉
 * 大半。10x16 既高一倍，又比 unscii_16 窄 37%，一屏仍有 28 x 15 = 420 个字符。
 * 屏归 LVGL，所以这里绝不用 display_write()。
 *
 * 默认不显示：不进日志页时屏上行为和以前完全一样。
 * 切换由 display_log_page_show() 请求，见 custom_status_screen.h。
 */

/* 快照缓冲：LINES 行 x (MAX_COLS + 1) 字符 + 行间 '\n' + 结尾 '\0' */
static char log_text[QF_DISPLAY_LOG_TEXT_MAX];
static lv_obj_t *log_label = NULL;

/* 正文用等宽字体，否则日志列对不齐。字体定义在 qf_display_log_lvfont.c，
 * 由 CMakeLists.txt 在 CONFIG_PROSPECTOR_DISPLAY_LOG_PAGE 下编进固件。 */
LV_FONT_DECLARE(qf_font_log_10x16);

/* 切页请求：display_log_page_show() 从任意上下文（含中断）置，定时器取走。
 * -1 = 无请求，0 = 隐藏，1 = 显示。和主屏那个 pending_swipe 是同一套设计。 */
static volatile int pending_log_page = -1;
static lv_timer_t *log_page_timer = NULL;

static void log_page_timer_cb(lv_timer_t *timer);

void custom_status_screen_log_create(lv_obj_t *parent) {
    if (!parent) {
        return;
    }

    /* 正文：一行一条日志，行距 0、定宽 280 + CLIP，和字符网格严格对齐。
     * 从 (0,0) 开始画：15 行 x 16px = 240px 正好铺满屏高，所以上面不再放
     * "LOG" 标题条 —— 那会占掉一行（16px），等于白白少一行日志。 */
    log_label = lv_label_create(parent);
    lv_obj_set_style_text_font(log_label, &qf_font_log_10x16, 0);
    lv_obj_set_style_text_color(log_label, lv_color_hex(0x00FF00), 0);
    lv_obj_set_style_text_line_space(log_label, 0, 0);
    lv_obj_set_style_text_align(log_label, LV_TEXT_ALIGN_LEFT, 0);
    /* 宽度按字符网格算：MAX_COLS x 10px 字宽。
     * 小屏 = 28 x 10 = 280（铺满 280 宽）；配 big-lcd snippet 时
     * MAX_COLS=32 -> 320（铺满大屏的 320 宽）。固定用 280 会让大屏右边空一条。 */
    lv_obj_set_width(log_label, CONFIG_PROSPECTOR_DISPLAY_LOG_MAX_COLS * 10);
    lv_label_set_long_mode(log_label, LV_LABEL_LONG_CLIP);
    lv_obj_set_pos(log_label, 0, 0);

    /* 立刻画一次现有内容：先丢掉切页之前攒下的脏标志，再强制取一次快照 */
    qf_display_log_take_dirty();

    int lines = qf_display_log_snapshot(log_text, sizeof(log_text));

    /* 用 _static（后面刷新也用同一个版本）：文本不复制、不占堆。
     * 一旦用过 _static（label->static_txt=1），就绝不能再对这个 label 用动态版
     * lv_label_set_text()/lv_label_set_text_fmt() —— 那会 lv_free(log_text)。 */
    lv_label_set_text_static(log_label, log_text);

    LOG_INF("Log page widgets created (%d lines buffered)", lines);
}

void custom_status_screen_log_destroy(void) {
    /* 控件随宿主的 lv_obj_clean(screen_obj) 一起销毁，这里只清指针 */
    log_label = NULL;
}

void custom_status_screen_log_refresh(void) {
    if (!log_label || !qf_display_log_take_dirty()) {
        return;
    }

    qf_display_log_snapshot(log_text, sizeof(log_text));
    /* _static：文本就是 log_text 本身，零拷贝、零 malloc。日志页每来一行都要
     * 整块重排（420 字符），这里省掉的是每次约 420 字节的 malloc/free 和堆碎片，
     * 避免和 LVGL 其它控件抢那块固定内存池（CONFIG_LV_Z_MEM_POOL_SIZE）。
     * lv_label_set_text_static() 内部照样会调 lv_label_refr_text()，
     * 所以 log_text 内容变了能正常重排刷新。 */
    lv_label_set_text_static(log_label, log_text);
}

static void log_page_timer_cb(lv_timer_t *timer) {
    ARG_UNUSED(timer);

    /* 1) 处理切页请求（display_log_page_show() 只置标志） */
    if (pending_log_page >= 0) {
        bool show = (pending_log_page == 1);

        pending_log_page = -1;

        if (show) {
            if (custom_status_screen_log_host_visible()) {
                /* 已经在日志页，无事可做 */
            } else if (!custom_status_screen_log_host_can_enter()) {
                /* 触摸专用的设置页不在这里抢：那几页有自己的返回路径 */
                LOG_WRN("Log page request ignored on this screen");
            } else if (!custom_status_screen_log_host_enter()) {
                LOG_WRN("Log page request ignored - transition already in progress");
            } else {
                LOG_INF(">>> Transitioning: -> LOG");
            }
        } else if (custom_status_screen_log_host_visible()) {
            custom_status_screen_log_host_leave();
            LOG_INF(">>> Transitioning: LOG -> back");
        }
    }

    /* 2) 日志页可见时刷新文本 */
    if (custom_status_screen_log_host_visible()) {
        custom_status_screen_log_refresh();
    }
}

void custom_status_screen_log_init(void) {
    /* 25ms：日志页只在"有新行"时才会碰 LVGL（dirty 标志门控），周期本身几乎
     * 不花 CPU，但把"新行 -> 上屏"的延迟从 100ms 压到 25ms。
     * 注意这个 timer 由 LVGL 的 timer handler 驱动，而 handler 的调用节奏又受
     * CONFIG_LV_DEF_REFR_PERIOD 影响（见 prospector_e73.conf），两者要一起压。 */
    if (!log_page_timer) {
        log_page_timer = lv_timer_create(log_page_timer_cb, 25, NULL);
        LOG_INF("Log page timer registered (25ms interval)");
    }
}

#else /* !CONFIG_PROSPECTOR_DISPLAY_LOG_PAGE */

/* 没编进日志页：定时器自然也没有，但对外接口得照旧有定义，调用方不用改代码 */
void custom_status_screen_log_init(void) {}

#endif /* CONFIG_PROSPECTOR_DISPLAY_LOG_PAGE */

/* ========== 日志页开关（对外接口，见 custom_status_screen.h）========== */

void display_log_page_show(bool show) {
#if IS_ENABLED(CONFIG_PROSPECTOR_DISPLAY_LOG_PAGE)
    /* 只置标志：LVGL 不允许跨线程调用，真正的切页由 log_page_timer_cb()
     * 在显示线程里做。任意上下文（含中断）都可调用。 */
    pending_log_page = show ? 1 : 0;
#else
    ARG_UNUSED(show);
    LOG_DBG("Log page not built in (CONFIG_PROSPECTOR_DISPLAY_LOG_PAGE=n)");
#endif
}

bool display_log_page_is_visible(void) {
#if IS_ENABLED(CONFIG_PROSPECTOR_DISPLAY_LOG_PAGE)
    return custom_status_screen_log_host_visible();
#else
    return false;
#endif
}
