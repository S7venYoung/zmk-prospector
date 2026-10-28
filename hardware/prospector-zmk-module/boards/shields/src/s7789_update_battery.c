/*
 * s7789_update_battery.c - 电量相关（左右手电量缓存）
 *
 * s7789_update.c 只管"本机其它状态"，电量全在这里：那边只留两个 hook 调用点，
 * 字段怎么填、哪些槽位不用，都在本文件：
 *   ble_battery_fill_adv_data()：左右手 -> 26 字节结构的电量字段；
 *   ble_battery_fill_snapshot()：左右手 -> 显示快照。
 *
 * 数据来源：
 *   ble_battery_update() + ble_battery_left / ble_battery_right（本文件 static）：
 *   左右手电量缓存，由 app/src/battery_cb.c 刷新后推过来（模块不反向依赖 app
 *   的函数）。0 = 未连接 / 未知。
 *
 * 本机（dongle）自己不带电池，屏幕只显示左右手两格（L / R）：旧的"接收端自身
 * 电量"通道（本机电量 -> 屏幕右上角）已删除，不再有本机电量来源。
 *
 * 这里不碰 split / BLS：哪只手是哪只手（dongle 靠从机 BLS 上报的 Battery Identifier
 * 认手）由 app 侧判断好再推过来。
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "s7789_update.h"

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

/* ========== 左右手电量缓存（由 app/src/battery_cb.c 推送） ========== */

/* 0 = 未连接 / 未知，界面据此显示"未连接"。两个字节各自原子写入：
 * 极端情况下界面可能出现一帧新旧混合，下一次刷新（app 侧 1s 一次）即一致。 */
static volatile uint8_t ble_battery_left;  /* 左手电量（显示端 "L" 槽位） */
static volatile uint8_t ble_battery_right; /* 右手电量（显示端 "R" 槽位） */

void ble_battery_update(uint8_t left, uint8_t right) {
    ble_battery_left = left;
    ble_battery_right = right;
}

/* ========== 给 s7789_update.c 留的 hook（那边只调用，不碰电量逻辑） ========== */

/* 左右手电量 -> 26 字节结构的电量字段（s7789_update.c 的 ble_fill_adv_data()）：
 *   battery_level         <- 左手：显示端拉平成 bat[0]，屏幕上的 L
 *   peripheral_battery[0] <- 右手：显示端拉平成 bat[1]，屏幕上的 R
 * 旧协议里代表第 3、4 个键盘设备的 peripheral_battery[1] / [2] 本机用不到，
 * 这里不赋值：调用方已 memset，显示端按 0 = 无数据处理。
 * 本机（dongle）自己没电池、也只有左右手两台设备，界面上就是 L、R 两格。 */
void ble_battery_fill_adv_data(struct zmk_status_adv_data *d) {
    d->battery_level = ble_battery_left;
    d->peripheral_battery[0] = ble_battery_right;
}

/* 电量字段 -> 显示快照（s7789_update.c 的 ble_poll_local_state()）：
 *   bat[0] = 左手（屏幕 "L"），bat[1] = 右手（屏幕 "R"）；
 *   bat[2] / bat[3] 不用（调用方 memset 保持 0，界面按"没有第 3、4 台设备"处理）。 */
void ble_battery_fill_snapshot(struct pending_display_data *out) {
    out->bat[0] = ble_battery_left;
    out->bat[1] = ble_battery_right;
}
