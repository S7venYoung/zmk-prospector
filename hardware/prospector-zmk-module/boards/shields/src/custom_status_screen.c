/**
 * Prospector Scanner UI - Full Widget Test
 * NO CONTAINER PATTERN - All widgets use absolute positioning
 *
 * Screen: 280x240 (90 degree rotated from 240x280)
 *
 * 原触摸功能（CST816S 触摸屏 + CONFIG_PROSPECTOR_TOUCH_ENABLED）已整体移除：
 * 触摸带来的“滑动切页”以及它专属的 Display Settings / Quick Actions /
 * Keyboard Select 三个设置页都不存在了（连 swipe 手势事件、50ms 处理定时器和
 * 这些页面的创建/销毁函数也一并删除）。现在只剩主界面、Prospector Display
 * 和日志页三种屏幕。
 *
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/reboot.h>
#include <zephyr/retention/bootmode.h>  /* For bootmode_set() - Zephyr 4.x bootloader entry */
#include <zephyr/drivers/led.h>  /* For PWM backlight control */
#include <string.h>
#include <lvgl.h>
#include <zmk/display.h>
#include <zmk/display/status_screen.h>
#include <zmk/event_manager.h>
#include <zmk/status_scanner.h>
#if IS_ENABLED(CONFIG_USB_DEVICE_STACK)
#include <zmk/usb.h>
#endif
#include "fonts.h"  /* NerdFont declarations */
#include "brightness_control.h"  /* For auto brightness sensor control */
#include "display_settings.h"   /* NVS persistence for display settings */
#include "layer_names.h"        /* 层名仓库（可改 + NVS 持久化） */
#include "prospector_layouts.h"  /* Carrefinho-inspired display layouts */
#include "yads2_layout.h"        /* yads2_layout_refresh_rssi()（信号栏定时兜底） */
#include "fault_recovery.h"      /* Crash recovery + display watchdog feed */
#include "custom_status_screen_log.h" /* 日志页（第 6 页）：实现在 custom_status_screen_log.c */

LOG_MODULE_REGISTER(display_screen, LOG_LEVEL_INF);

/* ========== 待显示数据（原 scanner_core.c，现由本机数据源提供） ========== */
/* struct pending_display_data 仍在 zmk/scanner_core.h 定义，禁止在本地重复定义
 * （历史上本地副本漂移过一次，导致 scanner_get_pending_update 里多拷贝 8 字节
 *  覆盖了调用方的栈变量）。实现见 s7789_update.c（接口已改名为 ble_*）。 */
#include <zmk/scanner_core.h>
#include "s7789_update.h"

/* LVGL timer for processing pending updates in main thread */
static lv_timer_t *pending_update_timer = NULL;

/* ========== Screen State Management ========== */
enum screen_state {
    SCREEN_MAIN = 0,
    SCREEN_PROSPECTOR_DISPLAY,
    SCREEN_LOG, /* 日志页：数据来自 qf_display_log.c，见 display_log_page_show() */
};

static enum screen_state current_screen = SCREEN_MAIN;
static lv_obj_t *screen_obj = NULL;

/* Transition protection flag - checked by work queues */
volatile bool transition_in_progress = false;

/* Prospector Display active flag - modifies data routing */
volatile bool prospector_display_active = false;

/* 日志页（第 6 页）的请求标志、定时器、控件都不在这里：整页实现搬到了
 * custom_status_screen_log.c（本文件太大了），本文件只保留
 * custom_status_screen_log.h 里那组 log_host_* 钩子，见下面"日志页的宿主钩子"。
 * 另外这个文件不再直接用 display_log_page_show()/is_visible()，也就不需要
 * include custom_status_screen.h 和 qf_display_log.h。 */

/* 自动亮度原本由触摸专属的“Display Settings”页上的开关控制，触摸移除后已经没有
 * 开关可以打开它，定时器也就不再创建（见下面 start_auto_brightness_timer()）。 */

#define AUTO_BRIGHTNESS_INTERVAL_MS 1000  /* Check sensor every 1 second */

static bool ds_auto_brightness_enabled = false;

/* 触摸已彻底移除：没有“显示设置”页上那个自动亮度开关，也就没有任何东西需要
 * 轮询。这里保留空实现，只是为了不改动 load_display_settings() 的调用点；若 NVS
 * 里仍是 auto=on（例如旧触摸固件被覆盖刷成这份），亮度就停在
 * CONFIG_PROSPECTOR_FIXED_BRIGHTNESS。 */
static void start_auto_brightness_timer(void) {}

/* Forward declarations */
static void destroy_main_screen_widgets(void);
static void create_main_screen_widgets(void);
static void destroy_prospector_display_widgets(void);
static void create_prospector_display_widgets(void);

/* Display update functions - called from pending_update_timer_cb */
void display_update_device_name(const char *name);
void display_update_layer(int layer);
void display_update_wpm(int wpm);
void display_update_connection(bool usb_rdy, bool ble_conn, bool ble_bond, int profile);
void display_update_modifiers(uint8_t mods);
void display_update_keyboard_battery_4(int bat0, int bat1, int bat2, int bat3);

/* Modifier flag definitions (from status_advertisement.h) */
#define ZMK_MOD_FLAG_LCTL    (1 << 0)
#define ZMK_MOD_FLAG_LSFT    (1 << 1)
#define ZMK_MOD_FLAG_LALT    (1 << 2)
#define ZMK_MOD_FLAG_LGUI    (1 << 3)
#define ZMK_MOD_FLAG_RCTL    (1 << 4)
#define ZMK_MOD_FLAG_RSFT    (1 << 5)
#define ZMK_MOD_FLAG_RALT    (1 << 6)
#define ZMK_MOD_FLAG_RGUI    (1 << 7)

/* Font declarations */
LV_FONT_DECLARE(lv_font_montserrat_12);
LV_FONT_DECLARE(lv_font_montserrat_16);
LV_FONT_DECLARE(lv_font_montserrat_28);
// LV_FONT_DECLARE(lv_font_unscii_8);
// LV_FONT_DECLARE(lv_font_unscii_16);

/* NerdFont modifier symbols - From YADS project (MIT License) */
static const char *mod_symbols[4] = {
    "\xf3\xb0\x98\xb4",  /* 󰘴 Control (U+F0634) */
    "\xf3\xb0\x98\xb6",  /* 󰘶 Shift (U+F0636) */
    "\xf3\xb0\x98\xb5",  /* 󰘵 Alt (U+F0635) */
    "\xf3\xb0\x98\xb3"   /* 󰘳 GUI/Win/Cmd (U+F0633) */
};

/* ========== Cached data (updated by scanner, preserved across screen transitions) ========== */
static int active_layer = 0;
static int wpm_value = 0;
#define MAX_KB_BATTERIES 4
static int battery_values[MAX_KB_BATTERIES] = {0, 0, 0, 0};  /* Up to 4 keyboard batteries */
static int active_battery_count = 0;  /* How many batteries are active (>0) */
static int8_t rssi = -100;  /* Default: very weak signal */
static float rate_hz = -1.0f;  /* Negative = not yet received, will show as "-.--Hz" */
static int ble_profile = 0;
static bool usb_ready = false;
static bool ble_connected = false;
static bool ble_bonded = false;
static char cached_device_name[32] = "Scanning...";
static uint8_t cached_modifiers = 0;

/* Static text buffers for lv_label_set_text_static()
 * Prevents LVGL internal memory allocation churn that causes
 * memory pool fragmentation over hours of continuous operation.
 * Each buffer persists for program lifetime - safe for LVGL static text. */
static char stbuf_rssi[16] = "?";
static char stbuf_rate[16] = "-.--Hz";
static char stbuf_wpm[8] = "0";
static char stbuf_kb_bat[MAX_KB_BATTERIES][16] = {"0", "0", "0", "0"};
static char stbuf_transport[48] = "";
static char stbuf_modifier[64] = "";

/* ========== PWM Backlight Control ========== */
#if DT_HAS_COMPAT_STATUS_OKAY(pwm_leds)
#define BACKLIGHT_NODE DT_COMPAT_GET_ANY_STATUS_OKAY(pwm_leds)
static const struct device *backlight_dev = DEVICE_DT_GET(BACKLIGHT_NODE);
#else
static const struct device *backlight_dev = NULL;
#endif

static void set_pwm_brightness(uint8_t brightness) {
    if (!backlight_dev || !device_is_ready(backlight_dev)) {
        LOG_WRN("Backlight device not ready");
        return;
    }
    /* Ensure minimum brightness of 1% to prevent screen from going completely dark */
    if (brightness < 1) {
        brightness = 1;
    }
    /* INVERT: Backlight circuit is inverted (100% PWM = dark, 0% = bright)
     * So we invert: user's 100% brightness → 0% PWM duty, 1% brightness → 99% PWM */
    uint8_t pwm_value = 100 - brightness;
    int ret = led_set_brightness(backlight_dev, 0, pwm_value);
    if (ret < 0) {
        LOG_ERR("Failed to set brightness: %d", ret);
    } else {
        LOG_INF("Backlight: user=%d%% -> PWM=%d%%", brightness, pwm_value);
    }
}

/* ========== Widget references (NO CONTAINERS) ========== */

/* Device name */
static lv_obj_t *device_name_label = NULL;

/* WPM */
static lv_obj_t *wpm_title_label = NULL;
static lv_obj_t *wpm_value_label = NULL;

/* Connection status */
static lv_obj_t *transport_label = NULL;
static lv_obj_t *ble_profile_label = NULL;

/* Layer - Fixed mode */
static lv_obj_t *layer_title_label = NULL;
static lv_obj_t *layer_labels[10] = {NULL};
static lv_obj_t *layer_over_max_label = NULL;  /* Large number for over-max display */
static bool layer_mode_over_max = false;       /* true when active_layer >= max_layers */
static int last_active_layer = -1;             /* Track previous layer for animations */

/* Layer - Slide mode */
#define SLIDE_VISIBLE_COUNT 9   /* Number of visible layer slots: 小中大大大大大中小 */
#define SLIDE_LARGE_COUNT 3     /* Number of "large" slots in center */
static lv_obj_t *layer_slide_labels[SLIDE_VISIBLE_COUNT] = {NULL};
static int layer_slide_window_start = 0;  /* First visible layer number in the window */

/* Modifier - placeholder for now (no NerdFont in ZMK test) */
static lv_obj_t *modifier_label = NULL;

/* Keyboard battery - array for up to 4 batteries */
static lv_obj_t *kb_bat_bar[MAX_KB_BATTERIES] = {NULL};      /* Battery bars (connected) */
static lv_obj_t *kb_bat_pct[MAX_KB_BATTERIES] = {NULL};      /* Percentage labels */
static lv_obj_t *kb_bat_name[MAX_KB_BATTERIES] = {NULL};     /* Name labels (L, R, Aux, A1, A2) */
static lv_obj_t *kb_bat_nc_bar[MAX_KB_BATTERIES] = {NULL};   /* Disconnected state bars */
static lv_obj_t *kb_bat_nc_label[MAX_KB_BATTERIES] = {NULL}; /* Disconnected state × symbols */

/* Battery name labels based on count:
 * 1: (no label)
 * 2: "L", "R"
 * 3: "L", "R", "Aux"
 * 4: "L", "R", "A1", "A2"
 */
static const char *battery_names_2[] = {"L", "R", NULL, NULL};
static const char *battery_names_3[] = {"L", "R", "Aux", NULL};
static const char *battery_names_4[] = {"L", "R", "A1", "A2"};

/* Signal status */
static lv_obj_t *channel_label = NULL;
static lv_obj_t *rx_title_label = NULL;
static lv_obj_t *rssi_bar = NULL;
static lv_obj_t *rssi_label = NULL;
static lv_obj_t *rate_label = NULL;

/* Display Settings State (persists across screen transitions, backed by NVS) */
/* ds_auto_brightness_enabled is declared near AUTO_BRIGHTNESS_INTERVAL_MS */
static uint8_t ds_manual_brightness = 65;
static uint8_t ds_max_layers = 7;
static bool ds_layer_slide_mode = IS_ENABLED(CONFIG_PROSPECTOR_LAYER_SLIDE_DEFAULT);
static uint8_t ds_layer_slide_max = 7;

/* Load persisted settings from NVS into display state variables */
static void load_display_settings(void) {
    display_settings_init();
    /* 层名仓库：把 NVS 里的自定义层名读回 RAM，保证首屏渲染前名字已就绪 */
    prospector_layer_names_init();
    ds_auto_brightness_enabled = display_settings_get_auto_brightness();
    ds_manual_brightness = display_settings_get_manual_brightness();
    ds_max_layers = display_settings_get_max_layers();
    ds_layer_slide_mode = display_settings_get_layer_slide_mode();
    LOG_INF("NVS settings loaded: bright=%d/%d%%, layers=%d, slide=%d",
            ds_auto_brightness_enabled, ds_manual_brightness,
            ds_max_layers, ds_layer_slide_mode);

    /* Apply saved brightness setting */
    if (ds_auto_brightness_enabled) {
        brightness_control_set_auto(true);
        start_auto_brightness_timer();
    } else {
        set_pwm_brightness(ds_manual_brightness);
    }
}

/* Forward declarations for layer display helpers */
static void create_layer_list_widgets(lv_obj_t *parent, int y_offset);
// static void destroy_layer_list_widgets(void);
static void create_over_max_widget(lv_obj_t *parent, int layer, int y_offset);
// static void destroy_over_max_widget(void);
/* Slide mode helpers */
static void create_layer_slide_widgets(lv_obj_t *parent, int y_offset);
// static void destroy_layer_slide_widgets(void);
static void update_layer_slide_display(int layer, bool animate);
static lv_color_t get_slide_layer_color(int layer, int max_layer);

/* Runtime channel (defined in system_settings_widget.c, fallback here) */
/* Default to CHANNEL_ALL (10) = show all keyboards */
static uint8_t ks_runtime_channel = 10;  /* CHANNEL_ALL */
static bool ks_channel_initialized = false;

/* Channel functions - try to use system_settings_widget.c version if available */
__attribute__((weak)) uint8_t scanner_get_runtime_channel(void) {
    if (!ks_channel_initialized) {
        ks_runtime_channel = 10;  /* Default: All (CHANNEL_ALL=10) */
        ks_channel_initialized = true;
    }
    return ks_runtime_channel;
}

__attribute__((weak)) void scanner_set_runtime_channel(uint8_t channel) {
    ks_runtime_channel = channel;
    ks_channel_initialized = true;
    LOG_INF("Channel set to %d", channel);
}

/* ========== Color functions ========== */

static lv_color_t get_layer_color(int layer) {
    switch (layer) {
        case 0: return lv_color_make(0xFF, 0x9B, 0x9B);
        case 1: return lv_color_make(0xFF, 0xD9, 0x3D);
        case 2: return lv_color_make(0x6B, 0xCF, 0x7F);
        case 3: return lv_color_make(0x4D, 0x96, 0xFF);
        case 4: return lv_color_make(0xB1, 0x9C, 0xD9);
        case 5: return lv_color_make(0xFF, 0x6B, 0x9D);
        case 6: return lv_color_make(0xFF, 0x9F, 0x43);
        case 7: return lv_color_make(0x87, 0xCE, 0xEB);
        case 8: return lv_color_make(0xF0, 0xE6, 0x8C);
        case 9: return lv_color_make(0xDD, 0xA0, 0xDD);
        default: return lv_color_white();
    }
}

/* Dynamic Hue-based pastel color for slide mode
 * Hue is divided evenly by max_layer count
 * Returns pastel color with 40% saturation, 100% brightness */
static lv_color_t get_slide_layer_color(int layer, int max_layer) {
    if (max_layer <= 0) max_layer = 1;

    /* Calculate Hue (0-360) based on layer position */
    int hue = (layer * 360) / max_layer;
    hue = hue % 360;  /* Wrap around */

    /* HSV to RGB conversion with S=0.4 (pastel), V=1.0 (bright) */
    float s = 0.4f;
    float v = 1.0f;
    float h = hue / 60.0f;
    int i = (int)h;
    float f = h - i;
    float p = v * (1.0f - s);
    float q = v * (1.0f - s * f);
    float t = v * (1.0f - s * (1.0f - f));

    float r, g, b;
    switch (i % 6) {
        case 0: r = v; g = t; b = p; break;
        case 1: r = q; g = v; b = p; break;
        case 2: r = p; g = v; b = t; break;
        case 3: r = p; g = q; b = v; break;
        case 4: r = t; g = p; b = v; break;
        default: r = v; g = p; b = q; break;
    }

    return lv_color_make((uint8_t)(r * 255), (uint8_t)(g * 255), (uint8_t)(b * 255));
}

static lv_color_t get_keyboard_battery_color(int level) {
    if (level >= 80) return lv_color_hex(0x00CC66);
    else if (level >= 60) return lv_color_hex(0x66CC00);
    else if (level >= 40) return lv_color_hex(0xFFCC00);
    else if (level >= 20) return lv_color_hex(0xFF8800);
    else return lv_color_hex(0xFF3333);
}

static uint8_t rssi_to_bars(int8_t rssi_val) {
    if (rssi_val >= -50) return 5;
    if (rssi_val >= -60) return 4;
    if (rssi_val >= -70) return 3;
    if (rssi_val >= -80) return 2;
    if (rssi_val >= -90) return 1;
    return 0;
}

static lv_color_t get_rssi_color(uint8_t bars) {
    switch (bars) {
        case 5: return lv_color_make(0xC0, 0xC0, 0xC0);
        case 4: return lv_color_make(0xA0, 0xA0, 0xA0);
        case 3: return lv_color_make(0x80, 0x80, 0x80);
        case 2: return lv_color_make(0x60, 0x60, 0x60);
        case 1: return lv_color_make(0x40, 0x40, 0x40);
        default: return lv_color_make(0x20, 0x20, 0x20);
    }
}

/* ========== Pending Update Timer Callback (runs in main thread) ========== */
static char last_keyboard_name[MAX_NAME_LEN] = "";  /* Track keyboard changes */

static void pending_update_timer_cb(lv_timer_t *timer) {
    ARG_UNUSED(timer);

    /* Watchdog feed FIRST - before any early return below. This timer is
     * the proof that the display thread is still ticking. */
    fault_recovery_display_alive();

    /* Heartbeat: log every 30 seconds to detect display thread hangs */
    static uint32_t heartbeat_counter = 0;
    if (++heartbeat_counter % 300 == 0) {  /* 300 × 100ms = 30s */
        LOG_INF("Display heartbeat #%u (screen=%d)", heartbeat_counter / 300, current_screen);
    }

    /* Ring buffer is drained by process_work in scanner_core.c (work queue context).
     * LVGL timer only handles display updates from pending_data. */

    /* Skip display updates during screen transitions (defensive guard) */
    if (transition_in_progress) {
        return;
    }

    /* Only process updates on main screen or prospector display */
    if (current_screen != SCREEN_MAIN && current_screen != SCREEN_PROSPECTOR_DISPLAY) {
        return;
    }

    /* 检查是否有待显示的更新（本机数据源） */
    struct pending_display_data data;
    if (ble_get_pending_update(&data)) {
        /* Check if all keyboards have timed out */
        if (data.no_keyboards) {
            LOG_INF("All keyboards timed out - returning to Scanning... state");

            /* Reset display to initial "Scanning..." state */
            display_update_device_name("Scanning...");
            display_update_layer(0);
            display_update_wpm(0);
            display_update_connection(false, false, false, 0);
            display_update_modifiers(0);
            display_update_keyboard_battery_4(0, 0, 0, 0);

            /* Clear last keyboard name so next keyboard triggers battery reposition */
            last_keyboard_name[0] = '\0';
            active_battery_count = -1;

            /* Apply timeout brightness if configured */
#ifdef CONFIG_PROSPECTOR_SCANNER_TIMEOUT_BRIGHTNESS
            if (CONFIG_PROSPECTOR_SCANNER_TIMEOUT_BRIGHTNESS > 0) {
                set_pwm_brightness(CONFIG_PROSPECTOR_SCANNER_TIMEOUT_BRIGHTNESS);
                LOG_INF("Timeout brightness set to %d%%", CONFIG_PROSPECTOR_SCANNER_TIMEOUT_BRIGHTNESS);
            }
#endif
            return;
        }

        /* Detect keyboard change - reset battery count to force full reposition */
        if (strcmp(last_keyboard_name, data.device_name) != 0) {
            LOG_INF("Keyboard changed: %s -> %s, resetting battery layout",
                    last_keyboard_name, data.device_name);
            strncpy(last_keyboard_name, data.device_name, MAX_NAME_LEN - 1);
            last_keyboard_name[MAX_NAME_LEN - 1] = '\0';
            active_battery_count = -1;  /* Force reposition on next battery update */

            /* Restore normal brightness when keyboard activity resumes */
#ifdef CONFIG_PROSPECTOR_FIXED_BRIGHTNESS
            set_pwm_brightness(CONFIG_PROSPECTOR_FIXED_BRIGHTNESS);
            LOG_INF("Brightness restored to %d%%", CONFIG_PROSPECTOR_FIXED_BRIGHTNESS);
#endif
        }

        if (current_screen == SCREEN_PROSPECTOR_DISPLAY) {
            /* Route data to Prospector Display layouts */
            struct prospector_keyboard_data kb_data = {0};
            kb_data.active_layer = data.layer;
            kb_data.modifier_flags = data.modifiers;
            kb_data.wpm_value = data.wpm;
            /* 本机只有左右手两台设备：其余外设格位由上面的 {0} 初始化保持 0 */
            kb_data.battery_level = data.bat[0];
            kb_data.peripheral_battery[0] = data.bat[1];
            kb_data.profile_slot = data.profile;
            kb_data.usb_connected = data.usb_ready;
            kb_data.ble_connected = data.ble_connected;
            kb_data.ble_bonded = data.ble_bonded;
            kb_data.has_dynamic_data = true;
            strncpy(kb_data.keyboard_name, data.device_name,
                    sizeof(kb_data.keyboard_name) - 1);
            /* Layer name from BLE advertisement (4 chars, not null-terminated) */
            memcpy(kb_data.current_layer_name, data.layer_name, 4);
            kb_data.current_layer_name[4] = '\0';
            prospector_layouts_update(&kb_data);
        } else {
            /* SCREEN_MAIN: Update YADS-style widgets */
            display_update_device_name(data.device_name);
            display_update_layer(data.layer);
            display_update_wpm(data.wpm);
            display_update_connection(data.usb_ready, data.ble_connected,
                                      data.ble_bonded, data.profile);
            display_update_modifiers(data.modifiers);

            /* Battery update：本机只有左右手两台设备，格子数由上面这个函数
             * 按“值 > 0”自行决定（右手无读数时自然退化成单格），剩余格位恒为 0 */
            display_update_keyboard_battery_4(data.bat[0], data.bat[1], 0, 0);
        }
    }

    /* 信号栏更新（信号与主数据分开，1Hz；数据来自 app/src/signal_cb.c） */
    /* 直接读全局量并在本函数内更新显示（避免任何带 float 参数的调用）
     * 注意：ble_is_signal_pending() 读取一次就清零，所以"取走"必须由当前真正
     * 在显示的那套皮肤来做，免得新值被另一套皮肤先吃掉、界面一直不更新。 */
    if (current_screen == SCREEN_PROSPECTOR_DISPLAY &&
        prospector_layouts_get_style() == PROSPECTOR_LAYOUT_YADS2) {
        /* yads2 布局的主刷新是数据驱动的（不按键就没有数据事件），RSSI 由这个
         * 100ms 定时器兜底；内部没有新值时会立刻返回，不会白重绘。 */
        yads2_layout_refresh_rssi();
    } else if (ble_is_signal_pending()) {
        int8_t sig_rssi = ble_signal_rssi;
        int32_t sig_rate_x100 = ble_signal_rate_x100;

        /* Update signal display INLINE (no function call with float param) */
        rssi = sig_rssi;
        rate_hz = (float)sig_rate_x100 / 100.0f;

        uint8_t bars = rssi_to_bars(sig_rssi);
        if (rssi_bar) {
            lv_bar_set_value(rssi_bar, bars, LV_ANIM_OFF);
            lv_obj_set_style_bg_color(rssi_bar, get_rssi_color(bars), LV_PART_INDICATOR);
        }
        if (rssi_label) {
            snprintf(stbuf_rssi, sizeof(stbuf_rssi), "%ddBm", sig_rssi);
            lv_label_set_text_static(rssi_label, stbuf_rssi);
        }
        if (rate_label) {
            if (sig_rate_x100 < 0) {
                snprintf(stbuf_rate, sizeof(stbuf_rate), "-.--Hz");
            } else {
                /* Display from integer directly: rate_x100 / 100 . rate_x100 % 100 */
                int whole = sig_rate_x100 / 100;
                int frac = (sig_rate_x100 % 100) / 10;  /* One decimal place */
                snprintf(stbuf_rate, sizeof(stbuf_rate), "%d.%dHz", whole, frac);
            }
            lv_label_set_text_static(rate_label, stbuf_rate);
        }
    }
}

/* ========== Main Screen Creation (NO CONTAINERS) ========== */

lv_obj_t *zmk_display_status_screen(void) {
    LOG_INF("=============================================");
    LOG_INF("=== Full Widget Test - NO CONTAINER ===");
    LOG_INF("=== All widgets use absolute positioning ===");
    LOG_INF("=============================================");

    /* Load persisted display settings from NVS flash */
    load_display_settings();

    /* Create main screen */
    LOG_INF("[INIT] Creating main_screen...");
    lv_obj_t *screen = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(screen, lv_color_black(), 0);
    lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, 0);
    lv_obj_clear_flag(screen, LV_OBJ_FLAG_SCROLLABLE);
    LOG_INF("[INIT] main_screen created");

    /* ===== 1. Device Name (TOP_MID, y=25) ===== */
    // LOG_INF("[INIT] Creating device name...");
    // device_name_label = lv_label_create(screen);
    // lv_obj_set_style_text_font(device_name_label, &lv_font_unscii_16, 0);
    // lv_obj_set_style_text_color(device_name_label, lv_color_white(), 0);
    // lv_label_set_text(device_name_label, "Scanning...");
    // lv_obj_align(device_name_label, LV_ALIGN_TOP_MID, 0, 25);
    // LOG_INF("[INIT] device name created");


    /* ===== 4. Connection Status (TOP_RIGHT) ===== */
    LOG_INF("[INIT] Creating connection status...");
    transport_label = lv_label_create(screen);
    lv_obj_set_style_text_font(transport_label, &lv_font_montserrat_12, 0);
    lv_obj_set_style_text_color(transport_label, lv_color_white(), 0);
    lv_obj_set_style_text_align(transport_label, LV_TEXT_ALIGN_RIGHT, 0);
    lv_label_set_recolor(transport_label, true);
    lv_obj_align(transport_label, LV_ALIGN_TOP_RIGHT, -10, 53);

    /* Initial state: BLE with profile on new line */
    lv_label_set_text(transport_label, "#ffffff BLE#\n#ffffff 0#");

    /* Profile label kept but hidden (integrated into transport_label) */
    ble_profile_label = lv_label_create(screen);
    lv_obj_set_style_text_font(ble_profile_label, &lv_font_montserrat_12, 0);
    lv_obj_set_style_text_color(ble_profile_label, lv_color_white(), 0);
    lv_label_set_text(ble_profile_label, "");  /* Hidden - integrated */
    lv_obj_align(ble_profile_label, LV_ALIGN_TOP_RIGHT, -8, 78);
    LOG_INF("[INIT] connection status created");

    /* ===== 5. Layer Widget (CENTER area, y=85-120) ===== */
    LOG_INF("[INIT] Creating layer widget...");
    layer_title_label = lv_label_create(screen);
    lv_obj_set_style_text_font(layer_title_label, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_color(layer_title_label, lv_color_make(160, 160, 160), 0);
    lv_obj_set_style_text_opa(layer_title_label, LV_OPA_70, 0);
    lv_label_set_text(layer_title_label, "Layer");
    lv_obj_align(layer_title_label, LV_ALIGN_TOP_MID, 0, 82);  /* 3px up */

    /* Create layer display - slide mode OR fixed mode (list/over-max) */
    if (ds_layer_slide_mode) {
        /* Slide mode: create 7-slot dial display */
        create_layer_slide_widgets(screen, 105);
        layer_mode_over_max = false;  /* Not used in slide mode */
    } else if (active_layer >= ds_max_layers) {
        layer_mode_over_max = true;
        create_over_max_widget(screen, active_layer, 105);
    } else {
        layer_mode_over_max = false;
        create_layer_list_widgets(screen, 105);
    }
    LOG_INF("[INIT] layer widget created");

    /* ===== 6. Modifier Widget (CENTER, y=145) - NerdFont icons ===== */
    LOG_INF("[INIT] Creating modifier widget with NerdFont...");
    modifier_label = lv_label_create(screen);
    lv_obj_set_style_text_font(modifier_label, &NerdFonts_Regular_40, 0);
    lv_obj_set_style_text_color(modifier_label, lv_color_white(), 0);
    lv_obj_set_style_text_letter_space(modifier_label, 10, 0);  /* Space between icons */
    lv_label_set_text(modifier_label, "");  /* Empty initially */
    lv_obj_align(modifier_label, LV_ALIGN_TOP_MID, 0, 145);
    LOG_INF("[INIT] modifier widget created");

    /* ===== 7. Keyboard Battery (dynamic layout for 1-4 batteries) ===== */
    LOG_INF("[INIT] Creating keyboard battery widgets...");

    /* Position constants - configurable for different battery counts */
    #define KB_BAR_HEIGHT      4
    #define KB_BAR_Y_OFFSET    -33    /* Distance from bottom */
    #define KB_PCT_Y_OFFSET    -42    /* Percentage label above bar */
    #define KB_NAME_X_OFFSET   0      /* Name label right edge aligns with bar left edge */

    /* Layout for different battery counts (bar width and positions) */
    /* 1 battery: centered, width 165 (1.5x of 110) */
    /* 2 batteries: L/R side by side, width 110 each */
    /* 3 batteries: width 70 each, spread across */
    /* 4 batteries: width 52 each, spread across */
    #define KB_BAR_WIDTH_1     165
    #define KB_BAR_WIDTH_2     110
    #define KB_BAR_WIDTH_3     70
    #define KB_BAR_WIDTH_4     52

    /* X offsets for each layout (from center) */
    // static const int16_t kb_x_offsets_1[] = {0};
    static const int16_t kb_x_offsets_2[] = {-70, 70};
    // static const int16_t kb_x_offsets_3[] = {-90, 0, 90};
    // static const int16_t kb_x_offsets_4[] = {-100, -35, 35, 100};

    /* Create all 4 battery slot widgets (initially hidden) */
    for (int i = 0; i < MAX_KB_BATTERIES; i++) {
        /* Default to 2-battery layout initially */
        int16_t bar_width = KB_BAR_WIDTH_2;
        int16_t x_offset = (i < 2) ? kb_x_offsets_2[i] : 0;

        /* Connected state bar */
        kb_bat_bar[i] = lv_bar_create(screen);
        lv_obj_set_size(kb_bat_bar[i], bar_width, KB_BAR_HEIGHT);
        lv_obj_align(kb_bat_bar[i], LV_ALIGN_BOTTOM_MID, x_offset, KB_BAR_Y_OFFSET);
        lv_bar_set_range(kb_bat_bar[i], 0, 100);
        lv_bar_set_value(kb_bat_bar[i], 0, LV_ANIM_OFF);
        lv_obj_set_style_bg_color(kb_bat_bar[i], lv_color_hex(0x202020), LV_PART_MAIN);
        lv_obj_set_style_bg_opa(kb_bat_bar[i], 255, LV_PART_MAIN);
        lv_obj_set_style_radius(kb_bat_bar[i], 1, LV_PART_MAIN);
        lv_obj_set_style_bg_color(kb_bat_bar[i], lv_color_hex(0x909090), LV_PART_INDICATOR);
        lv_obj_set_style_bg_opa(kb_bat_bar[i], 255, LV_PART_INDICATOR);
        lv_obj_set_style_bg_grad_color(kb_bat_bar[i], lv_color_hex(0xf0f0f0), LV_PART_INDICATOR);
        lv_obj_set_style_bg_grad_dir(kb_bat_bar[i], LV_GRAD_DIR_HOR, LV_PART_INDICATOR);
        lv_obj_set_style_radius(kb_bat_bar[i], 1, LV_PART_INDICATOR);
        lv_obj_set_style_opa(kb_bat_bar[i], 0, LV_PART_MAIN);
        lv_obj_set_style_opa(kb_bat_bar[i], 0, LV_PART_INDICATOR);

        /* Percentage label (above bar, centered) */
        kb_bat_pct[i] = lv_label_create(screen);
        lv_obj_set_style_text_font(kb_bat_pct[i], &lv_font_montserrat_12, 0);
        lv_obj_set_style_text_color(kb_bat_pct[i], lv_color_white(), 0);
        lv_obj_align(kb_bat_pct[i], LV_ALIGN_BOTTOM_MID, x_offset, KB_PCT_Y_OFFSET);
        lv_label_set_text(kb_bat_pct[i], "0");
        lv_obj_set_style_opa(kb_bat_pct[i], 0, 0);

        /* Name label (left of bar, same height as percentage) */
        kb_bat_name[i] = lv_label_create(screen);
        lv_obj_set_style_text_font(kb_bat_name[i], &lv_font_montserrat_12, 0);
        lv_obj_set_style_text_color(kb_bat_name[i], lv_color_hex(0x808080), 0);
        lv_obj_align(kb_bat_name[i], LV_ALIGN_BOTTOM_MID, x_offset - bar_width/2 + KB_NAME_X_OFFSET, KB_PCT_Y_OFFSET);
        lv_obj_set_style_text_align(kb_bat_name[i], LV_TEXT_ALIGN_RIGHT, 0);
        lv_label_set_text(kb_bat_name[i], "");
        lv_obj_set_style_opa(kb_bat_name[i], 0, 0);

        /* Disconnected state bar */
        kb_bat_nc_bar[i] = lv_obj_create(screen);
        lv_obj_set_size(kb_bat_nc_bar[i], bar_width, KB_BAR_HEIGHT);
        lv_obj_align(kb_bat_nc_bar[i], LV_ALIGN_BOTTOM_MID, x_offset, KB_BAR_Y_OFFSET);
        lv_obj_set_style_bg_color(kb_bat_nc_bar[i], lv_color_hex(0x9e2121), LV_PART_MAIN);
        lv_obj_set_style_bg_opa(kb_bat_nc_bar[i], 255, LV_PART_MAIN);
        lv_obj_set_style_radius(kb_bat_nc_bar[i], 1, LV_PART_MAIN);
        lv_obj_set_style_border_width(kb_bat_nc_bar[i], 0, 0);
        lv_obj_set_style_pad_all(kb_bat_nc_bar[i], 0, 0);
        /* Initially hide slots 2 and 3 (only show first 2 by default) */
        lv_obj_set_style_opa(kb_bat_nc_bar[i], (i < 2) ? 255 : 0, 0);

        /* Disconnected state label (× symbol) */
        kb_bat_nc_label[i] = lv_label_create(screen);
        lv_obj_set_style_text_font(kb_bat_nc_label[i], &lv_font_montserrat_12, 0);
        lv_obj_set_style_text_color(kb_bat_nc_label[i], lv_color_hex(0xe63030), 0);
        lv_obj_align(kb_bat_nc_label[i], LV_ALIGN_BOTTOM_MID, x_offset, KB_PCT_Y_OFFSET);
        lv_label_set_text(kb_bat_nc_label[i], LV_SYMBOL_CLOSE);
        lv_obj_set_style_opa(kb_bat_nc_label[i], (i < 2) ? 255 : 0, 0);
    }

    LOG_INF("[INIT] keyboard battery widgets created (4 slots)");

    /* ===== 8. Signal Status (BOTTOM, y=220) ===== */
    LOG_INF("[INIT] Creating signal status...");

    channel_label = lv_label_create(screen);
    lv_obj_set_style_text_font(channel_label, &lv_font_montserrat_12, 0);
    lv_obj_set_style_text_color(channel_label, lv_color_make(0x80, 0x80, 0x80), 0);
    lv_label_set_text(channel_label, "Ch:0");
    lv_obj_set_pos(channel_label, 62, 219);  /* 5px down, 5px left */

    rx_title_label = lv_label_create(screen);
    lv_obj_set_style_text_font(rx_title_label, &lv_font_montserrat_12, 0);
    lv_obj_set_style_text_color(rx_title_label, lv_color_make(0x80, 0x80, 0x80), 0);
    lv_label_set_text(rx_title_label, "RX:");
    lv_obj_set_pos(rx_title_label, 102, 219);  /* 5px down, 5px left */

    rssi_bar = lv_bar_create(screen);
    lv_obj_set_size(rssi_bar, 30, 8);
    lv_obj_set_pos(rssi_bar, 130, 223);  /* RX indicator position */
    lv_bar_set_range(rssi_bar, 0, 5);
    lv_bar_set_value(rssi_bar, 0, LV_ANIM_OFF);
    lv_obj_set_style_bg_color(rssi_bar, lv_color_make(0x20, 0x20, 0x20), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(rssi_bar, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_bg_color(rssi_bar, get_rssi_color(0), LV_PART_INDICATOR);
    lv_obj_set_style_bg_opa(rssi_bar, LV_OPA_COVER, LV_PART_INDICATOR);
    lv_obj_set_style_radius(rssi_bar, 2, LV_PART_MAIN);
    lv_obj_set_style_radius(rssi_bar, 2, LV_PART_INDICATOR);

    rssi_label = lv_label_create(screen);
    lv_obj_set_style_text_font(rssi_label, &lv_font_montserrat_12, 0);
    lv_obj_set_style_text_color(rssi_label, lv_color_make(0xA0, 0xA0, 0xA0), 0);
    lv_label_set_text(rssi_label, "0dBm");
    lv_obj_set_pos(rssi_label, 167, 219);  /* 5px down, 5px left */

    rate_label = lv_label_create(screen);
    lv_obj_set_style_text_font(rate_label, &lv_font_montserrat_12, 0);
    lv_obj_set_style_text_color(rate_label, lv_color_make(0xA0, 0xA0, 0xA0), 0);
    lv_label_set_text(rate_label, "0.0Hz");
    lv_obj_set_pos(rate_label, 222, 219);  /* 5px down, 5px left */
    LOG_INF("[INIT] signal status created");

    LOG_INF("=============================================");
    LOG_INF("=== Full Widget Test Complete ===");
    LOG_INF("=== Display-only build: touch / swipe navigation removed ===");
    LOG_INF("=============================================");

    /* Save screen reference for screen transitions */
    screen_obj = screen;

    /* Check if Prospector Display layout is configured as default */
#if CONFIG_PROSPECTOR_DEFAULT_LAYOUT > 0
    /* Non-YADS layout selected: switch to Prospector Display immediately.
     * 触摸输入（以及随触摸而来的滑动切页）已移除，所以这里就是常驻屏幕，
     * 只有重启才会重新按 NVS / Kconfig 选择布局。 */
    {
        /* Destroy YADS widgets that were just created above */
        destroy_main_screen_widgets();
        lv_obj_clean(screen);
        lv_obj_set_style_bg_color(screen, lv_color_black(), 0);

        /* Create Prospector Display with configured layout */
        create_prospector_display_widgets();

        /* Override NVS-saved layout with Kconfig default on first boot */
        prospector_layout_t kconfig_layout = (prospector_layout_t)CONFIG_PROSPECTOR_DEFAULT_LAYOUT;
        if (prospector_layouts_get_style() != kconfig_layout) {
            prospector_layouts_set_style(kconfig_layout);
        }

        current_screen = SCREEN_PROSPECTOR_DISPLAY;
        LOG_INF("Default layout: %s (from Kconfig)",
                prospector_layouts_get_name(kconfig_layout));
    }
#else
    current_screen = SCREEN_MAIN;
#endif

    /* Create pending update timer - processes Work Queue data in main thread */
    if (!pending_update_timer) {
        pending_update_timer = lv_timer_create(pending_update_timer_cb, 100, NULL);
        LOG_INF("Pending update timer registered (100ms interval)");
    }

    /* Log page (screen 6): its LVGL timer lives in custom_status_screen_log.c
     * together with the rest of that page - a request flag is set from any
     * context, that timer does the actual LVGL work (same design as the timers
     * above). No-op when the page is not built in. */
    custom_status_screen_log_init();

    return screen;
}

/* ========== Widget Update Functions (called from scanner_core.c) ========== */

void display_update_device_name(const char *name) {
    if (name) {
        strncpy(cached_device_name, name, sizeof(cached_device_name) - 1);
        cached_device_name[sizeof(cached_device_name) - 1] = '\0';
    }
    if (device_name_label && name) {
        lv_label_set_text_static(device_name_label, cached_device_name);
    }
}

/* Animation callback for horizontal (X) slide - for over-max label (uses align) */
static void layer_slide_x_anim_cb(void *var, int32_t value) {
    lv_obj_t *obj = (lv_obj_t *)var;
    lv_obj_align(obj, LV_ALIGN_TOP_MID, value, 105);  /* Keep Y at 105, animate X offset */
}

/* Animation callback for absolute X position - for layer list labels */
static void layer_pos_x_anim_cb(void *var, int32_t value) {
    lv_obj_set_x((lv_obj_t *)var, value);
}

/* Animation callback for pulse highlight effect.
 *
 * Deliberately NOT a transform (scale/rotate). With LV_USE_MATRIX off,
 * any transformed object forces LVGL to render it through a separate
 * layer whose buffer (4-7KB, contiguous) comes from the 32/64KB LVGL
 * pool on EVERY frame. Once the pool is fragmented that allocation fails
 * and, with LV_USE_OS=0, lv_refr's draw_buf_flush() spins forever in
 * lv_draw_dispatch_wait_for_request() - the display thread never returns
 * and the last frame stays on the LCD. A text-opacity pulse is drawn
 * in-place with no layer, so this failure mode cannot occur. */
static void layer_pulse_anim_cb(void *var, int32_t value) {
    /* value goes 0 -> 100 -> 0: dip the text to ~50% and back */
    lv_obj_t *label = (lv_obj_t *)var;
    lv_opa_t opa = (lv_opa_t)(LV_OPA_COVER - (value * 128) / 100);
    lv_obj_set_style_text_opa(label, opa, 0);
}

/* Helper to create layer list widgets */
static void create_layer_list_widgets(lv_obj_t *parent, int y_offset) {
    int num_layers = ds_max_layers;
    int spacing = 25;
    int label_width = 22;
    int start_x = 140 - ((num_layers - 1) * spacing / 2) - (label_width / 2);

    for (int i = 0; i < num_layers && i < 10; i++) {
        layer_labels[i] = lv_label_create(parent);
        lv_obj_set_style_text_font(layer_labels[i], &lv_font_montserrat_28, 0);
        lv_obj_set_width(layer_labels[i], label_width);
        lv_obj_set_style_text_align(layer_labels[i], LV_TEXT_ALIGN_CENTER, 0);
        /* Enable transform for pulse animation */
        lv_obj_set_style_transform_pivot_x(layer_labels[i], label_width / 2, 0);
        lv_obj_set_style_transform_pivot_y(layer_labels[i], 14, 0);  /* Half of font height */

        char text[4];
        snprintf(text, sizeof(text), "%d", i);
        lv_label_set_text(layer_labels[i], text);

        if (i == active_layer) {
            lv_obj_set_style_text_color(layer_labels[i], get_layer_color(i), 0);
            lv_obj_set_style_text_opa(layer_labels[i], LV_OPA_COVER, 0);
        } else {
            lv_obj_set_style_text_color(layer_labels[i], lv_color_make(40, 40, 40), 0);
            lv_obj_set_style_text_opa(layer_labels[i], LV_OPA_30, 0);
        }
        lv_obj_set_pos(layer_labels[i], start_x + (i * spacing), y_offset);
    }
}

// /* Helper to destroy layer list widgets */
// static void destroy_layer_list_widgets(void) {
//     for (int i = 0; i < 10; i++) {
//         if (layer_labels[i]) {
//             lv_obj_del(layer_labels[i]);
//             layer_labels[i] = NULL;
//         }
//     }
// }

/* Helper to create over-max label widget */
static void create_over_max_widget(lv_obj_t *parent, int layer, int y_offset) {
    layer_over_max_label = lv_label_create(parent);
    lv_obj_set_style_text_font(layer_over_max_label, &lv_font_montserrat_28, 0);
    lv_obj_set_style_text_color(layer_over_max_label, get_layer_color(layer % 10), 0);
    lv_obj_set_style_text_align(layer_over_max_label, LV_TEXT_ALIGN_CENTER, 0);
    /* Enable transform for animations */
    lv_obj_set_style_transform_pivot_x(layer_over_max_label, 30, 0);
    lv_obj_set_style_transform_pivot_y(layer_over_max_label, 14, 0);

    char text[8];
    snprintf(text, sizeof(text), "%d", layer);
    lv_label_set_text(layer_over_max_label, text);
    lv_obj_align(layer_over_max_label, LV_ALIGN_TOP_MID, 0, y_offset);
}

// /* Helper to destroy over-max widget */
// static void destroy_over_max_widget(void) {
//     if (layer_over_max_label) {
//         lv_obj_del(layer_over_max_label);
//         layer_over_max_label = NULL;
//     }
// }

/* Callback to delete object after slide-out animation completes */
static void slide_out_ready_cb(lv_anim_t *anim) {
    lv_obj_t *obj = (lv_obj_t *)anim->var;
    if (obj) {
        lv_obj_del(obj);
    }
}

/* Start horizontal slide-in animation
 * from_right: true = slide from right to left, false = slide from left to right */
static void start_slide_in_x_anim(lv_obj_t *obj, bool from_right) {
    lv_anim_t anim;
    lv_anim_init(&anim);
    lv_anim_set_var(&anim, obj);
    lv_anim_set_exec_cb(&anim, layer_slide_x_anim_cb);
    int start_x = from_right ? 40 : -40;  /* Start 40px to the side */
    lv_anim_set_values(&anim, start_x, 0);  /* Animate to center (x_offset=0) */
    lv_anim_set_time(&anim, 150);  /* 150ms */
    lv_anim_set_path_cb(&anim, lv_anim_path_ease_out);
    lv_anim_start(&anim);
}

/* Start horizontal slide-out animation with auto-delete
 * to_left: true = slide to left, false = slide to right */
static void start_slide_out_x_anim(lv_obj_t *obj, bool to_left) {
    lv_anim_t anim;
    lv_anim_init(&anim);
    lv_anim_set_var(&anim, obj);
    lv_anim_set_exec_cb(&anim, layer_slide_x_anim_cb);
    int end_x = to_left ? -40 : 40;  /* End 40px to the side */
    lv_anim_set_values(&anim, 0, end_x);  /* From center to side */
    lv_anim_set_time(&anim, 80);  /* 80ms - fast disappear */
    lv_anim_set_path_cb(&anim, lv_anim_path_ease_in);
    lv_anim_set_ready_cb(&anim, slide_out_ready_cb);
    lv_anim_start(&anim);
}

/* Start pulse animation on active layer */
static void start_pulse_anim(lv_obj_t *obj) {
    lv_anim_t anim;
    lv_anim_init(&anim);
    lv_anim_set_var(&anim, obj);
    lv_anim_set_exec_cb(&anim, layer_pulse_anim_cb);
    lv_anim_set_values(&anim, 0, 100);  /* opacity dip 0% -> 100% of effect */
    lv_anim_set_time(&anim, 100);  /* 100ms expand */
    lv_anim_set_playback_time(&anim, 100);  /* 100ms shrink back */
    lv_anim_set_path_cb(&anim, lv_anim_path_ease_in_out);
    lv_anim_start(&anim);
}

/* Slide in layer list labels from left */
static void start_layer_list_slide_in(void) {
    int num_layers = ds_max_layers;
    int spacing = 25;
    int label_width = 22;
    int start_x = 140 - ((num_layers - 1) * spacing / 2) - (label_width / 2);
    int slide_offset = 50;  /* Slide from 50px to the left */

    for (int i = 0; i < num_layers && i < 10 && layer_labels[i]; i++) {
        int target_x = start_x + (i * spacing);
        lv_anim_t anim;
        lv_anim_init(&anim);
        lv_anim_set_var(&anim, layer_labels[i]);
        lv_anim_set_exec_cb(&anim, layer_pos_x_anim_cb);
        lv_anim_set_values(&anim, target_x - slide_offset, target_x);
        lv_anim_set_time(&anim, 150);
        lv_anim_set_path_cb(&anim, lv_anim_path_ease_out);
        lv_anim_start(&anim);
    }
}

/* ========== Slide Mode Layer Display Functions ========== */

/* Slide mode layout with gradient fade:
 * 7 visible slots: [0] [1] [2] [3] [4] [5] [6]
 * - Slot 0,6: small (16pt, very dim) - fade out edges
 * - Slot 1-5: large (28pt, bright) - center zone (5 slots)
 *
 * Active layer stays in large zone (1-5), scrolls only when needed.
 * Negative window_start allowed - negative slots shown as empty.
 */

#define SLIDE_LARGE_ZONE_START 2  /* Index where large zone begins (slots 2,3,4,5,6) */
#define SLIDE_LARGE_ZONE_END 6    /* Index where large zone ends (inclusive) - 5 large slots */

/* Get font for slot based on gradient position:
 * Pattern: 小中大大大大大中小 (9 slots: 0-8)
 * Slot 0,8: Small (16pt) - edge
 * Slot 1,7: Medium (20pt) - transition
 * Slot 2,3,4,5,6: Large (28pt) - center (5 large slots)
 */
static const lv_font_t* get_slide_slot_font(int slot) {
    if (slot == 0 || slot == 8) {
        return &lv_font_montserrat_16;  /* Edge - small */
    } else if (slot == 1 || slot == 7) {
        return &lv_font_montserrat_20;  /* Transition - medium */
    }
    return &lv_font_montserrat_28;      /* Center - large (slots 2,3,4,5,6) */
}

/* Get opacity for slot based on gradient position (inactive):
 * Edge: very dim, Medium: dim, Large: visible
 */
static lv_opa_t get_slide_slot_opa(int slot) {
    if (slot == 0 || slot == 8) {
        return LV_OPA_20;  /* Edge - very dim (darker) */
    } else if (slot == 1 || slot == 7) {
        return LV_OPA_40;  /* Transition - dim (darker) */
    }
    return LV_OPA_70;      /* Center - visible */
}

/* Get Y adjustment for vertical alignment based on font size */
static int get_slide_slot_y_adj(int slot) {
    if (slot == 0 || slot == 8) {
        return 6;   /* Small font needs more Y adjustment */
    } else if (slot == 1 || slot == 7) {
        return 4;   /* Medium font needs some Y adjustment */
    }
    return 0;       /* Large font - no adjustment */
}

/* Get X offset to move edge slots slightly inward */
static int get_slide_slot_x_offset(int slot) {
    if (slot == 0) {
        return 4;   /* Move left edge slot inward (right) */
    } else if (slot == 8) {
        return -4;  /* Move right edge slot inward (left) */
    }
    return 0;
}

/* Slide mode layout constants - using fixed width labels like fixed mode */
#define SLIDE_SLOT_SPACING 34       /* Uniform spacing between slots (wider for 2-digit) */
#define SLIDE_LABEL_WIDTH_SMALL 22  /* Width for edge slots (small font) */
#define SLIDE_LABEL_WIDTH_MEDIUM 28 /* Width for transition slots (medium font) */
#define SLIDE_LABEL_WIDTH_LARGE 34  /* Width for center slots (large font) */

/* Get label width for slot based on font size */
static int get_slide_label_width(int slot) {
    if (slot == 0 || slot == 8) {
        return SLIDE_LABEL_WIDTH_SMALL;
    } else if (slot == 1 || slot == 7) {
        return SLIDE_LABEL_WIDTH_MEDIUM;
    }
    return SLIDE_LABEL_WIDTH_LARGE;
}

/* Create slide mode layer widgets */
static void create_layer_slide_widgets(lv_obj_t *parent, int y_offset) {
    /* Calculate window start to keep active_layer in large zone
     * Prefer left side of large zone so large zone stays visually centered */
    layer_slide_window_start = active_layer - SLIDE_LARGE_ZONE_START;
    /* DON'T clamp to 0 - allow negative values, empty slots for negative layers */

    /* Calculate start_x to center all slots */
    int total_width = (SLIDE_VISIBLE_COUNT - 1) * SLIDE_SLOT_SPACING;
    int start_x = 140 - (total_width / 2);

    for (int i = 0; i < SLIDE_VISIBLE_COUNT; i++) {
        int layer_num = layer_slide_window_start + i;
        bool is_active = (layer_num == active_layer && layer_num >= 0);
        int label_width = get_slide_label_width(i);

        layer_slide_labels[i] = lv_label_create(parent);

        /* Font size based on gradient position */
        lv_obj_set_style_text_font(layer_slide_labels[i], get_slide_slot_font(i), 0);

        /* Fixed width and center alignment for uniform spacing */
        lv_obj_set_width(layer_slide_labels[i], label_width);
        lv_obj_set_style_text_align(layer_slide_labels[i], LV_TEXT_ALIGN_CENTER, 0);
        lv_label_set_long_mode(layer_slide_labels[i], LV_LABEL_LONG_CLIP);  /* Prevent text wrapping */

        /* Text color and opacity based on active state and gradient */
        if (layer_num < 0) {
            /* Negative layer number = empty/invisible */
            lv_obj_set_style_text_opa(layer_slide_labels[i], LV_OPA_TRANSP, 0);
        } else if (is_active) {
            /* Active layer = Hue-based color, full opacity */
            lv_obj_set_style_text_color(layer_slide_labels[i],
                get_slide_layer_color(layer_num, ds_layer_slide_max), 0);
            lv_obj_set_style_text_opa(layer_slide_labels[i], LV_OPA_COVER, 0);
        } else {
            /* Inactive = gray with gradient opacity */
            lv_obj_set_style_text_color(layer_slide_labels[i], lv_color_make(80, 80, 80), 0);
            lv_obj_set_style_text_opa(layer_slide_labels[i], get_slide_slot_opa(i), 0);
        }

        /* Set label text (layer number or empty for negative) */
        char text[12];
        if (layer_num >= 0) {
            snprintf(text, sizeof(text), "%d", layer_num);
        } else {
            text[0] = '\0';  /* Empty for negative */
        }
        lv_label_set_text(layer_slide_labels[i], text);

        /* Position using uniform spacing (center each label on its slot) */
        int y_adj = get_slide_slot_y_adj(i);
        int x_offset = get_slide_slot_x_offset(i);
        int x_pos = start_x + (i * SLIDE_SLOT_SPACING) - (label_width / 2) + x_offset;
        lv_obj_set_pos(layer_slide_labels[i], x_pos, y_offset + y_adj);

        /* Enable transform for animations */
        lv_obj_set_style_transform_pivot_x(layer_slide_labels[i], label_width / 2, 0);
        lv_obj_set_style_transform_pivot_y(layer_slide_labels[i], 14, 0);
    }

    LOG_INF("Slide mode widgets created: window_start=%d, active=%d", layer_slide_window_start, active_layer);
}

/* Destroy slide mode layer widgets */
// static void destroy_layer_slide_widgets(void) {
//     for (int i = 0; i < SLIDE_VISIBLE_COUNT; i++) {
//         if (layer_slide_labels[i]) {
//             lv_anim_del(layer_slide_labels[i], NULL);  /* Cancel any running animations */
//             lv_obj_del(layer_slide_labels[i]);
//             layer_slide_labels[i] = NULL;
//         }
//     }
//     layer_slide_window_start = 0;
// }

/* Reset all label positions to ensure they stay in correct place */
static void slide_reset_positions(void) {
    int total_width = (SLIDE_VISIBLE_COUNT - 1) * SLIDE_SLOT_SPACING;
    int start_x = 140 - (total_width / 2);

    for (int i = 0; i < SLIDE_VISIBLE_COUNT; i++) {
        if (layer_slide_labels[i]) {
            int label_width = get_slide_label_width(i);
            int y_adj = get_slide_slot_y_adj(i);
            int x_offset = get_slide_slot_x_offset(i);
            int x_pos = start_x + (i * SLIDE_SLOT_SPACING) - (label_width / 2) + x_offset;
            lv_obj_set_pos(layer_slide_labels[i], x_pos, 105 + y_adj);
        }
    }
}

/* Animation callback for scroll complete */
static void slide_scroll_anim_cb(void *var, int32_t value) {
    lv_obj_t *obj = (lv_obj_t *)var;
    if (obj) {
        lv_obj_set_style_translate_x(obj, value, 0);
    }
}

/* Update slide mode display - called when layer changes */
static void update_layer_slide_display(int layer, bool animate) {
    /* Auto-expand max layer if needed */
    if (layer >= ds_layer_slide_max) {
        ds_layer_slide_max = layer + 1;
        LOG_INF("Slide max expanded to %d", ds_layer_slide_max);
    }

    /* Calculate which slot the active layer is currently in */
    int current_slot = layer - layer_slide_window_start;

    /* Check if we need to scroll */
    bool need_scroll = false;
    int new_window_start = layer_slide_window_start;

    if (current_slot < SLIDE_LARGE_ZONE_START) {
        /* Layer moving left of large zone - scroll left */
        new_window_start = layer - SLIDE_LARGE_ZONE_START;
        need_scroll = true;
    } else if (current_slot > SLIDE_LARGE_ZONE_END) {
        /* Layer moving right of large zone - scroll right */
        new_window_start = layer - SLIDE_LARGE_ZONE_END;
        need_scroll = true;
    }

    /* DON'T clamp to 0 - allow negative values for edge layers (0, 1, 2) */

    /* Calculate scroll amount (number of slots shifted) */
    int scroll_slots = 0;
    if (need_scroll) {
        scroll_slots = new_window_start - layer_slide_window_start;
        layer_slide_window_start = new_window_start;
    }

    /* Update all labels with correct content and styling */
    for (int i = 0; i < SLIDE_VISIBLE_COUNT; i++) {
        if (!layer_slide_labels[i]) continue;

        int layer_num = layer_slide_window_start + i;
        bool is_active = (layer_num == layer && layer_num >= 0);

        /* Update text */
        char text[8];
        if (layer_num >= 0) {
            snprintf(text, sizeof(text), "%d", layer_num);
        } else {
            text[0] = '\0';  /* Empty for negative */
        }
        lv_label_set_text(layer_slide_labels[i], text);

        /* Update styling with gradient */
        if (layer_num < 0) {
            /* Negative layer = invisible */
            lv_obj_set_style_text_opa(layer_slide_labels[i], LV_OPA_TRANSP, 0);
        } else if (is_active) {
            /* Active layer = Hue-based color, full opacity */
            lv_obj_set_style_text_color(layer_slide_labels[i],
                get_slide_layer_color(layer_num, ds_layer_slide_max), 0);
            lv_obj_set_style_text_opa(layer_slide_labels[i], LV_OPA_COVER, 0);

            /* Pulse animation on active layer change */
            if (animate) {
                start_pulse_anim(layer_slide_labels[i]);
            }
        } else {
            /* Inactive = gray with gradient opacity based on slot position */
            lv_obj_set_style_text_color(layer_slide_labels[i], lv_color_make(80, 80, 80), 0);
            lv_obj_set_style_text_opa(layer_slide_labels[i], get_slide_slot_opa(i), 0);
        }
    }

    /* Set correct positions for all labels */
    slide_reset_positions();

    /* Apply scroll animation if scrolling occurred */
    if (need_scroll && animate && scroll_slots != 0) {
        /* Calculate scroll offset in pixels */
        int scroll_offset = scroll_slots * SLIDE_SLOT_SPACING;

        /* Animate all labels from offset position to final position */
        for (int i = 0; i < SLIDE_VISIBLE_COUNT; i++) {
            if (!layer_slide_labels[i]) continue;

            /* Cancel any existing translate animation */
            lv_anim_del(layer_slide_labels[i], slide_scroll_anim_cb);

            /* Start from offset position and animate to 0 (final position) */
            lv_anim_t anim;
            lv_anim_init(&anim);
            lv_anim_set_var(&anim, layer_slide_labels[i]);
            lv_anim_set_exec_cb(&anim, slide_scroll_anim_cb);
            lv_anim_set_values(&anim, scroll_offset, 0);
            lv_anim_set_time(&anim, 150);
            lv_anim_set_path_cb(&anim, lv_anim_path_ease_out);
            lv_anim_start(&anim);
        }
    }

    LOG_DBG("Slide update: layer=%d, window_start=%d, slot=%d, scroll=%d",
            layer, layer_slide_window_start, layer - layer_slide_window_start, scroll_slots);
}

void display_update_layer(int layer) {
    if (layer < 0 || layer > 255) return;

    int prev_layer = active_layer;
    active_layer = layer;  /* Always cache the value */

    /* Only update UI when on main screen */
    if (current_screen != SCREEN_MAIN) {
        return;
    }

    /* ========== Slide Mode ========== */
    if (ds_layer_slide_mode) {
        /* Slide mode: just update the slide display, it handles everything */
        bool animate = (prev_layer != layer);
        update_layer_slide_display(layer, animate);
        last_active_layer = layer;
        return;
    }

    /* ========== Fixed Mode (original behavior) ========== */
    bool should_be_over_max = (layer >= ds_max_layers);
    int layer_y = 105;  /* Y position for layer widgets */

    /* Determine slide direction based on layer change */
    bool going_up = (layer > prev_layer);  /* Layer increasing = slide from right */

    /* Mode transition: normal -> over-max */
    if (should_be_over_max && !layer_mode_over_max) {
        layer_mode_over_max = true;

        /* Slide-out layer list to left (animation callback will delete) */
        for (int i = 0; i < 10; i++) {
            if (layer_labels[i]) {
                start_slide_out_x_anim(layer_labels[i], true);  /* to_left = true */
                layer_labels[i] = NULL;
            }
        }

        /* Create over-max widget with slide-in from right */
        if (screen_obj) {
            create_over_max_widget(screen_obj, layer, layer_y);
            start_slide_in_x_anim(layer_over_max_label, true);  /* from_right = true */
        }
    }
    /* Mode transition: over-max -> normal */
    else if (!should_be_over_max && layer_mode_over_max) {
        layer_mode_over_max = false;

        /* Slide-out over-max widget to right (animation callback will delete) */
        if (layer_over_max_label) {
            start_slide_out_x_anim(layer_over_max_label, false);  /* to_left = false */
            layer_over_max_label = NULL;
        }

        /* Create layer list with slide-in from left */
        if (screen_obj) {
            create_layer_list_widgets(screen_obj, layer_y);
            start_layer_list_slide_in();
        }
    }
    /* Update within over-max mode */
    else if (layer_mode_over_max) {
        if (prev_layer != layer && screen_obj) {
            /* Slide out old number, slide in new number */
            if (layer_over_max_label) {
                /* Slide out: going_up = slide to left, going_down = slide to right */
                start_slide_out_x_anim(layer_over_max_label, going_up);
                layer_over_max_label = NULL;
            }

            /* Create new label and slide in from opposite direction */
            create_over_max_widget(screen_obj, layer, layer_y);
            start_slide_in_x_anim(layer_over_max_label, going_up);  /* from_right if going_up */
        }
    }
    else {
        /* Update normal layer list - just update colors, pulse on active */
        for (int i = 0; i < ds_max_layers && i < 10 && layer_labels[i]; i++) {
            if (i == active_layer) {
                lv_obj_set_style_text_color(layer_labels[i], get_layer_color(i), 0);
                lv_obj_set_style_text_opa(layer_labels[i], LV_OPA_COVER, 0);

                /* Pulse animation on active layer change */
                if (prev_layer != layer) {
                    start_pulse_anim(layer_labels[i]);
                }
            } else {
                lv_obj_set_style_text_color(layer_labels[i], lv_color_make(40, 40, 40), 0);
                lv_obj_set_style_text_opa(layer_labels[i], LV_OPA_30, 0);
            }
        }
    }

    last_active_layer = layer;
}

void display_update_wpm(int wpm) {
    wpm_value = wpm;  /* Cache for screen transitions */
    if (wpm_value_label) {
        snprintf(stbuf_wpm, sizeof(stbuf_wpm), "%d", wpm);
        lv_label_set_text_static(wpm_value_label, stbuf_wpm);
    }
}

void display_update_connection(bool usb_rdy, bool ble_conn, bool ble_bond, int profile) {
    usb_ready = usb_rdy;
    ble_connected = ble_conn;
    ble_bonded = ble_bond;
    ble_profile = profile;

    if (transport_label) {
        /* Exclusive display: USB or BLE (not both) */
        if (usb_ready) {
            /* USB connected - show USB only */
            snprintf(stbuf_transport, sizeof(stbuf_transport), "#ffffff USB#");
        } else {
            /* USB not connected - show BLE with profile number on new line
             * BLE text colors:
             * - Green (00ff00): Connected
             * - Blue (4A90E2): Bonded but not connected (registered profile)
             * - White (ffffff): Not bonded (empty profile)
             * Profile number: Always white
             */
            const char *ble_color;
            if (ble_conn) {
                ble_color = "00ff00";  /* Green - connected */
            } else if (ble_bond) {
                ble_color = "4A90E2";  /* Blue - bonded but not connected */
            } else {
                ble_color = "ffffff";  /* White - not bonded */
            }
            snprintf(stbuf_transport, sizeof(stbuf_transport),
                    "#%s BLE#\n#ffffff %d#", ble_color, profile);
        }
        lv_label_set_text_static(transport_label, stbuf_transport);
    }

    /* Hide profile label - now integrated into transport_label */
    if (ble_profile_label) {
        lv_label_set_text_static(ble_profile_label, "");
    }
}

void display_update_modifiers(uint8_t mods) {
    cached_modifiers = mods;  /* Cache for screen transitions */
    if (modifier_label) {
        int pos = 0;
        stbuf_modifier[0] = '\0';

        /* Build NerdFont icon string - YADS style */
        if (mods & (ZMK_MOD_FLAG_LCTL | ZMK_MOD_FLAG_RCTL)) {
            pos += snprintf(stbuf_modifier + pos, sizeof(stbuf_modifier) - pos, "%s", mod_symbols[0]);
        }
        if (mods & (ZMK_MOD_FLAG_LSFT | ZMK_MOD_FLAG_RSFT)) {
            pos += snprintf(stbuf_modifier + pos, sizeof(stbuf_modifier) - pos, "%s", mod_symbols[1]);
        }
        if (mods & (ZMK_MOD_FLAG_LALT | ZMK_MOD_FLAG_RALT)) {
            pos += snprintf(stbuf_modifier + pos, sizeof(stbuf_modifier) - pos, "%s", mod_symbols[2]);
        }
        if (mods & (ZMK_MOD_FLAG_LGUI | ZMK_MOD_FLAG_RGUI)) {
            pos += snprintf(stbuf_modifier + pos, sizeof(stbuf_modifier) - pos, "%s", mod_symbols[3]);
        }

        /* Empty string when no modifiers active */
        lv_label_set_text_static(modifier_label, stbuf_modifier);
    }
}

/* Helper function to reposition battery widgets based on count */
static void reposition_battery_widgets(int count) {
    if (count < 1) count = 1;
    if (count > MAX_KB_BATTERIES) count = MAX_KB_BATTERIES;

    /* Select layout based on count */
    static const int16_t kb_x_offsets_1[] = {0, 0, 0, 0};
    static const int16_t kb_x_offsets_2[] = {-70, 70, 0, 0};
    static const int16_t kb_x_offsets_3[] = {-90, 0, 90, 0};
    static const int16_t kb_x_offsets_4[] = {-100, -35, 35, 100};

    const int16_t *x_offsets;
    int16_t bar_width;

    switch (count) {
    case 1:
        x_offsets = kb_x_offsets_1;
        bar_width = 165;  /* 1.5x of standard width */
        break;
    case 2:
        x_offsets = kb_x_offsets_2;
        bar_width = 110;
        break;
    case 3:
        x_offsets = kb_x_offsets_3;
        bar_width = 70;
        break;
    case 4:
    default:
        x_offsets = kb_x_offsets_4;
        bar_width = 52;
        break;
    }

    /* Get name labels based on count */
    const char **names = NULL;
    if (count == 2) names = battery_names_2;
    else if (count == 3) names = battery_names_3;
    else if (count == 4) names = battery_names_4;

    #define KB_BAR_Y_OFFSET_R    -33
    #define KB_PCT_Y_OFFSET_R    -42
    #define KB_NAME_X_OFFSET_R   0

    for (int i = 0; i < MAX_KB_BATTERIES; i++) {
        bool visible = (i < count);
        int16_t x_off = x_offsets[i];

        /* Reposition bar */
        if (kb_bat_bar[i]) {
            lv_obj_set_size(kb_bat_bar[i], bar_width, 4);
            lv_obj_align(kb_bat_bar[i], LV_ALIGN_BOTTOM_MID, x_off, KB_BAR_Y_OFFSET_R);
        }

        /* Reposition percentage label */
        if (kb_bat_pct[i]) {
            lv_obj_align(kb_bat_pct[i], LV_ALIGN_BOTTOM_MID, x_off, KB_PCT_Y_OFFSET_R);
        }

        /* Reposition and set name label */
        if (kb_bat_name[i]) {
            lv_obj_align(kb_bat_name[i], LV_ALIGN_BOTTOM_MID, x_off - bar_width/2 + KB_NAME_X_OFFSET_R, KB_PCT_Y_OFFSET_R);
            if (visible && names && names[i]) {
                lv_label_set_text(kb_bat_name[i], names[i]);
            } else {
                lv_label_set_text(kb_bat_name[i], "");
            }
        }

        /* Reposition nc bar */
        if (kb_bat_nc_bar[i]) {
            lv_obj_set_size(kb_bat_nc_bar[i], bar_width, 4);
            lv_obj_align(kb_bat_nc_bar[i], LV_ALIGN_BOTTOM_MID, x_off, KB_BAR_Y_OFFSET_R);
            /* Hide unused slots, but keep visible slots ready (visibility controlled by update function) */
            if (!visible) {
                lv_obj_set_style_opa(kb_bat_nc_bar[i], 0, 0);
            }
            lv_obj_invalidate(kb_bat_nc_bar[i]);  /* Force redraw */
        }

        /* Reposition nc label */
        if (kb_bat_nc_label[i]) {
            lv_obj_align(kb_bat_nc_label[i], LV_ALIGN_BOTTOM_MID, x_off, KB_PCT_Y_OFFSET_R);
            /* Hide unused slots */
            if (!visible) {
                lv_obj_set_style_opa(kb_bat_nc_label[i], 0, 0);
            }
            lv_obj_invalidate(kb_bat_nc_label[i]);  /* Force redraw */
        }

        /* Force redraw for bar, pct, name */
        if (kb_bat_bar[i]) lv_obj_invalidate(kb_bat_bar[i]);
        if (kb_bat_pct[i]) lv_obj_invalidate(kb_bat_pct[i]);
        if (kb_bat_name[i]) lv_obj_invalidate(kb_bat_name[i]);
    }

    LOG_INF("Battery widgets repositioned for count=%d", count);
}

void display_update_keyboard_battery_4(int bat0, int bat1, int bat2, int bat3) {
    int values[MAX_KB_BATTERIES] = {bat0, bat1, bat2, bat3};

    /* Count active batteries */
    int count = 0;
    for (int i = 0; i < MAX_KB_BATTERIES; i++) {
        battery_values[i] = values[i];
        if (values[i] > 0) count++;
    }

    /* If count changed, reposition widgets (including count=0 case) */
    if (count != active_battery_count) {
        active_battery_count = count;
        if (count > 0) {
            reposition_battery_widgets(count);
        }
        /* When count=0, hide all widgets below */
    }

    /* Update each battery slot */
    for (int i = 0; i < MAX_KB_BATTERIES; i++) {
        bool slot_visible = (count > 0 && i < count);
        int val = values[i];

        if (slot_visible && val > 0) {
            /* Connected: show bar and percentage, hide × */
            if (kb_bat_nc_bar[i]) lv_obj_set_style_opa(kb_bat_nc_bar[i], 0, 0);
            if (kb_bat_nc_label[i]) lv_obj_set_style_opa(kb_bat_nc_label[i], 0, 0);
            if (kb_bat_bar[i]) {
                lv_obj_set_style_opa(kb_bat_bar[i], 255, LV_PART_MAIN);
                lv_obj_set_style_opa(kb_bat_bar[i], 255, LV_PART_INDICATOR);
                lv_bar_set_value(kb_bat_bar[i], val, LV_ANIM_OFF);
                lv_obj_set_style_bg_color(kb_bat_bar[i], get_keyboard_battery_color(val), LV_PART_INDICATOR);
            }
            if (kb_bat_pct[i]) {
                lv_obj_set_style_opa(kb_bat_pct[i], 255, 0);
                snprintf(stbuf_kb_bat[i], sizeof(stbuf_kb_bat[i]), "%d", val);
                lv_label_set_text_static(kb_bat_pct[i], stbuf_kb_bat[i]);
                lv_obj_set_style_text_color(kb_bat_pct[i], get_keyboard_battery_color(val), 0);
            }
            if (kb_bat_name[i]) {
                lv_obj_set_style_opa(kb_bat_name[i], 255, 0);
            }
        } else if (slot_visible) {
            /* Disconnected: show ×, hide bar and percentage */
            if (kb_bat_bar[i]) {
                lv_obj_set_style_opa(kb_bat_bar[i], 0, LV_PART_MAIN);
                lv_obj_set_style_opa(kb_bat_bar[i], 0, LV_PART_INDICATOR);
            }
            if (kb_bat_pct[i]) lv_obj_set_style_opa(kb_bat_pct[i], 0, 0);
            if (kb_bat_name[i]) lv_obj_set_style_opa(kb_bat_name[i], 255, 0);
            if (kb_bat_nc_bar[i]) lv_obj_set_style_opa(kb_bat_nc_bar[i], 255, 0);
            if (kb_bat_nc_label[i]) lv_obj_set_style_opa(kb_bat_nc_label[i], 255, 0);
        } else {
            /* Slot not visible (beyond active count or count=0) - hide everything */
            if (kb_bat_bar[i]) {
                lv_obj_set_style_opa(kb_bat_bar[i], 0, LV_PART_MAIN);
                lv_obj_set_style_opa(kb_bat_bar[i], 0, LV_PART_INDICATOR);
            }
            if (kb_bat_pct[i]) lv_obj_set_style_opa(kb_bat_pct[i], 0, 0);
            if (kb_bat_name[i]) lv_obj_set_style_opa(kb_bat_name[i], 0, 0);
            if (kb_bat_nc_bar[i]) lv_obj_set_style_opa(kb_bat_nc_bar[i], 0, 0);
            if (kb_bat_nc_label[i]) lv_obj_set_style_opa(kb_bat_nc_label[i], 0, 0);
        }
    }
}

/* Legacy 2-battery interface for backward compatibility */
void display_update_keyboard_battery(int left, int right) {
    display_update_keyboard_battery_4(left, right, 0, 0);
}

void display_update_signal(int8_t rssi_val, float rate) {
    rssi = rssi_val;
    rate_hz = rate;

    uint8_t bars = rssi_to_bars(rssi_val);

    if (rssi_bar) {
        lv_bar_set_value(rssi_bar, bars, LV_ANIM_OFF);
        lv_obj_set_style_bg_color(rssi_bar, get_rssi_color(bars), LV_PART_INDICATOR);
    }

    if (rssi_label) {
        snprintf(stbuf_rssi, sizeof(stbuf_rssi), "%ddBm", rssi_val);
        lv_label_set_text_static(rssi_label, stbuf_rssi);
    }

    if (rate_label) {
        /* Robust rate display: handle invalid/out-of-range values */
        if (rate < 0.0f) {
            /* Negative = no data yet */
            snprintf(stbuf_rate, sizeof(stbuf_rate), "-.--Hz");
        } else if (rate > 999.9f || rate != rate) {  /* rate != rate checks for NaN */
            LOG_WRN("Invalid rate value: %.2f, displaying as -.--", (double)rate);
            snprintf(stbuf_rate, sizeof(stbuf_rate), "-.--Hz");
        } else {
            /* Safe conversion with rounding */
            int rate_int = (int)(rate * 10.0f + 0.5f);
            if (rate_int > 9999) rate_int = 9999;  /* Cap at 999.9Hz */
            snprintf(stbuf_rate, sizeof(stbuf_rate), "%d.%dHz", rate_int / 10, rate_int % 10);
        }
        lv_label_set_text_static(rate_label, stbuf_rate);
    }
}

/* ========== Screen Transition Functions ========== */

static void destroy_main_screen_widgets(void) {
    LOG_INF("Destroying main screen widgets...");

    /* Cancel any running layer animations BEFORE deleting objects */
    for (int i = 0; i < 10; i++) {
        if (layer_labels[i]) {
            lv_anim_del(layer_labels[i], NULL);  /* Cancel all animations on this object */
        }
    }
    if (layer_over_max_label) {
        lv_anim_del(layer_over_max_label, NULL);
    }
    /* Cancel slide mode animations */
    for (int i = 0; i < SLIDE_VISIBLE_COUNT; i++) {
        if (layer_slide_labels[i]) {
            lv_anim_del(layer_slide_labels[i], NULL);
        }
    }

    if (rate_label) { lv_obj_del(rate_label); rate_label = NULL; }
    if (rssi_label) { lv_obj_del(rssi_label); rssi_label = NULL; }
    if (rssi_bar) { lv_obj_del(rssi_bar); rssi_bar = NULL; }
    if (rx_title_label) { lv_obj_del(rx_title_label); rx_title_label = NULL; }
    if (channel_label) { lv_obj_del(channel_label); channel_label = NULL; }
    /* Keyboard battery - delete all 4 slots */
    for (int i = 0; i < MAX_KB_BATTERIES; i++) {
        if (kb_bat_nc_label[i]) { lv_obj_del(kb_bat_nc_label[i]); kb_bat_nc_label[i] = NULL; }
        if (kb_bat_nc_bar[i]) { lv_obj_del(kb_bat_nc_bar[i]); kb_bat_nc_bar[i] = NULL; }
        if (kb_bat_name[i]) { lv_obj_del(kb_bat_name[i]); kb_bat_name[i] = NULL; }
        if (kb_bat_pct[i]) { lv_obj_del(kb_bat_pct[i]); kb_bat_pct[i] = NULL; }
        if (kb_bat_bar[i]) { lv_obj_del(kb_bat_bar[i]); kb_bat_bar[i] = NULL; }
    }
    if (modifier_label) { lv_obj_del(modifier_label); modifier_label = NULL; }
    for (int i = 0; i < 10; i++) {
        if (layer_labels[i]) { lv_obj_del(layer_labels[i]); layer_labels[i] = NULL; }
    }
    if (layer_over_max_label) { lv_obj_del(layer_over_max_label); layer_over_max_label = NULL; }
    /* Delete slide mode widgets */
    for (int i = 0; i < SLIDE_VISIBLE_COUNT; i++) {
        if (layer_slide_labels[i]) { lv_obj_del(layer_slide_labels[i]); layer_slide_labels[i] = NULL; }
    }
    if (layer_title_label) { lv_obj_del(layer_title_label); layer_title_label = NULL; }
    if (ble_profile_label) { lv_obj_del(ble_profile_label); ble_profile_label = NULL; }
    if (transport_label) { lv_obj_del(transport_label); transport_label = NULL; }
    if (wpm_value_label) { lv_obj_del(wpm_value_label); wpm_value_label = NULL; }
    if (wpm_title_label) { lv_obj_del(wpm_title_label); wpm_title_label = NULL; }
    if (device_name_label) { lv_obj_del(device_name_label); device_name_label = NULL; }

    /* Reset state for proper reinitialization */
    layer_mode_over_max = false;
    active_battery_count = 0;  /* Force reposition on next update */

    LOG_INF("Main screen widgets destroyed");
}

static void create_main_screen_widgets(void) {
    if (!screen_obj) return;
    LOG_INF("Creating main screen widgets...");

    transport_label = lv_label_create(screen_obj);
    lv_obj_set_style_text_font(transport_label, &lv_font_montserrat_12, 0);
    lv_obj_set_style_text_color(transport_label, lv_color_white(), 0);
    lv_obj_set_style_text_align(transport_label, LV_TEXT_ALIGN_RIGHT, 0);
    lv_label_set_recolor(transport_label, true);
    lv_obj_align(transport_label, LV_ALIGN_TOP_RIGHT, -10, 53);
    lv_label_set_text(transport_label, "#ffffff BLE#\n#ffffff 0#");  /* Exclusive display */

    ble_profile_label = lv_label_create(screen_obj);
    lv_obj_set_style_text_font(ble_profile_label, &lv_font_montserrat_12, 0);
    lv_obj_set_style_text_color(ble_profile_label, lv_color_white(), 0);
    lv_label_set_text(ble_profile_label, "");  /* Hidden - integrated */
    lv_obj_align(ble_profile_label, LV_ALIGN_TOP_RIGHT, -8, 78);

    layer_title_label = lv_label_create(screen_obj);
    lv_obj_set_style_text_font(layer_title_label, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_color(layer_title_label, lv_color_make(160, 160, 160), 0);
    lv_label_set_text(layer_title_label, "Layer");
    lv_obj_align(layer_title_label, LV_ALIGN_TOP_MID, 0, 82);  /* 3px up */

    /* Create layer display - slide mode OR fixed mode (list/over-max) */
    if (ds_layer_slide_mode) {
        /* Slide mode: create 7-slot dial display */
        create_layer_slide_widgets(screen_obj, 105);
        layer_mode_over_max = false;  /* Not used in slide mode */
    } else if (active_layer >= ds_max_layers) {
        layer_mode_over_max = true;
        create_over_max_widget(screen_obj, active_layer, 105);
    } else {
        layer_mode_over_max = false;
        create_layer_list_widgets(screen_obj, 105);
    }

    modifier_label = lv_label_create(screen_obj);
    lv_obj_set_style_text_font(modifier_label, &NerdFonts_Regular_40, 0);
    lv_obj_set_style_text_color(modifier_label, lv_color_white(), 0);
    lv_obj_set_style_text_letter_space(modifier_label, 10, 0);  /* Space between icons */
    lv_label_set_text(modifier_label, "");
    lv_obj_align(modifier_label, LV_ALIGN_TOP_MID, 0, 145);

    /* === Keyboard battery widgets (4 slots, dynamic layout) === */
    static const int16_t kb_x_offsets_2_r[] = {-70, 70, 0, 0};
    int16_t bar_width_r = 110;  /* Default to 2-battery layout */

    for (int i = 0; i < MAX_KB_BATTERIES; i++) {
        int16_t x_offset_r = (i < 2) ? kb_x_offsets_2_r[i] : 0;

        /* Connected state bar */
        kb_bat_bar[i] = lv_bar_create(screen_obj);
        lv_obj_set_size(kb_bat_bar[i], bar_width_r, 4);
        lv_obj_align(kb_bat_bar[i], LV_ALIGN_BOTTOM_MID, x_offset_r, -33);
        lv_bar_set_range(kb_bat_bar[i], 0, 100);
        lv_bar_set_value(kb_bat_bar[i], 0, LV_ANIM_OFF);
        lv_obj_set_style_bg_color(kb_bat_bar[i], lv_color_hex(0x202020), LV_PART_MAIN);
        lv_obj_set_style_bg_opa(kb_bat_bar[i], 255, LV_PART_MAIN);
        lv_obj_set_style_radius(kb_bat_bar[i], 1, LV_PART_MAIN);
        lv_obj_set_style_bg_color(kb_bat_bar[i], lv_color_hex(0x909090), LV_PART_INDICATOR);
        lv_obj_set_style_bg_opa(kb_bat_bar[i], 255, LV_PART_INDICATOR);
        lv_obj_set_style_bg_grad_color(kb_bat_bar[i], lv_color_hex(0xf0f0f0), LV_PART_INDICATOR);
        lv_obj_set_style_bg_grad_dir(kb_bat_bar[i], LV_GRAD_DIR_HOR, LV_PART_INDICATOR);
        lv_obj_set_style_radius(kb_bat_bar[i], 1, LV_PART_INDICATOR);
        lv_obj_set_style_opa(kb_bat_bar[i], 0, LV_PART_MAIN);
        lv_obj_set_style_opa(kb_bat_bar[i], 0, LV_PART_INDICATOR);

        /* Percentage label */
        kb_bat_pct[i] = lv_label_create(screen_obj);
        lv_obj_set_style_text_font(kb_bat_pct[i], &lv_font_montserrat_12, 0);
        lv_obj_set_style_text_color(kb_bat_pct[i], lv_color_white(), 0);
        lv_obj_align(kb_bat_pct[i], LV_ALIGN_BOTTOM_MID, x_offset_r, -42);
        lv_label_set_text(kb_bat_pct[i], "0");
        lv_obj_set_style_opa(kb_bat_pct[i], 0, 0);

        /* Name label */
        kb_bat_name[i] = lv_label_create(screen_obj);
        lv_obj_set_style_text_font(kb_bat_name[i], &lv_font_montserrat_12, 0);
        lv_obj_set_style_text_color(kb_bat_name[i], lv_color_hex(0x808080), 0);
        lv_obj_align(kb_bat_name[i], LV_ALIGN_BOTTOM_MID, x_offset_r - bar_width_r/2, -42);
        lv_obj_set_style_text_align(kb_bat_name[i], LV_TEXT_ALIGN_RIGHT, 0);
        lv_label_set_text(kb_bat_name[i], "");
        lv_obj_set_style_opa(kb_bat_name[i], 0, 0);

        /* Disconnected state bar */
        kb_bat_nc_bar[i] = lv_obj_create(screen_obj);
        lv_obj_set_size(kb_bat_nc_bar[i], bar_width_r, 4);
        lv_obj_align(kb_bat_nc_bar[i], LV_ALIGN_BOTTOM_MID, x_offset_r, -33);
        lv_obj_set_style_bg_color(kb_bat_nc_bar[i], lv_color_hex(0x9e2121), LV_PART_MAIN);
        lv_obj_set_style_bg_opa(kb_bat_nc_bar[i], 255, LV_PART_MAIN);
        lv_obj_set_style_radius(kb_bat_nc_bar[i], 1, LV_PART_MAIN);
        lv_obj_set_style_border_width(kb_bat_nc_bar[i], 0, 0);
        lv_obj_set_style_pad_all(kb_bat_nc_bar[i], 0, 0);
        lv_obj_set_style_opa(kb_bat_nc_bar[i], (i < 2) ? 255 : 0, 0);

        /* Disconnected state label */
        kb_bat_nc_label[i] = lv_label_create(screen_obj);
        lv_obj_set_style_text_font(kb_bat_nc_label[i], &lv_font_montserrat_12, 0);
        lv_obj_set_style_text_color(kb_bat_nc_label[i], lv_color_hex(0xe63030), 0);
        lv_obj_align(kb_bat_nc_label[i], LV_ALIGN_BOTTOM_MID, x_offset_r, -42);
        lv_label_set_text(kb_bat_nc_label[i], LV_SYMBOL_CLOSE);
        lv_obj_set_style_opa(kb_bat_nc_label[i], (i < 2) ? 255 : 0, 0);
    }

    channel_label = lv_label_create(screen_obj);
    lv_obj_set_style_text_font(channel_label, &lv_font_montserrat_12, 0);
    lv_obj_set_style_text_color(channel_label, lv_color_make(0x80, 0x80, 0x80), 0);
    lv_label_set_text(channel_label, "Ch:0");
    lv_obj_set_pos(channel_label, 62, 219);  /* 5px down, 5px left */

    rx_title_label = lv_label_create(screen_obj);
    lv_obj_set_style_text_font(rx_title_label, &lv_font_montserrat_12, 0);
    lv_obj_set_style_text_color(rx_title_label, lv_color_make(0x80, 0x80, 0x80), 0);
    lv_label_set_text(rx_title_label, "RX:");
    lv_obj_set_pos(rx_title_label, 102, 219);  /* 5px down, 5px left */

    rssi_bar = lv_bar_create(screen_obj);
    lv_obj_set_size(rssi_bar, 30, 8);
    lv_obj_set_pos(rssi_bar, 130, 223);  /* RX indicator position */
    lv_bar_set_range(rssi_bar, 0, 5);
    lv_bar_set_value(rssi_bar, 0, LV_ANIM_OFF);
    lv_obj_set_style_bg_color(rssi_bar, lv_color_hex(0x202020), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(rssi_bar, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_bg_color(rssi_bar, get_rssi_color(0), LV_PART_INDICATOR);
    lv_obj_set_style_bg_opa(rssi_bar, LV_OPA_COVER, LV_PART_INDICATOR);
    lv_obj_set_style_radius(rssi_bar, 2, LV_PART_MAIN);
    lv_obj_set_style_radius(rssi_bar, 2, LV_PART_INDICATOR);

    rssi_label = lv_label_create(screen_obj);
    lv_obj_set_style_text_font(rssi_label, &lv_font_montserrat_12, 0);
    lv_obj_set_style_text_color(rssi_label, lv_color_make(0xA0, 0xA0, 0xA0), 0);
    lv_label_set_text(rssi_label, "--dBm");
    lv_obj_set_pos(rssi_label, 167, 219);  /* 5px down, 5px left */

    rate_label = lv_label_create(screen_obj);
    lv_obj_set_style_text_font(rate_label, &lv_font_montserrat_12, 0);
    lv_obj_set_style_text_color(rate_label, lv_color_make(0xA0, 0xA0, 0xA0), 0);
    lv_label_set_text(rate_label, "-.--Hz");
    lv_obj_set_pos(rate_label, 222, 219);  /* 5px down, 5px left */

    LOG_INF("Main screen widgets created, restoring cached values...");

    /* Restore all cached values to newly created widgets */
    display_update_device_name(cached_device_name);
    display_update_wpm(wpm_value);
    display_update_connection(usb_ready, ble_connected, ble_bonded, ble_profile);
    display_update_layer(active_layer);
    display_update_modifiers(cached_modifiers);

    /* Force battery widget reposition based on cached values */
    /* Count how many batteries have non-zero values */
    int cached_count = 0;
    for (int i = 0; i < MAX_KB_BATTERIES; i++) {
        if (battery_values[i] > 0) cached_count++;
    }
    if (cached_count > 0) {
        /* Force reposition with cached count */
        active_battery_count = cached_count;
        reposition_battery_widgets(cached_count);
        LOG_INF("Battery widgets repositioned for cached count=%d", cached_count);
    }
    /* Now update battery display with values */
    display_update_keyboard_battery_4(battery_values[0], battery_values[1], battery_values[2], battery_values[3]);

    display_update_signal(rssi, rate_hz);

    LOG_INF("Cached values restored");
}

/* ========== Prospector Display (Carrefinho-inspired layouts) ========== */

static void destroy_prospector_display_widgets(void) {
    prospector_display_active = false;
    prospector_layouts_destroy();
    LOG_INF("Prospector Display destroyed");
}

static void create_prospector_display_widgets(void) {
    LOG_INF("Creating Prospector Display...");

    /* Dark background */
    lv_obj_set_style_bg_color(screen_obj, lv_color_black(), 0);

    /* Initialize layout system on the screen object */
    prospector_layouts_init(screen_obj);

    /* Restore saved layout style from NVS */
    uint8_t saved_layout = display_settings_get_layout();
    if (saved_layout != (uint8_t)prospector_layouts_get_style()) {
        prospector_layouts_set_style((prospector_layout_t)saved_layout);
    }

    prospector_display_active = true;

    LOG_INF("Prospector Display created (%s)",
            prospector_layouts_get_name(prospector_layouts_get_style()));
}


/* ========== 日志页（第 6 页）的宿主钩子 ==========
 *
 * 日志页的实现整块搬到了 custom_status_screen_log.c（本文件太大了）：页面控件、
 * 文本缓冲、刷新、以及处理切页请求的定时器都在那边。
 *
 * 但"屏"和页号归本文件，切页必然要碰 current_screen / transition_in_progress /
 * 各页控件的销毁重建，所以由本文件实现下面这几个钩子给日志页调用，
 * 契约见 custom_status_screen_log.h。
 *
 * 没编进日志页时这一整段不参与编译。
 */
#if IS_ENABLED(CONFIG_PROSPECTOR_DISPLAY_LOG_PAGE)

/* 从日志页返回时回到哪一页（= 进日志页之前那一页） */
static enum screen_state log_page_prev_screen = SCREEN_MAIN;

/* 切页前清屏：控件由各自的 destroy_*_widgets() 收尾，这里只管屏自己 */
static void clear_screen_obj(void) {
    if (!screen_obj) {
        return;
    }

    lv_obj_clean(screen_obj);
    lv_obj_set_style_bg_color(screen_obj, lv_color_black(), 0);
    lv_obj_invalidate(screen_obj);
}

bool custom_status_screen_log_host_visible(void) {
    return current_screen == SCREEN_LOG;
}

bool custom_status_screen_log_host_can_enter(void) {
    /* 只有主屏 / Prospector Display 能进日志页 */
    return current_screen == SCREEN_MAIN || current_screen == SCREEN_PROSPECTOR_DISPLAY;
}

bool custom_status_screen_log_host_enter(void) {
    if (!custom_status_screen_log_host_can_enter() || transition_in_progress) {
        return false;
    }

    transition_in_progress = true;
    log_page_prev_screen = current_screen;

    if (current_screen == SCREEN_PROSPECTOR_DISPLAY) {
        destroy_prospector_display_widgets();
    } else {
        destroy_main_screen_widgets();
    }

    clear_screen_obj();
    custom_status_screen_log_create(screen_obj);
    current_screen = SCREEN_LOG;
    transition_in_progress = false;

    return true;
}

void custom_status_screen_log_host_leave(void) {
    transition_in_progress = true;

    custom_status_screen_log_destroy();
    clear_screen_obj();

    if (log_page_prev_screen == SCREEN_PROSPECTOR_DISPLAY) {
        create_prospector_display_widgets();
        current_screen = SCREEN_PROSPECTOR_DISPLAY; /* 该函数自己不设 current_screen */
    } else {
        /* create_main_screen_widgets() 内部会设 current_screen
         * （非 YADS 布局时它会直接落到 Prospector Display） */
        create_main_screen_widgets();
    }

    transition_in_progress = false;
}

/* 刷新与定时器回调（log_page_refresh / log_page_timer_cb）也跟着日志页搬到
 * custom_status_screen_log.c 了 —— 它们只碰这一页的控件和请求标志。 */

#endif /* CONFIG_PROSPECTOR_DISPLAY_LOG_PAGE */

