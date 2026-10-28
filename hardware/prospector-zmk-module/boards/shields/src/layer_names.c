/*
 * Copyright (c) 2024 The ZMK Contributors
 * SPDX-License-Identifier: MIT
 *
 * Layer Names - 层名仓库实现（设计说明见 layer_names.h）
 *
 * NVS 键："layername/0" … "layername/<层数-1>"，每层一个含结尾 '\0' 的字符串。
 * 空字符串（1 字节）的语义是"这一层没有自定义名字"，显示时回落到 devicetree
 * 的 display-name —— 所以仓库为空时整机行为与改造前完全一致。
 */

#include "layer_names.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/settings/settings.h>
#include <zmk/keymap.h>

LOG_MODULE_REGISTER(layer_names, CONFIG_ZMK_LOG_LEVEL);

/* ble_msg_send_display_refresh() 定义在 s7789_update.c（本 shield 状态屏的数据源）。
 * 两个 shield 共用本文件，万一某个构建没有编译 s7789_update.c，就用 weak 声明跳过，
 * 与 s7789_update.c 里 scanner_get_runtime_channel 的写法保持一致。 */
extern int ble_msg_send_display_refresh(void) __attribute__((weak));

/* ========== 内部状态 ========== */

/* 层名仓库：'\0' 开头表示"这一层没有自定义名字" */
static char layer_names[ZMK_KEYMAP_LAYERS_LEN][PROSPECTOR_LAYER_NAME_MAX_LEN];

/* 待写回 NVS 的层（bit0 = 第 0 层）：只写改动过的层，少做无谓的 flash 写入 */
static uint32_t save_pending_mask;

static bool names_loaded;

/* ========== NVS 载入 ========== */

#if IS_ENABLED(CONFIG_SETTINGS)

static int layer_names_handle_set(const char *name, size_t len, settings_read_cb read_cb,
                                  void *cb_arg) {
    char *end = NULL;
    unsigned long index = strtoul(name, &end, 10);

    /* 只认 "layername/<数字>" 这种直接子键 */
    if (end == name || *end != '\0' || index >= ARRAY_SIZE(layer_names)) {
        return -ENOENT;
    }

    if (len == 0 || len > sizeof(layer_names[0])) {
        LOG_WRN("Layer name %lu: invalid stored length %u", index, (unsigned int)len);
        return -EINVAL;
    }

    int rc = read_cb(cb_arg, layer_names[index], len);
    if (rc < 0) {
        LOG_WRN("Layer name %lu: read failed (%d)", index, rc);
        return rc;
    }

    /* 存的时候含结尾 '\0'，这里兜底保证任何情况下都以 '\0' 收尾 */
    layer_names[index][sizeof(layer_names[0]) - 1] = '\0';
    LOG_INF("Loaded layer name %lu: \"%s\"", index, layer_names[index]);
    return 0;
}

SETTINGS_STATIC_HANDLER_DEFINE(prosp_layer_names, PROSPECTOR_LAYER_NAME_SETTINGS_PREFIX, NULL,
                               layer_names_handle_set, NULL, NULL);

#endif /* CONFIG_SETTINGS */

void prospector_layer_names_init(void) {
    if (names_loaded) {
        return;
    }

#if IS_ENABLED(CONFIG_SETTINGS)
    /* ZMK 的 main() 里 settings_load() 已经加载过所有 handler，这里再显式加载一次
     * 我们这个子树，保证显示初始化时名字一定就绪（与 display_settings_init() 同样的
     * 做法，避免依赖 init 顺序）。 */
    settings_load_subtree(PROSPECTOR_LAYER_NAME_SETTINGS_PREFIX);
#endif

    names_loaded = true;

    for (size_t i = 0; i < ARRAY_SIZE(layer_names); i++) {
        const char *name = prospector_layer_name_get((uint8_t)i);
        LOG_INF("Layer %u name: \"%s\" (custom=%d)", (unsigned int)i, (name != NULL) ? name : "-",
                prospector_layer_name_is_custom((uint8_t)i) ? 1 : 0);
    }
}

/* ========== 读取 ========== */

bool prospector_layer_name_is_custom(uint8_t layer_index) {
    return layer_index < ARRAY_SIZE(layer_names) && layer_names[layer_index][0] != '\0';
}

/*
 * 获取指定索引的层名,读取入口
 */
const char *prospector_layer_name_get(uint8_t layer_index) {
    if (layer_index >= ZMK_KEYMAP_LAYERS_LEN) {
        return NULL; /* keymap 里没有这一层，调用方自行退化成层号 */
    }

    //如果这一层有自定义名字（不是空）,就返回这个自定义名字
    if (layer_names[layer_index][0] != '\0') {
        return layer_names[layer_index];
    }

    /* 没有自定义名字 -> 用 devicetree 的 display-name 当出厂默认值。
     * zmk_keymap_layer_name() 收的是 layer id，这里传进来的是 layer index，
     * 交给 zmk_keymap_layer_index_to_id() 转换：该函数无条件提供，未开启
     * CONFIG_ZMK_KEYMAP_LAYER_REORDERING 时是恒等映射，开启后也能取对名字。 */
    zmk_keymap_layer_id_t id = zmk_keymap_layer_index_to_id(layer_index);
    if (id == ZMK_KEYMAP_LAYER_ID_INVAL) {
        return NULL;
    }

    return zmk_keymap_layer_name(id);
}

/* ========== 写入 ========== */

int prospector_layer_name_set(uint8_t layer_index, const char *name, size_t len) {
    if (layer_index >= ARRAY_SIZE(layer_names)) {
        return -EINVAL;
    }

    if (name == NULL || len == 0 || name[0] == '\0') {
        prospector_layer_name_clear(layer_index);
        return 0;
    }

    /* 调用方把结尾 '\0' 也算进 len 时按 strlen 处理 */
    if (name[len - 1] == '\0') {
        len--;
        if (len == 0) {
            prospector_layer_name_clear(layer_index);
            return 0;
        }
    }

    if (len >= PROSPECTOR_LAYER_NAME_MAX_LEN) {
        return -ENOSPC;
    }

    char buf[PROSPECTOR_LAYER_NAME_MAX_LEN];
    memcpy(buf, name, len);
    buf[len] = '\0';

    if (strcmp(layer_names[layer_index], buf) == 0) {
        return 0; /* 没有变化：不置脏，不产生 flash 写入 */
    }

    memcpy(layer_names[layer_index], buf, len + 1);
    save_pending_mask |= BIT(layer_index);
    LOG_INF("Layer %u name set to \"%s\" (pending save)", layer_index, buf);
    return 0;
}

void prospector_layer_name_clear(uint8_t layer_index) {
    if (layer_index >= ARRAY_SIZE(layer_names)) {
        return;
    }

    if (layer_names[layer_index][0] == '\0') {
        return; /* 本来就没有自定义名字 */
    }

    layer_names[layer_index][0] = '\0';
    save_pending_mask |= BIT(layer_index);
    LOG_INF("Layer %u name cleared (falls back to display-name)", layer_index);
}

void prospector_layer_names_reset(void) {
    for (size_t i = 0; i < ARRAY_SIZE(layer_names); i++) {
        prospector_layer_name_clear((uint8_t)i);
    }
    LOG_INF("All layer names reset to devicetree display-name");
}

static void do_save(void) {
    for (size_t i = 0; i < ARRAY_SIZE(layer_names); i++) {
        if ((save_pending_mask & BIT(i)) == 0) {
            continue;
        }

        /* "layername/<n>" + '\0' */
        char key[sizeof(PROSPECTOR_LAYER_NAME_SETTINGS_PREFIX) + 8];
        snprintf(key, sizeof(key), PROSPECTOR_LAYER_NAME_SETTINGS_PREFIX "/%u", (unsigned int)i);

        /* 连结尾 '\0' 一起存：空名字就是 1 字节，语义是"这一层没有自定义名字" */
        int rc = settings_save_one(key, layer_names[i], strlen(layer_names[i]) + 1);
        if (rc < 0) {
            LOG_WRN("Failed to save %s (%d)", key, rc);
        } else {
            save_pending_mask &= ~BIT(i);
        }
    }
}

void prospector_layer_names_save_if_dirty(void) {
    if (save_pending_mask == 0) {
        return;
    }

    do_save();
    LOG_INF("Layer names saved to NVS");
}


void prospector_layer_names_refresh_display(void) {
    /* 层名变了，但快照 memcmp 只看前面几个字节，所以这里显式请求一次全量重绘 */
    if (ble_msg_send_display_refresh != NULL) {
        ble_msg_send_display_refresh();
    }
}

void prospector_layer_names_commit(void) {
    prospector_layer_names_save_if_dirty();
    prospector_layer_names_refresh_display();
}

