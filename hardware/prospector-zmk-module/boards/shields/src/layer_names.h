/*
 * Copyright (c) 2024 The ZMK Contributors
 * SPDX-License-Identifier: MIT
 *
 * Layer Names - 可写的层名仓库（RAM + NVS 持久化）
 *
 * 为什么需要它
 * ------------
 * ZMK 自己那套"可改层名"的接口只在开启 ZMK Studio 的构建里存在：
 *   - app/src/keymap.c 的 zmk_keymap_set_layer_name() 在未开启
 *     CONFIG_ZMK_KEYMAP_SETTINGS_STORAGE 时编译成 -ENOTSUP 桩；
 *   - zmk_keymap_layer_names[] 是只读的 const char *[]，指向 devicetree 的
 *     display-name 字符串，既不能改也不能存。
 * 因此本仓库自己在模块侧维护一份"可写 + 落 NVS"的层名：
 *   - devicetree 的 display-name 仍然是"出厂默认值"：某层没有自定义名字时
 *     自动回落 zmk_keymap_layer_name()，所以仓库为空时显示行为与以前完全一致；
 *   - 改名链（UI -> set -> commit）与 ZMK Studio 无关，也不需要
 *     CONFIG_ZMK_STUDIO / REORDERING。
 *
 * NVS 键
 * ------
 *   "layername/0" … "layername/<层数-1>"   每层一个字符串（含结尾 '\0'）
 * 前缀独立于 display_settings.c 的 "prosp"：Zephyr 同一个 settings 前缀只能注册
 * 一个 handler，重复注册返回 -EEXIST（zephyr/subsys/settings/src/settings.c）。
 *
 * 调用上下文
 * ----------
 * 与 display_settings.c 一样不加锁，假定调用方串行化访问（当前都是显示线程上的
 * UI / 每 100ms 的 LVGL 定时器）。写入是 <=19 字节的 memcpy，读取返回的指针由
 * 调用方立即 snprintf 复制走（YADS2 的用法）。
 */

#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/*
 * 层名缓冲区大小（含结尾 '\0'），即单层最多 19 个可见字符。
 * 与 ZMK 的 CONFIG_ZMK_KEYMAP_LAYER_NAME_MAX_LEN 默认值（20）以及 YADS2 布局里
 * stbuf_layer_rows[row][24] 的容器宽度对齐；可以用
 * -DPROSPECTOR_LAYER_NAME_MAX_LEN=… 覆盖成更大的值。
 */
#ifndef PROSPECTOR_LAYER_NAME_MAX_LEN
#define PROSPECTOR_LAYER_NAME_MAX_LEN 20
#endif

/* NVS 键前缀（见文件头注释） */
#define PROSPECTOR_LAYER_NAME_SETTINGS_PREFIX "layername"

/**
 * @brief 从 NVS 载入层名（幂等，可重复调用）
 *
 * 应在显示初始化时调用一次（custom_status_screen.c 的 load_display_settings()），
 * 这样上电首屏渲染前名字就已经就绪。
 */
void prospector_layer_names_init(void);

/**
 * @brief 取某一层的最终显示名（层名仓库优先，回落到 DT display-name）
 *
 * @param layer_index keymap 里的层序号（0 起），即 zmk_keymap_layer_index_to_id()
 *                    收的那种"层 index"，不是 layer id
 * @return 可显示的字符串；越界或既没有自定义名又没有 display-name 时返回 NULL
 *         （调用方按上游 YADS 的规则退化成层号显示）
 */
const char *prospector_layer_name_get(uint8_t layer_index);

/**
 * @brief 该层是否有用户自定义名（false = 显示的是 DT display-name）
 */
bool prospector_layer_name_is_custom(uint8_t layer_index);

/**
 * @brief 设置某一层的名字（只改 RAM + 标脏，不做 flash 写入）
 *
 * @param layer_index 层序号
 * @param name        新名字
 * @param len         名字的可见长度（strlen(name)，不含结尾 '\0'；若把结尾 '\0'
 *                    一起算进来也能正确识别）
 * @return 0 成功（含"与当前值相同，未产生改动"）；-EINVAL 层序号越界；
 *         -ENOSPC 名字太长（超过 PROSPECTOR_LAYER_NAME_MAX_LEN - 1）
 *
 * name 传 NULL / 空串等同于 prospector_layer_name_clear()。
 * 改完记得调 prospector_layer_names_commit()（落 NVS + 刷新屏幕）。
 */
int prospector_layer_name_set(uint8_t layer_index, const char *name, size_t len);

/**
 * @brief 清除某一层的自定义名，显示回落到 DT display-name（只改 RAM + 标脏）
 */
void prospector_layer_name_clear(uint8_t layer_index);

/**
 * @brief 把改动过的层写回 NVS（无改动时是空操作）
 *
 * 与 display_settings_save_if_dirty() 同一套"脏标志 + 离开界面才落盘"的惯例，
 * 避免每敲一个字符就写一次 flash。
 */
void prospector_layer_names_save_if_dirty(void);

/**
 * @brief 清空所有自定义层名（全部回落到 DT display-name，需要 save/commit 才落盘）
 */
void prospector_layer_names_reset(void);

/**
 * @brief 请求状态屏重绘一次（层名变了但前几个字节没变时，快照 memcmp 检测不到）
 *
 * 内部调用 s7789_update.c 的 ble_msg_send_display_refresh()；若当前构建没有编译
 * s7789_update.c（weak 符号为空）则安静跳过。
 */
void prospector_layer_names_refresh_display(void);

/**
 * @brief 改名后的收尾动作：落 NVS + 请求重绘
 *
 * UI 改完名字调用这一个函数即可；改名过程中若不需要落盘（例如还在输入中），
 * 可以先只调 prospector_layer_names_refresh_display() 看效果。
 */
void prospector_layer_names_commit(void);
