/*
 * s7789_update.h - ST7789 状态屏的"本机"数据源接口
 *
 * 原来这些数据由模块的 scanner_core.c / status_scanner.c 通过被动扫描其它
 * 键盘的 BLE 广播提供，接口名统一为 scanner_*。当前固件不再使用 scanner，
 * 改由 s7789_update.c 直接读取本机 ZMK 状态，接口名统一改为 ble_*。
 *
 * 命名对应关系（旧 -> 新）：
 *   scanner_get_pending_update        -> ble_get_pending_update
 *   scanner_is_signal_pending         -> ble_is_signal_pending
 *   scanner_signal_rssi / _rate_x100  -> ble_signal_rssi / ble_signal_rate_x100
 *   scanner_get_kb_version            -> ble_get_kb_version
 *   scanner_get_selected_keyboard     -> ble_get_selected_keyboard
 *   scanner_set_selected_keyboard     -> ble_set_selected_keyboard
 *   zmk_status_scanner_copy_keyboard  -> ble_copy_keyboard_status
 *   scanner_msg_send_display_refresh  -> ble_msg_send_display_refresh
 *
 * 电量相关（左右手电量缓存）在 s7789_update_battery.c 里实现：本机（dongle）自身
 * 电量不再显示，也没有对应接口。
 */

#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* struct pending_display_data 的唯一定义处，禁止在本文件重复定义 */
#include <zmk/scanner_core.h>
/* struct zmk_status_adv_data（26 字节广告数据结构，hook 参数用） */
#include <zmk/status_advertisement.h>
/* struct zmk_keyboard_status（多键盘列表界面使用） */
#include <zmk/status_scanner.h>

/**
 * @brief 取一份待显示数据快照（仅显示线程 / LVGL 定时器调用）
 *
 * @param out 输出快照
 * @return true 表示有变化需要刷新显示
 */
bool ble_get_pending_update(struct pending_display_data *out);

/**
 * @brief 是否有新的信号值待显示（定义在 s7789_update.c）
 *
 * 有新值时第一次调用返回 true 并清零（取走即用），显示端据此只重绘一次。
 * 新值由 app/src/signal_cb.c 读到从机链路 RSSI 后调用 ble_signal_update() 推过来；
 * 还没连上从机 / 没有屏幕模块时恒为 false，信号栏保持初始显示。
 */
bool ble_is_signal_pending(void);

/**
 * @brief 更新信号栏数据（定义在 s7789_update.c，由 app/src/signal_cb.c 调用）
 *
 * 与电量同一个方向（app -> 模块）：模块只做缓存 + 通知，不反向 extern app 侧函数。
 *
 * @param rssi 已连接从机链路的接收信号强度（dBm，HCI Read_RSSI 读到的值）
 * @param rate_x100 速率 x100；负值表示没有来源（界面显示 "-.--Hz"）
 *
 * 调用上下文：系统工作队列，约 1s 一次。
 */
void ble_signal_update(int8_t rssi, int32_t rate_x100);

/**
 * @brief 更新本模块缓存的左右手电量（定义在 s7789_update_battery.c，由 app/src/battery_cb.c 调用）
 *
 * 显示端只认自己这份缓存：模块提供入口 + 缓存，不反向 extern app 侧的函数。
 * "哪只手是哪只手"（dongle 模式下靠从机 BLS 上报的 Battery Identifier 认手）
 * 由 app 侧判断好后再推过来。0 表示未连接 / 未知，界面据此显示"未连接"。
 *
 * 调用上下文：系统工作队列（app 侧定时刷新）；两个字节各自原子写入，
 * 极端情况下界面可能出现一帧新旧混合，下一次刷新（1s 内）即一致。
 *
 * @param left  左手电量（显示端 "L" 槽位）
 * @param right 右手电量（显示端 "R" 槽位）
 */
void ble_battery_update(uint8_t left, uint8_t right);

/**
 * @brief hook：左右手电量 -> 26 字节结构（s7789_update.c 的 ble_fill_adv_data() 调用）
 *
 * 定义在 s7789_update_battery.c；调用方传入的 d 已 memset。
 */
void ble_battery_fill_adv_data(struct zmk_status_adv_data *d);

/**
 * @brief hook：电量字段 -> 显示快照（s7789_update.c 的 ble_poll_local_state() 调用）
 *
 * 定义在 s7789_update_battery.c；调用方传入的 out 已 memset。
 */
void ble_battery_fill_snapshot(struct pending_display_data *out);

/**
 * @brief 取"键盘"（本机）固件版本信息
 *
 * 本机模式下返回的是本机固件版本与键盘名。
 */
bool ble_get_kb_version(uint8_t *major, uint8_t *minor, uint8_t *patch, bool *is_dev,
                        char *name, size_t name_len);

/**
 * @brief 取/设置当前选中的键盘槽位（本机模式只有槽位 0）
 */
int ble_get_selected_keyboard(void);
int ble_set_selected_keyboard(int index);

/**
 * @brief 复制键盘状态快照（本机模式只有槽位 0 有效）
 */
bool ble_copy_keyboard_status(int index, struct zmk_keyboard_status *out);

/**
 * @brief 请求下一次刷新立即执行（屏幕切换后调用）
 */
int ble_msg_send_display_refresh(void);

/* 信号强度 / 速率缓存：由 ble_signal_update() 写入（app/src/signal_cb.c 推送），
 * 显示端在 ble_is_signal_pending() 返回 true 时读这两个值。
 * 速率没有连接态来源，恒为负值（界面显示 "-.--Hz"）。 */
extern volatile int8_t ble_signal_rssi;
extern volatile int32_t ble_signal_rate_x100;

/* 左右手电量缓存改由 s7789_update_battery.c 内部 static 持有，外部只能通过
 * ble_battery_update()（写）和两个 hook（读）访问，0 = 未连接 / 未知 */
