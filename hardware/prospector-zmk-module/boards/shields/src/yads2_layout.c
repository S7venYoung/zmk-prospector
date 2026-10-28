/*
 * Copyright (c) 2024 The ZMK Contributors
 * SPDX-License-Identifier: MIT
 *
 * YADS2 布局（Dongle 模式）
 *
 * 第二种 YADS 风格界面，按扫描器（dongle）的用途排布：
 * - 左上/右上角：左右手的连接状态（左角 = 左手，右角 = 右手）
 * - 顶部中间：键盘名
 * - 左右角内侧：BLE 指示（默认占位 BLE 1 / BLE 2）
 * - 中间：层滚筒（3 行，当前层居中高亮），下方是 NerdFont 修饰键图标
 * - 底部：每只手各自的电量（百分比 + 进度条）；电量行上方靠右是 RSSI
 *   （"-62dBm"，18px，数据源与 Classic 主屏同一套接口：s7789_update 的
 *   ble_signal_rssi + ble_is_signal_pending()；还没有有效值时显示灰色 "--dBm"）
 *
 * 排布参考上游 YADS 界面 janpfischer/zmk-dongle-screen（MIT 许可）：
 * https://github.com/janpfischer/zmk-dongle-screen/tree/main/boards/shields/dongle_screen
 * 
 *
 * 
 * 使用本机 ZMK 从机发过来的数据更新。
 *
 * 显示区：280x240（与其他布局同一坐标系）。
 */

#include "yads2_layout.h"
#include "fonts_carrefinho.h"
#include "fonts.h"
/* RSSI 数据源：与 Classic 主屏（custom_status_screen.c）用同一对接口 ——
 * ble_signal_rssi 是值，ble_is_signal_pending() 表示是否有新值待刷新。 */
#include "s7789_update.h"
/* 层名仓库：用户可改、落 NVS；没有自定义名时回落到 devicetree display-name */
#include "layer_names.h"
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zmk/keymap.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

LOG_MODULE_REGISTER(yads2_layout, CONFIG_ZMK_LOG_LEVEL);

/* LVGL 内置字体：用于任意文本（键盘名、手的连接状态、BLE 指示）以及
 * LV_SYMBOL_* 图标字形。
 * Carrefinho 字体是字形子集 —— 例如 FG_Medium_20 只到 U+0060，没有小写字母，
 * LVGL 会把缺失字形画成占位方框（CONFIG_LV_USE_FONT_PLACEHOLDER=y）。
 * 这些子集字体只用于固定的全大写/数字字符串。 */
LV_FONT_DECLARE(lv_font_montserrat_16);
LV_FONT_DECLARE(lv_font_montserrat_20);
LV_FONT_DECLARE(lv_font_montserrat_28);
/* RSSI 文本用 12 号：子集字体（DINish/FG）没有小写字母，"-62dBm" 必须有 */
LV_FONT_DECLARE(lv_font_montserrat_12);

/* ========== 颜色 ==========
 * 基调沿用上游 YADS 界面（janpfischer/zmk-dongle-screen）：默认白字，
 * 只有电池有彩色。上游的 output（USB/BLE）控件不使用：本机是 dongle，
 * USB 一定在供电，BLE 指示由 yads2_layout_set_ble() 驱动。 */
#define YADS2_COLOR_TEXT          0xFFFFFF
#define YADS2_COLOR_DIM           0x7B7D93
/* 顶部设备名（两行 "Prospector" / "Receiver"）：Material Light Blue 300，
 * 暗底上明亮清晰，又不像纯白那样抢中间层滚筒的视线 */
#define YADS2_COLOR_NAME          0x4FC3F7
#define YADS2_COLOR_PEER_OK       0x00FF00 /* 该手已连接 */
/* 电池配色 = 红绿灯。文字与进度条同色，进度条在填充端渐变到更亮的同色。 */
#define YADS2_COLOR_BATTERY_HIGH   0x00E676 /* > 50%  */
#define YADS2_COLOR_BATTERY_MID    0xFFC000 /* 11..50% */
#define YADS2_COLOR_BATTERY_OFF    0xE63030 /* <=10%、无数据、未连接 */
#define YADS2_COLOR_BAR_HIGH_TRACK 0x0B3D22
#define YADS2_COLOR_BAR_MID_TRACK  0x4A3808
#define YADS2_COLOR_BAR_LOW_TRACK  0x4A1010
/* 信号强度（RSSI）复用同一套红绿灯，避免再引入一套配色 */
#define YADS2_COLOR_SIGNAL_GOOD YADS2_COLOR_BATTERY_HIGH /* >= -60 dBm */
#define YADS2_COLOR_SIGNAL_FAIR YADS2_COLOR_BATTERY_MID  /* -75 .. -61 dBm */
#define YADS2_COLOR_SIGNAL_WEAK YADS2_COLOR_BATTERY_OFF  /* < -75 dBm */

/* 电量降到该值（含）以下就显示红色（上游 YADS 为 <= 10%） */
#define YADS2_LOW_BATTERY_THRESHOLD 10

/* ========== 几何位置（280x240 坐标系） ========== */
/* 顶部两行：第一行是左右手连接状态（圆角屏的左上 / 右上，向内缩进避开圆角弧线），
 * 第二行是 BLE 指示，两者同一列对齐（左列 "L ✓" / "BLE 1"，右列 "R ✓" / "BLE 2"）；
 * 中间是两行居中的设备名（见 YADS2_NAME_TEXT），与这两列不冲突。 */
#define YADS2_NAME_Y 6
/* 设备名宽度：两行文字各自居中，可用宽度受两侧列限制 —— 第二行夹在 "BLE 1"/"BLE 2"
 * 之间（约 60..220 = 160px），取 146 保证 "Prospector"(20 号字约 110px) 与
 * "Receiver" 都能完整显示（原来单行 104 宽 + LONG_CLIP 会把两边字符裁掉）。 */
#define YADS2_NAME_WIDTH 146
/* 屏幕是圆角面板：角上的文字要往里让，别被圆角弧线切掉。20px 对 280x240 的
 * 圆角（约 20px 半径）刚好在弧线内侧；BLE 指示与它们同列，会自动跟着内缩。 */
#define YADS2_PEER_LEFT_X 20
#define YADS2_PEER_RIGHT_X_OFFSET (-20)
#define YADS2_TOP_Y 6
/* BLE 指示排在左右手状态下方：peer 用 16 号字（行高约 19px），下面一层从 28
 * 开始；BLE 字号 20（行高约 24px），到底部 52 仍留出层滚筒（55 起）的空当。 */
#define YADS2_BLE_Y 28

/* 中间：层滚筒 —— 固定 3 行，当前层永远在中间行（上下为相邻层），三行同一字号；
 * 当前层白色高亮、相邻层灰色。超出 keymap 范围的行隐藏，因此高亮始终居中。
 * 层名通过 zmk_keymap_layer_name() 从本机 keymap 读取，是完整名字
 * （广播里只有 4 个字符）。默认（尚未收到更新）高亮第一层，
 * 之后由 yads2_layout_set_layer() 切换。 */
#define YADS2_LAYER_ROW_COUNT 3
#define YADS2_LAYER_ROW_TOP_Y 55 /* 最上面一行的顶端 */
#define YADS2_LAYER_ROW_STEP 35  /* 行距（FG_Medium_26 行高 < 该值，留一点呼吸感） */
#define YADS2_LAYER_ROW_WIDTH 272

/* 顶部中间的设备名写死（两行居中显示），不再跟随广播里的 keyboard_name
 * （CONFIG_ZMK_KEYBOARD_NAME）：Prospector 在这里是 dongle/接收器，显示对端
 * 键盘名没有意义。文本里的 \n 是 LVGL label 的换行符。 */
#define YADS2_NAME_TEXT "Prospector\nReceiver RS"

/* NerdFont 修饰键行，在层滚筒下方 */
#define YADS2_MOD_Y 160

/* 底部：电量行（始终显示两格）
 * 每只手一条："<L|R> <电量>%"，下面是进度条。
 * 数据源把左手放在 battery_level（槽 0）、右手放在 peripheral_battery[0]
 * （槽 1，见 s7789_update.c / status_advertisement.c），所以显示 L、R 两条；
 * 断开或没有有效读数的那只手显示红色 "--"，不会回退到占位电量。 */
#define YADS2_BATTERY_ROW_WIDTH 268
#define YADS2_BATTERY_ROW_HEIGHT 40
#define YADS2_BATTERY_ROW_Y_OFFSET (-2)
#define YADS2_BATTERY_LABEL_Y 0
#define YADS2_BATTERY_BAR_Y 24
#define YADS2_BATTERY_BAR_HEIGHT 10
#define YADS2_BATTERY_BAR_MAX_WIDTH 130
#define YADS2_BATTERY_BAR_MIN_WIDTH 52
#define YADS2_BATTERY_BAR_GAP 18
/* 还没有键盘名（数据源完全没数据）时，两角的手状态先用"已连接"预览；
 * 电量槽位本身一律如实显示，不再用占位电量。 */
#define YADS2_BATTERY_PLACEHOLDER_LEVEL 50

/* 还没有键盘数据时先预览的槽位数（2 = 分体左右两半的样子） */
#define YADS2_BATTERY_DEFAULT_SLOTS 2

/* 槽位宽度小于该值时省略 '%'，保证 "L 85" 不被裁切 */
#define YADS2_BATTERY_LABEL_NARROW_WIDTH 80

/* ========== 底部：RSSI（信号强度） ==========
 * 紧贴电量行上沿、靠右下角对齐：中间是 NerdFont 修饰键图标行（居中，
 * 最多 4 个图标约 160px 宽），右下的这段空当不会被图标压到。
 * 文本形如 "-62dBm"；还没有有效值（本机模式尚未接通 RSSI 数据源）时显示
 * 灰色 "--dBm"，与 Classic 主屏的信号栏占位一致。
 *
 * 字号 18（原来 12，太小看不清具体数值）：屏宽 280，修饰键图标行居中，
 * 4 个图标时两侧各 (280-160)/2 = 60px 空当，而 18px 的 "-62dBm" 约 60px 宽，
 * 所以只有"4 个修饰键同时按住"这种极端情况才会和最右边的图标贴住几像素
 * （0~3 个修饰键时余量充足）。再往上加字号就会明显压到图标了。
 * X 取 -6 是让文字右边缘与下面的电量行右边缘对齐（电量行宽 268 居中 → 两侧各 6px）。 */
#define YADS2_RSSI_LABEL_X_OFFSET (-6)
#define YADS2_RSSI_LABEL_Y_OFFSET (-50)
/* 着色阈值（dBm）：>= GOOD 绿色，>= FAIR 黄色，更低红色 */
#define YADS2_RSSI_GOOD_THRESHOLD (-60)
#define YADS2_RSSI_FAIR_THRESHOLD (-75)

/* ========== NerdFont 修饰键符号（与 Classic 界面同一套字形） ========== */
static const char *mod_symbols[4] = {
    "\xf3\xb0\x98\xb4", /* Ctrl  (U+F0634) */
    "\xf3\xb0\x98\xb6", /* Shift (U+F0636) */
    "\xf3\xb0\x98\xb5", /* Alt   (U+F0635) */
    "\xf3\xb0\x98\xb3"  /* GUI   (U+F0633) */
};

/* ========== 静态文本缓冲 ==========
 * 用 lv_label_set_text_static() 可避免 LVGL 每条广播都重新分配标签文本，
 * 否则长时间运行会让内存池碎片化。 */
static char stbuf_layer_rows[YADS2_LAYER_ROW_COUNT][24] = {{""}, {""}, {""}};
static char stbuf_peer[2][8] = {{"L "}, {"R "}};
static char stbuf_ble_slots[2][12] = {{"BLE 1"}, {"BLE 2"}};
static char stbuf_mod[64] = "";
static char stbuf_battery[YADS2_MAX_BATTERIES][12] = {{"--"}, {"--"}, {"--"}, {"--"}};
static char stbuf_rssi[12] = "--dBm";

/* ========== 控件状态 ========== */
struct yads2_battery_slot {
    lv_obj_t *label; /* "<L|R> <电量>%" */
    lv_obj_t *bar;   /* 按电量填充的进度条 */
};

static lv_obj_t *layout_container = NULL;
static lv_obj_t *ble_slot_labels[2] = {NULL, NULL};
static lv_obj_t *peer_labels[2] = {NULL, NULL};
static lv_obj_t *name_label = NULL;
static lv_obj_t *layer_rows[YADS2_LAYER_ROW_COUNT] = {NULL};
static lv_obj_t *mod_label = NULL;
static lv_obj_t *battery_row = NULL;
static lv_obj_t *rssi_label = NULL; /* 电量行上方的 "-62dBm" */
static struct yads2_battery_slot battery_slots[YADS2_MAX_BATTERIES];

/* 每个槽位的内容：名称（"L"、"R"、"Aux"、"A1"、"A2"）、最新电量与连接状态 */
static char slot_names[YADS2_MAX_BATTERIES][6] = {{""}, {""}, {""}, {""}};
static uint8_t slot_levels[YADS2_MAX_BATTERIES] = {0, 0, 0, 0};
static bool slot_connected[YADS2_MAX_BATTERIES] = {false, false, false, false};
static int slot_widths[YADS2_MAX_BATTERIES] = {0, 0, 0, 0};

static bool layout_created = false;
static int battery_slot_count = 0;

/* 层滚筒状态：层名来自本机 keymap */
static uint8_t layer_count = 0;   /* keymap 里的层数 */
static uint8_t layer_current = 0; /* 当前高亮的层 */

/* 左上/右上角的 BLE 指示：1..5 = profile 号，其它值显示 "BLE -"；
 * 默认是占位（BLE 1 / BLE 2）。 */
static uint8_t ble_slot_profiles[2] = {1, 2};

/* 缓存值 —— 只有真正变化时才去动 LVGL */
static bool cached_valid = false;
static bool cached_peer[2] = {false, false};
static uint8_t cached_mods = 0;
static char cached_keyboard_name[24] = "";
static uint8_t cached_battery_level = 0;
static bool cached_battery_connected = false;
static uint8_t cached_peripheral_battery[YADS2_MAX_PERIPHERALS] = {0};
static bool cached_peripheral_connected[YADS2_MAX_PERIPHERALS] = {false};
/* RSSI 缓存：cached_rssi_valid = 是否已经收到过有效值（false 时显示 "--dBm"） */
static bool cached_rssi_valid = false;
static int8_t cached_rssi = 0;

/* 用保存的值刷新一个电量槽位（文字 + 进度条） */
static void yads2_render_battery_slot(int slot);

/* 按电量条数给出名称，与 Classic 界面一致 */
static const char *const *battery_names_for_count(int count) {
    static const char *const names_1[] = {""};
    static const char *const names_2[] = {"L", "R"};
    static const char *const names_3[] = {"L", "R", "Aux"};
    static const char *const names_4[] = {"L", "R", "A1", "A2"};

    switch (count) {
    case 2:
        return names_2;
    case 3:
        return names_3;
    case 4:
        return names_4;
    default:
        return names_1;
    }
}

/* ========== 电量行 ========== */

/* 按键盘/手的数量计算槽位宽度并居中排布 */
static void yads2_apply_battery_layout(int count) {
    if (!layout_container || count < 1) {
        return;
    }
    if (count > YADS2_MAX_BATTERIES) {
        count = YADS2_MAX_BATTERIES;
    }

    int bar_width = (YADS2_BATTERY_ROW_WIDTH - (count - 1) * YADS2_BATTERY_BAR_GAP) / count;
    if (bar_width > YADS2_BATTERY_BAR_MAX_WIDTH) {
        bar_width = YADS2_BATTERY_BAR_MAX_WIDTH;
    }
    if (bar_width < YADS2_BATTERY_BAR_MIN_WIDTH) {
        bar_width = YADS2_BATTERY_BAR_MIN_WIDTH;
    }

    int total_width = count * bar_width + (count - 1) * YADS2_BATTERY_BAR_GAP;
    int start_x = (YADS2_BATTERY_ROW_WIDTH - total_width) / 2;
    const char *const *names = battery_names_for_count(count);

    for (int i = 0; i < YADS2_MAX_BATTERIES; i++) {
        struct yads2_battery_slot *slot = &battery_slots[i];
        bool visible = i < count;
        int x = start_x + i * (bar_width + YADS2_BATTERY_BAR_GAP);
        lv_obj_t *objects[2] = {slot->label, slot->bar};

        if (visible) {
            const char *name = (names && names[i]) ? names[i] : "";
            snprintf(slot_names[i], sizeof(slot_names[i]), "%s", name);
            slot_widths[i] = bar_width;
        } else {
            slot_names[i][0] = '\0';
            slot_widths[i] = 0;
        }

        if (slot->label) {
            lv_obj_set_width(slot->label, bar_width);
            lv_obj_set_pos(slot->label, x, YADS2_BATTERY_LABEL_Y);
        }
        if (slot->bar) {
            lv_obj_set_size(slot->bar, bar_width, YADS2_BATTERY_BAR_HEIGHT);
            lv_obj_set_pos(slot->bar, x, YADS2_BATTERY_BAR_Y);
        }

        for (int o = 0; o < 2; o++) {
            if (!objects[o]) {
                continue;
            }
            if (visible) {
                lv_obj_clear_flag(objects[o], LV_OBJ_FLAG_HIDDEN);
            } else {
                lv_obj_add_flag(objects[o], LV_OBJ_FLAG_HIDDEN);
            }
        }

        if (visible) {
            yads2_render_battery_slot(i);
        }
    }

    battery_slot_count = count;
}

/* 把颜色向白色混合（用于电池进度条的渐变末端） */
static uint32_t yads2_lighten(uint32_t color, uint8_t amount) {
    uint32_t r = (color >> 16) & 0xFF;
    uint32_t g = (color >> 8) & 0xFF;
    uint32_t b = color & 0xFF;

    r += ((0xFF - r) * amount) / 0xFF;
    g += ((0xFF - g) * amount) / 0xFF;
    b += ((0xFF - b) * amount) / 0xFF;

    return (r << 16) | (g << 8) | b;
}

/* 拼出 "<名称> <电量>%"（未知时 "<名称> --"）并给该槽位上色 */
static void yads2_render_battery_slot(int slot) {
    if (slot < 0 || slot >= YADS2_MAX_BATTERIES) {
        return;
    }

    struct yads2_battery_slot *w = &battery_slots[slot];
    bool have_level = slot_connected[slot] && slot_levels[slot] > 0;
    /* 所有槽位（槽 0 = 左手、槽 1 = 右手……）都如实显示：没有有效读数就是红色
     * "--"，不再回退到占位电量，否则没连接的手看起来像是一直有 50% 电量。 */
    uint8_t level = have_level ? slot_levels[slot] : 0;

    /* 红绿灯：>50% 绿色，11..50% 琥珀色，<=10% 或无数据显示红色 */
    uint32_t state_color, track_color;
    if (level == 0 || level <= YADS2_LOW_BATTERY_THRESHOLD) {
        state_color = YADS2_COLOR_BATTERY_OFF;
        track_color = YADS2_COLOR_BAR_LOW_TRACK;
    } else if (level <= 50) {
        state_color = YADS2_COLOR_BATTERY_MID;
        track_color = YADS2_COLOR_BAR_MID_TRACK;
    } else {
        state_color = YADS2_COLOR_BATTERY_HIGH;
        track_color = YADS2_COLOR_BAR_HIGH_TRACK;
    }

    if (w->label) {
        /* 窄槽位省略 '%'，避免文字被裁切 */
        bool with_pct = slot_widths[slot] >= YADS2_BATTERY_LABEL_NARROW_WIDTH;

        if (level > 0) {
            if (slot_names[slot][0]) {
                snprintf(stbuf_battery[slot], sizeof(stbuf_battery[slot]),
                         with_pct ? "%s %u%%" : "%s %u", slot_names[slot], level);
            } else {
                snprintf(stbuf_battery[slot], sizeof(stbuf_battery[slot]),
                         with_pct ? "%u%%" : "%u", level);
            }
        } else if (slot_names[slot][0]) {
            snprintf(stbuf_battery[slot], sizeof(stbuf_battery[slot]), "%s --",
                     slot_names[slot]);
        } else {
            snprintf(stbuf_battery[slot], sizeof(stbuf_battery[slot]), "--");
        }
        lv_label_set_text_static(w->label, stbuf_battery[slot]);
        lv_obj_set_style_text_color(w->label, lv_color_hex(state_color), LV_PART_MAIN);
    }

    if (w->bar) {
        lv_bar_set_value(w->bar, level, LV_ANIM_OFF);
        lv_obj_set_style_bg_color(w->bar, lv_color_hex(track_color), LV_PART_MAIN);
        lv_obj_set_style_bg_color(w->bar, lv_color_hex(state_color), LV_PART_INDICATOR);
        lv_obj_set_style_bg_grad_color(w->bar, lv_color_hex(yads2_lighten(state_color, 120)),
                                       LV_PART_INDICATOR);
        lv_obj_set_style_bg_grad_dir(w->bar, LV_GRAD_DIR_HOR, LV_PART_INDICATOR);
    }
}

/* 保存某个槽位的最新电量并刷新显示 */
static void yads2_set_battery_slot(int slot, uint8_t level, bool connected) {
    if (slot < 0 || slot >= YADS2_MAX_BATTERIES) {
        return;
    }

    slot_levels[slot] = level;
    slot_connected[slot] = connected;
    yads2_render_battery_slot(slot);
}

/* ========== BLE 指示（左上/右上角） ========== */

/* 左右两个指示，在调用方通过 yads2_layout_set_ble() 给出真实 profile 之前
 * 只是占位（"BLE 1" / "BLE 2"）。上游 YADS 的 USB 行不显示：本机是 dongle，
 * USB 一定在供电。 */
static void yads2_render_ble_slot(uint8_t slot) {
    if (slot >= 2 || ble_slot_labels[slot] == NULL) {
        return;
    }

    if (ble_slot_profiles[slot] >= 1 && ble_slot_profiles[slot] <= 5) {
        snprintf(stbuf_ble_slots[slot], sizeof(stbuf_ble_slots[slot]), "BLE %u",
                 (unsigned int)ble_slot_profiles[slot]);
    } else {
        snprintf(stbuf_ble_slots[slot], sizeof(stbuf_ble_slots[slot]), "BLE -");
    }

    lv_label_set_text_static(ble_slot_labels[slot], stbuf_ble_slots[slot]);
}

/* 更新一个 BLE 指示：slot 0 = 左（左上角），1 = 右（右上角）；
 * profile 1..5 显示 "BLE n"，其它显示 "BLE -"。 */
void yads2_layout_set_ble(uint8_t slot, uint8_t profile) {
    if (!layout_created || slot >= 2) {
        return;
    }

    ble_slot_profiles[slot] = profile;
    yads2_render_ble_slot(slot);
}

/* ========== 左右手连接状态（顶部两角） ========== */

/* 键盘把左半电量放在 battery_level、右半放在 peripheral_battery[0]；
 * 某一半断开时 ZMK 会报 level < 1，所以"有有效电量"就算已连接
 * （与上游 YADS 的电池控件同一规则）。状态显示在角上，紧挨 BLE 指示。 */
static void yads2_update_peer_status(bool left_ok, bool right_ok) {
    const bool ok[2] = {left_ok, right_ok};

    for (int i = 0; i < 2; i++) {
        if (peer_labels[i] == NULL) {
            continue;
        }

        snprintf(stbuf_peer[i], sizeof(stbuf_peer[i]), "%s%s", (i == 0) ? "L " : "R ",
                 ok[i] ? LV_SYMBOL_OK : LV_SYMBOL_CLOSE);
        lv_label_set_text_static(peer_labels[i], stbuf_peer[i]);
        lv_obj_set_style_text_color(peer_labels[i],
                                    lv_color_hex(ok[i] ? YADS2_COLOR_PEER_OK
                                                       : YADS2_COLOR_BATTERY_OFF),
                                    LV_PART_MAIN);
    }
}

/* ========== 键盘名 / 层 / 修饰键 ========== */

static void yads2_update_name(const char *keyboard_name) {
    if (!name_label) {
        return;
    }

    /* 文本固定为 YADS2_NAME_TEXT（两行：Prospector / Receiver），只有颜色还反映
     * 连接状态：收到键盘数据时用设备名色，完全没有数据时用暗色。 */
    if (keyboard_name && keyboard_name[0]) {
        lv_obj_set_style_text_color(name_label, lv_color_hex(YADS2_COLOR_NAME), LV_PART_MAIN);
    } else {
        lv_obj_set_style_text_color(name_label, lv_color_hex(YADS2_COLOR_DIM), LV_PART_MAIN);
    }
    lv_label_set_text_static(name_label, YADS2_NAME_TEXT);
}

/* ========== 层滚筒（层名来自本机 keymap） ========== */

/* 取某一层的完整名字；名字来自"层名仓库"（用户可在 UI 里改、落 NVS），仓库里
 * 没有这一层时由仓库内部回落到 devicetree display-name，两者都没有就退化为
 * 层号（与上游 YADS 的 layer 控件同一规则）。
 * 注意 index 是"层序号"：index -> layer id 的转换在仓库里做，所以以后即使开启
 * CONFIG_ZMK_KEYMAP_LAYER_REORDERING（层被重排）也不会显示错层的名字。 */
static void yads2_layer_name(uint8_t index, char *out, size_t out_len) {
    const char *name = (index < layer_count) ? prospector_layer_name_get(index) : NULL;

    if (name != NULL && name[0] != '\0') {
        snprintf(out, out_len, "%s", name);
    } else {
        snprintf(out, out_len, "%u", (unsigned int)index);
    }
}

/* 绘制层滚筒：三行以当前层为中心排列，因此当前层永远在中间行，第一层/最后一层
 * 也保持垂直居中（缺的相邻行直接隐藏）。三行同一字号，当前层靠颜色高亮。 */
static void yads2_render_layer_rows(void) {
    if (layer_count == 0 || layer_rows[0] == NULL) {
        return;
    }

    for (uint8_t row = 0; row < YADS2_LAYER_ROW_COUNT; row++) {
        lv_obj_t *label = layer_rows[row];
        if (label == NULL) {
            continue;
        }

        int index = (int)layer_current - 1 + (int)row;
        if (index < 0 || index >= (int)layer_count) {
            lv_obj_add_flag(label, LV_OBJ_FLAG_HIDDEN);
            continue;
        }

        char name[20];
        bool current = ((uint8_t)index == layer_current);

        yads2_layer_name((uint8_t)index, name, sizeof(name));
        snprintf(stbuf_layer_rows[row], sizeof(stbuf_layer_rows[row]), "%s", name);

        lv_label_set_text_static(label, stbuf_layer_rows[row]);
        lv_obj_set_style_text_color(label,
                                    lv_color_hex(current ? YADS2_COLOR_TEXT : YADS2_COLOR_DIM),
                                    LV_PART_MAIN);
        lv_obj_clear_flag(label, LV_OBJ_FLAG_HIDDEN);
    }
}

void yads2_layout_set_layer(uint8_t index) {
    if (!layout_created || layer_count == 0) {
        return;
    }

    if (index >= layer_count) {
        index = (uint8_t)(layer_count - 1);
    }

    layer_current = index;
    yads2_render_layer_rows();
}

static void yads2_update_modifiers(uint8_t modifier_flags) {
    if (!mod_label) {
        return;
    }

    int pos = 0;
    stbuf_mod[0] = '\0';

    if (modifier_flags & 0x11) { /* LCTL | RCTL */
        pos += snprintf(stbuf_mod + pos, sizeof(stbuf_mod) - pos, "%s", mod_symbols[0]);
    }
    if (modifier_flags & 0x22) { /* LSFT | RSFT */
        pos += snprintf(stbuf_mod + pos, sizeof(stbuf_mod) - pos, "%s", mod_symbols[1]);
    }
    if (modifier_flags & 0x44) { /* LALT | RALT */
        pos += snprintf(stbuf_mod + pos, sizeof(stbuf_mod) - pos, "%s", mod_symbols[2]);
    }
    if (modifier_flags & 0x88) { /* LGUI | RGUI */
        pos += snprintf(stbuf_mod + pos, sizeof(stbuf_mod) - pos, "%s", mod_symbols[3]);
    }

    lv_label_set_text_static(mod_label, stbuf_mod);
}

/* ========== 创建 ========== */

static void yads2_create_top_row(lv_obj_t *parent) {
    /* 左右手状态放最外侧角上，BLE 指示排在它正下方同一列（两行而不是挤在一行）：
     * 左列 = "L ✓" / "BLE 1"，右列 = "R ✓" / "BLE 2"（位置本身即代表左右手） */
    for (int slot = 0; slot < 2; slot++) {
        peer_labels[slot] = lv_label_create(parent);
        lv_obj_set_style_text_font(peer_labels[slot], &lv_font_montserrat_16, LV_PART_MAIN);
        lv_obj_set_style_text_color(peer_labels[slot], lv_color_hex(YADS2_COLOR_BATTERY_OFF),
                                    LV_PART_MAIN);
        if (slot == 0) {
            lv_obj_set_pos(peer_labels[slot], YADS2_PEER_LEFT_X, YADS2_TOP_Y);
        } else {
            lv_obj_align(peer_labels[slot], LV_ALIGN_TOP_RIGHT, YADS2_PEER_RIGHT_X_OFFSET,
                         YADS2_TOP_Y);
        }
        lv_label_set_text_static(peer_labels[slot], stbuf_peer[slot]);

        ble_slot_labels[slot] = lv_label_create(parent);
        lv_obj_set_style_text_font(ble_slot_labels[slot], &DINishCondensed_SemiBold_20,
                                   LV_PART_MAIN);
        lv_obj_set_style_text_color(ble_slot_labels[slot], lv_color_hex(YADS2_COLOR_TEXT),
                                    LV_PART_MAIN);
        if (slot == 0) {
            /* 与上面的 "L ✓" 左对齐，下移一行 */
            lv_obj_set_pos(ble_slot_labels[slot], YADS2_PEER_LEFT_X, YADS2_BLE_Y);
        } else {
            /* 与上面的 "R ✓" 右对齐，下移一行 */
            lv_obj_align(ble_slot_labels[slot], LV_ALIGN_TOP_RIGHT, YADS2_PEER_RIGHT_X_OFFSET,
                         YADS2_BLE_Y);
        }
        lv_label_set_text_static(ble_slot_labels[slot], stbuf_ble_slots[slot]);
    }

    /* 顶部中间的设备名：两行居中（"Prospector" / "Receiver"），用 20 号字（比原来
     * 的 16 号大一号），行高约 24px —— 从 y=6 起占 6..54，层滚筒（55 起）刚好让开。
     * 必须用内置字体：文本是任意字符串，子集字体（DINish/FG）没有小写字母。 */
    name_label = lv_label_create(parent);
    lv_obj_set_style_text_font(name_label, &lv_font_montserrat_20, LV_PART_MAIN);
    lv_obj_set_style_text_color(name_label, lv_color_hex(YADS2_COLOR_DIM), LV_PART_MAIN);
    lv_obj_set_style_text_align(name_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_label_set_long_mode(name_label, LV_LABEL_LONG_CLIP);
    lv_obj_set_width(name_label, YADS2_NAME_WIDTH);
    lv_obj_align(name_label, LV_ALIGN_TOP_MID, 0, YADS2_NAME_Y);
    lv_label_set_text_static(name_label, YADS2_NAME_TEXT);
}

static void yads2_create_center(lv_obj_t *parent) {
    /* 层滚筒的三行；三行共用同一字体（当前层只靠颜色高亮，行距才能均匀） */
    for (int row = 0; row < YADS2_LAYER_ROW_COUNT; row++) {
        layer_rows[row] = lv_label_create(parent);
        /* 层名是 keymap 里的完整名字（含小写/空格，如 "Default Layer"），必须用
         * 字形集完整的字体：FR_xx / FG_xx 这类 carrefinho 字体是给大写短文本用的，
         * 缺字形会画成占位方框（见本文件顶部说明）。classic 界面的层名也用这个。 */
        lv_obj_set_style_text_font(layer_rows[row], &lv_font_montserrat_28, LV_PART_MAIN);
        lv_obj_set_style_text_color(layer_rows[row], lv_color_hex(YADS2_COLOR_DIM),
                                    LV_PART_MAIN);
        lv_obj_set_style_text_align(layer_rows[row], LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
        lv_label_set_long_mode(layer_rows[row], LV_LABEL_LONG_CLIP);
        lv_obj_set_width(layer_rows[row], YADS2_LAYER_ROW_WIDTH);
        lv_obj_align(layer_rows[row], LV_ALIGN_TOP_MID, 0,
                     YADS2_LAYER_ROW_TOP_Y + row * YADS2_LAYER_ROW_STEP);
        lv_obj_add_flag(layer_rows[row], LV_OBJ_FLAG_HIDDEN);
    }

    /* NerdFont 修饰键行，在层滚筒下方 */
    mod_label = lv_label_create(parent);
    lv_obj_set_style_text_font(mod_label, &NerdFonts_Regular_40, LV_PART_MAIN);
    lv_obj_set_style_text_color(mod_label, lv_color_hex(YADS2_COLOR_TEXT), LV_PART_MAIN);
    lv_obj_align(mod_label, LV_ALIGN_TOP_MID, 0, YADS2_MOD_Y);
    lv_label_set_text_static(mod_label, "");
}

/* 底部的电量条（每只键盘/手一条） */
static void yads2_create_battery_row(lv_obj_t *parent) {
    lv_obj_t *row = lv_obj_create(parent);
    battery_row = row;
    lv_obj_set_size(row, YADS2_BATTERY_ROW_WIDTH, YADS2_BATTERY_ROW_HEIGHT);
    lv_obj_align(row, LV_ALIGN_BOTTOM_MID, 0, YADS2_BATTERY_ROW_Y_OFFSET);
    lv_obj_set_style_bg_opa(row, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_set_style_border_width(row, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(row, 0, LV_PART_MAIN);
    lv_obj_clear_flag(row, LV_OBJ_FLAG_SCROLLABLE);

    for (int i = 0; i < YADS2_MAX_BATTERIES; i++) {
        struct yads2_battery_slot *slot = &battery_slots[i];

        /* 进度条上方那行 "<L|R> <电量>%" —— 用 semibold 字体做粗体效果，
         * 颜色跟随电池状态 */
        slot->label = lv_label_create(row);
        lv_obj_set_style_text_font(slot->label, &DINishCondensed_SemiBold_22, LV_PART_MAIN);
        lv_obj_set_style_text_color(slot->label, lv_color_hex(YADS2_COLOR_BATTERY_HIGH),
                                    LV_PART_MAIN);
        lv_obj_set_style_text_align(slot->label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
        lv_label_set_text_static(slot->label, stbuf_battery[i]);

        /* 按上报电量填充的进度条（颜色与渐变在 render 里设置） */
        slot->bar = lv_bar_create(row);
        lv_obj_set_size(slot->bar, YADS2_BATTERY_BAR_MAX_WIDTH, YADS2_BATTERY_BAR_HEIGHT);
        lv_bar_set_range(slot->bar, 0, 100);
        lv_bar_set_value(slot->bar, 0, LV_ANIM_OFF);
        lv_obj_set_style_radius(slot->bar, 3, LV_PART_MAIN);
        lv_obj_set_style_radius(slot->bar, 3, LV_PART_INDICATOR);
        lv_obj_set_style_bg_opa(slot->bar, LV_OPA_COVER, LV_PART_MAIN);
        lv_obj_set_style_bg_opa(slot->bar, LV_OPA_COVER, LV_PART_INDICATOR);
        lv_obj_set_style_bg_color(slot->bar, lv_color_hex(YADS2_COLOR_BAR_HIGH_TRACK),
                                  LV_PART_MAIN);
        lv_obj_set_style_bg_color(slot->bar, lv_color_hex(YADS2_COLOR_BATTERY_HIGH),
                                  LV_PART_INDICATOR);
        lv_obj_set_style_bg_grad_color(
            slot->bar, lv_color_hex(yads2_lighten(YADS2_COLOR_BATTERY_HIGH, 120)),
            LV_PART_INDICATOR);
        lv_obj_set_style_bg_grad_dir(slot->bar, LV_GRAD_DIR_HOR, LV_PART_INDICATOR);
    }

    /* 还没有真实数据时先预览分体（L + R）两半的样子 */
    yads2_apply_battery_layout(YADS2_BATTERY_DEFAULT_SLOTS);
}

/* ========== RSSI（电量行上方） ========== */

/* 用缓存值刷新 RSSI 文本与颜色；没有有效值时显示灰色占位 "--dBm" */
static void yads2_render_rssi(void) {
    if (rssi_label == NULL) {
        return;
    }

    if (!cached_rssi_valid) {
        snprintf(stbuf_rssi, sizeof(stbuf_rssi), "--dBm");
        lv_obj_set_style_text_color(rssi_label, lv_color_hex(YADS2_COLOR_DIM), LV_PART_MAIN);
    } else {
        uint32_t color = YADS2_COLOR_SIGNAL_WEAK;

        if (cached_rssi >= YADS2_RSSI_GOOD_THRESHOLD) {
            color = YADS2_COLOR_SIGNAL_GOOD;
        } else if (cached_rssi >= YADS2_RSSI_FAIR_THRESHOLD) {
            color = YADS2_COLOR_SIGNAL_FAIR;
        }
        snprintf(stbuf_rssi, sizeof(stbuf_rssi), "%ddBm", (int)cached_rssi);
        lv_obj_set_style_text_color(rssi_label, lv_color_hex(color), LV_PART_MAIN);
    }

    lv_label_set_text_static(rssi_label, stbuf_rssi);
}

/* 电量行上方的 RSSI 标签（右下角对齐，避开居中的修饰键图标行） */
static void yads2_create_rssi(lv_obj_t *parent) {
    cached_rssi_valid = false;
    cached_rssi = 0;
    snprintf(stbuf_rssi, sizeof(stbuf_rssi), "--dBm");

    rssi_label = lv_label_create(parent);
    /* 18px：12px 在 280x240 的屏上偏小，具体数值看不清（见上方 RSSI 配置注释） */
    lv_obj_set_style_text_font(rssi_label, &lv_font_montserrat_18, LV_PART_MAIN);
    lv_obj_set_style_text_color(rssi_label, lv_color_hex(YADS2_COLOR_DIM), LV_PART_MAIN);
    lv_obj_align(rssi_label, LV_ALIGN_BOTTOM_RIGHT, YADS2_RSSI_LABEL_X_OFFSET,
                 YADS2_RSSI_LABEL_Y_OFFSET);
    lv_label_set_text_static(rssi_label, stbuf_rssi);
}

/* 有新 RSSI 时刷新；没有就保持上一次的值（与主屏信号栏同一规则） */
static void yads2_update_rssi(void) {
    if (!ble_is_signal_pending()) {
        return;
    }

    int8_t rssi = ble_signal_rssi;
    if (cached_rssi_valid && rssi == cached_rssi) {
        return;
    }

    cached_rssi = rssi;
    cached_rssi_valid = true;
    yads2_render_rssi();
}

/* ========== 对外接口 ========== */

/* 定时器兜底刷新（见 yads2_layout.h）：本布局的主刷新是数据驱动的，闲置时没人
 * 按键就没有数据事件，新的 RSSI 会一直躺在 ble_signal_rssi 里没人取走，所以
 * custom_status_screen.c 的 100ms 定时器会调这里。内部自己判断"有没有新值"，
 * 没有就什么都不做（ble_is_signal_pending() 读一次即清零，只由一个消费者取）。 */
void yads2_layout_refresh_rssi(void) { yads2_update_rssi(); }

lv_obj_t *yads2_layout_create(lv_obj_t *parent) {
    if (layout_created) {
        LOG_WRN("YADS2 layout already created");
        return parent;
    }
    if (!parent) {
        return NULL;
    }

    layout_container = parent;

    /* 黑底，与上游 YADS 界面一致 */
    lv_obj_set_style_bg_color(parent, lv_color_hex(0x000000), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(parent, LV_OPA_COVER, LV_PART_MAIN);

    yads2_create_top_row(parent);
    yads2_create_center(parent);
    yads2_create_battery_row(parent);
    yads2_create_rssi(parent);

    /* 第一次 update 时强制全量刷新 */
    cached_valid = false;

    /* 层滚筒：层名来自本机 keymap；在通过 yads2_layout_set_layer() 给出当前层之前，
     * 先高亮第一层 */
    layer_count = (uint8_t)ZMK_KEYMAP_LAYERS_LEN;
    layer_current = 0;

    /* BLE 指示：调用方给出 profile 之前先用占位值 */
    ble_slot_profiles[0] = 1;
    ble_slot_profiles[1] = 2;
    yads2_render_ble_slot(0);
    yads2_render_ble_slot(1);

    layout_created = true;
    yads2_layout_set_layer(0);

    /* 收到第一次 update 之前，手状态先用预览值 */
    yads2_update_peer_status(true, true);

    LOG_INF("YADS2 layout created (%u keymap layers)", (unsigned int)layer_count);
    return parent;
}

void yads2_layout_update(uint8_t active_layer, const char *layer_name,
                         uint8_t battery_level, bool battery_connected,
                         const uint8_t peripheral_battery[YADS2_MAX_PERIPHERALS],
                         const bool peripheral_connected[YADS2_MAX_PERIPHERALS],
                         uint8_t wpm, uint8_t modifier_flags,
                         bool usb_connected, uint8_t ble_profile,
                         bool ble_connected, bool ble_bonded,
                         const char *keyboard_name) {
    if (!layout_created) {
        return;
    }

    ARG_UNUSED(wpm);           /* 本布局不显示 WPM */
    ARG_UNUSED(layer_name);    /* 层名来自本机 keymap */
    ARG_UNUSED(usb_connected); /* 本机是 dongle，USB 一定在供电 */

    if (layer_count > 0 && active_layer < layer_count) {
        yads2_layout_set_layer(active_layer);
    }

    const char *name = (keyboard_name != NULL) ? keyboard_name : "";
    bool have_keyboard = (name[0] != '\0');

    /* 电量槽位：槽 0 = 左手（battery_level）、槽 1 = 右手（peripheral_battery[0]），
     * 之后还有外设时再加格子。
     * 无论外设是否在线都至少保留 L + R 两格，保持"左右两组"的版式；
     * 没有数据的格子如实显示断开（红色 "--"），接上后会自动变多。 */
    int count = YADS2_BATTERY_DEFAULT_SLOTS;
    if (have_keyboard) {
        for (int i = 0; i < YADS2_MAX_PERIPHERALS; i++) {
            if (peripheral_connected[i] || peripheral_battery[i] > 0) {
                int candidate = i + 2;
                if (candidate > count) {
                    count = candidate;
                }
            }
        }
    }
    if (count > YADS2_MAX_BATTERIES) {
        count = YADS2_MAX_BATTERIES;
    }

    if (count != battery_slot_count) {
        yads2_apply_battery_layout(count);
        cached_valid = false; /* 槽位数量变了，重新渲染整行的文字/颜色 */
    }

    /* 还没收到任何数据时电量行也保持显示：格子照常占位，值显示为 "--" */

    /* 键盘名 */
    if (!cached_valid || strncmp(name, cached_keyboard_name, sizeof(cached_keyboard_name)) != 0) {
        yads2_update_name(name);
        snprintf(cached_keyboard_name, sizeof(cached_keyboard_name), "%s", name);
    }

    /* 修饰键 */
    if (!cached_valid || modifier_flags != cached_mods) {
        yads2_update_modifiers(modifier_flags);
        cached_mods = modifier_flags;
    }

    /* BLE 指示由 yads2_layout_set_ble() 驱动（默认占位），这里不使用广播里的
     * profile/flags */

    /* 电量 */
    bool battery_changed = !cached_valid || battery_level != cached_battery_level ||
                           battery_connected != cached_battery_connected;
    for (int i = 0; i < YADS2_MAX_PERIPHERALS; i++) {
        if (peripheral_battery[i] != cached_peripheral_battery[i] ||
            peripheral_connected[i] != cached_peripheral_connected[i]) {
            battery_changed = true;
        }
    }

    if (battery_changed) {
        yads2_set_battery_slot(0, battery_level, battery_connected);
        for (int i = 0; i < YADS2_MAX_PERIPHERALS; i++) {
            yads2_set_battery_slot(i + 1, peripheral_battery[i], peripheral_connected[i]);
            cached_peripheral_battery[i] = peripheral_battery[i];
            cached_peripheral_connected[i] = peripheral_connected[i];
        }
        cached_battery_level = battery_level;
        cached_battery_connected = battery_connected;
    }

    /* RSSI（信号强度）：数据源与本机其它界面相同，有新值才刷新 */
    yads2_update_rssi();

    /* 两角的左右手连接状态：某半只要上报了电量或者有矩阵信息就算已连接；还没收到任何数据时
     * （且电量占位开启）两只手都按已连接显示，让界面看起来完整。
     * BLE 指示由 yads2_layout_set_ble() 驱动、层滚筒由 yads2_layout_set_layer()
     * 驱动， */
    ARG_UNUSED(ble_connected);
    ARG_UNUSED(ble_bonded);
    ARG_UNUSED(ble_profile);

    bool peer_ok[2];
    peer_ok[0] = slot_connected[0] && slot_levels[0] > 0;
    peer_ok[1] = slot_connected[1] && slot_levels[1] > 0;
    if (!have_keyboard && (YADS2_BATTERY_PLACEHOLDER_LEVEL > 0)) {
        peer_ok[0] = true;
        peer_ok[1] = true;
    }

    if (!cached_valid || peer_ok[0] != cached_peer[0] || peer_ok[1] != cached_peer[1]) {
        yads2_update_peer_status(peer_ok[0], peer_ok[1]);
        cached_peer[0] = peer_ok[0];
        cached_peer[1] = peer_ok[1];
    }

    cached_valid = true;
}

void yads2_layout_destroy(void) {
    if (!layout_created) {
        return;
    }

    /* 所有控件都直接挂在 screen / 电量行上 */
    lv_obj_t *objects[] = {battery_row,  rssi_label,     ble_slot_labels[0],
                           ble_slot_labels[1], peer_labels[0], peer_labels[1],
                           name_label,   mod_label};
    for (size_t i = 0; i < sizeof(objects) / sizeof(objects[0]); i++) {
        if (objects[i]) {
            lv_obj_del(objects[i]);
        }
    }

    for (int row = 0; row < YADS2_LAYER_ROW_COUNT; row++) {
        if (layer_rows[row]) {
            lv_obj_del(layer_rows[row]);
            layer_rows[row] = NULL;
        }
    }

    battery_row = NULL;
    rssi_label = NULL;
    ble_slot_labels[0] = NULL;
    ble_slot_labels[1] = NULL;
    peer_labels[0] = NULL;
    peer_labels[1] = NULL;
    name_label = NULL;
    mod_label = NULL;
    memset(battery_slots, 0, sizeof(battery_slots));
    memset(slot_levels, 0, sizeof(slot_levels));
    memset(slot_connected, 0, sizeof(slot_connected));
    memset(cached_peer, 0, sizeof(cached_peer));
    for (int i = 0; i < YADS2_MAX_BATTERIES; i++) {
        slot_names[i][0] = '\0';
        snprintf(stbuf_battery[i], sizeof(stbuf_battery[i]), "--");
    }

    layout_container = NULL;
    layout_created = false;
    battery_slot_count = 0;
    cached_valid = false;
    cached_keyboard_name[0] = '\0';
    cached_rssi_valid = false;
    cached_rssi = 0;
    snprintf(stbuf_rssi, sizeof(stbuf_rssi), "--dBm");
    layer_count = 0;
    layer_current = 0;
    ble_slot_profiles[0] = 1;
    ble_slot_profiles[1] = 2;

    LOG_INF("YADS2 layout destroyed");
}




