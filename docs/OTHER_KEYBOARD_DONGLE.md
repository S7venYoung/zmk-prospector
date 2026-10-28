# 其他 ZMK 键盘项目如何接入 Dongle

本指南针对**同一块 kblade/ms2 显示接收器**连接其他 ZMK 键盘。
Dongle 主分支为 [`codex/kblade-dya-four-themes`](https://github.com/S7venYoung/zmk-prospector/tree/codex/kblade-dya-four-themes)。
扫描仪分支只接收广播，不能替代键盘的 split central；不要把扫描仪配置用于接收按键。

## 引用什么

可复用的是接收器工程、DYA 功能、本地硬件模块和主题。接收器同时承担该键盘的
完整 keymap 与 split central 角色，因此需要按照实际键盘重新构建固件。
现成 Corne UF2 对应 Corne 7 列、46 键起始布局，不能据此保证其他键数或布局正确。

```text
左手 peripheral ──┐
                   ├─ ZMK split ─ kblade Dongle（完整 keymap + 屏幕）─ USB/BLE ─ 电脑
右手 peripheral ──┘
```

如果已经有能工作的 ZMK Dongle 三件套，保留自己的左右手/接收器布局与键位映射，
再接入本项目的显示和 DYA 配置；这比从 Corne 文件改引脚更容易核对。
如果原项目只有左右手，其中一边承担 central，则先增加真正的 Dongle 变体，
并让两只手都以 peripheral 角色运行。

## 1. 对齐固件基线

完整 DYA 接收器采用本项目 [Dongle west.yml](https://github.com/S7venYoung/zmk-prospector/blob/codex/kblade-dya-four-themes/west.yml)
的固定依赖，ZMK 为 `cormoran/zmk@e5c9b6915b56801193e359dd9bad4a167ce0d1b8`。
普通 ZMK、DYA 核心和跨 Zephyr 版本的扩展不能假定可以混搭。
左右手至少需要与接收器的 split 协议兼容，不能只凭“都是 nRF52840/ZMK”判断兼容。
编码器、指点设备、Runtime Sensor Rotate 等还取决于外设设备树与上报支持。

两种工作区组织方式：

- 直接按 [Dongle GUIDE](https://github.com/S7venYoung/zmk-prospector/blob/codex/kblade-dya-four-themes/GUIDE.md)
  初始化本项目工作区，将自己的键盘配置仓库另行放入工作区或外部目录。
- 在自己的 west manifest 中增加本项目，并合并/固定相同的 ZMK、Zephyr、DYA 依赖。

只增加项目的片段：

```yaml
manifest:
  projects:
    - name: zmk-prospector
      url: https://github.com/S7venYoung/zmk-prospector
      revision: codex/kblade-dya-four-themes # 发布配置时固定到验证过的 commit
      path: modules/zmk-prospector
```

若希望从本项目导入依赖，可在该项目下加 `import: {file: west.yml}`。
务必处理自己 manifest 中已有的同名 `zmk`、`zephyr` 和扩展模块；上层项目声明可能覆盖
导入版本。仅有上面的片段不会自动把官方 ZMK 升级为 DYA 核心。

**不要再添加远程 `prospector-zmk-module` 项目。** 配套硬件已经位于
`modules/zmk-prospector/hardware/prospector-zmk-module/`。

## 2. 提供自己的键盘 Dongle Shield

以下 `my_keyboard` 只是占位名，需替换为自己项目的真实名称。
自己的配置仓库通常需要：

```text
my-zmk-config/
├── boards/shields/my_keyboard/
│   ├── Kconfig.shield
│   ├── Kconfig.defconfig
│   ├── my_keyboard_dongle.overlay
│   └── 现有的布局、transform、左右手定义……
└── config/
    ├── my_keyboard_dongle.keymap
    ├── my_keyboard_dongle.conf
    └── 左右手各自的配置……
```

`Kconfig.shield` 中注册接收器：

```kconfig
config SHIELD_MY_KEYBOARD_DONGLE
    def_bool $(shields_list_contains,my_keyboard_dongle)
```

`Kconfig.defconfig` 中声明默认角色：

```kconfig
if SHIELD_MY_KEYBOARD_DONGLE
config ZMK_KEYBOARD_NAME
    default "My Keyboard DYA"
config ZMK_SPLIT
    default y
config ZMK_SPLIT_ROLE_CENTRAL
    default y
endif
```

这些只是注册入口，不是一份完整通用 Shield。`my_keyboard_dongle.overlay` 必须提供
接收器需要的完整矩阵/physical layout/transform，和左右手上报位置保持一致。
接收器并没有两只手的物理矩阵，不要照搬半边键盘的 GPIO 扫描引脚到 kblade，
否则可能与显示屏、SPI、背光或电源控制冲突。

核对项目包括：总键数、左右偏移、矩阵转换、位置编号、physical layout，
以及有传感器时的数量、索引和方向。已有正常工作的 Dongle Shield 应优先复用。

## 3. 提供完整 keymap 与接收器配置

`my_keyboard_dongle.keymap` 保存两只手合起来的完整键位和全部层，使用自己的布局，
不要保留 Corne 的 46 个 bindings 再期待其他键数自动补齐。每层 bindings 必须与
接收器 physical layout 和上报位置匹配。需要 DYA 运行时宏等行为时，保留对应的
behaviors include；参考原始 [Corne keymap](https://github.com/S7venYoung/zmk-prospector/blob/codex/kblade-dya-four-themes/config/corne_dongle.keymap)。

可将 `config/corne_dongle.conf` 作为功能配置的起点，复制为自己的 Shield 配置名：

```sh
theme_repo="/absolute/path/to/zmk-prospector"
keyboard_repo="/absolute/path/to/my-zmk-config"
cp "$theme_repo/config/corne_dongle.conf" \
   "$keyboard_repo/config/my_keyboard_dongle.conf"
```

这里的变量需先设置为真实路径。核对键盘名称、USB 产品名、外设数量和实际硬件功能。
主要角色与显示配置包括：

```conf
CONFIG_ZMK_SPLIT=y
CONFIG_ZMK_SPLIT_ROLE_CENTRAL=y
CONFIG_ZMK_SPLIT_BLE_CENTRAL_PERIPHERALS=2
CONFIG_ZMK_SPLIT_BLE_CENTRAL_BATTERY_LEVEL_FETCHING=y
CONFIG_ZMK_USB=y
CONFIG_ZMK_BLE=y
CONFIG_ZMK_DISPLAY=y
CONFIG_ZMK_DISPLAY_STATUS_SCREEN_CUSTOM=y
CONFIG_ZMK_PROSPECTOR_SCANNER_ORIGINAL=y
CONFIG_ZMK_PROSPECTOR_DYA_SETTINGS=y
```

这段只展示关键项。完整 Studio、Custom Settings 和 RPC 配置应以本项目配置为准，
不能只粘贴这几行就认为完整 DYA 已启用。设备不存在时，不要添加虚构的传感器节点。

## 4. 用自己的 Shield 构建 kblade 接收器

在已经完成 `west update` 的兼容工作区中，设置三个真实绝对路径：

```sh
workspace_dir="/absolute/path/to/west-workspace"
theme_repo="$workspace_dir/modules/zmk-prospector"
keyboard_repo="/absolute/path/to/my-zmk-config"

west build -s "$workspace_dir/zmk/app" -d "$workspace_dir/build-my-keyboard-dongle" \
  -b kblade//zmk -S studio-rpc-usb-uart -S zmk-usb-logging -- \
  -DSHIELD="prospector_ms2 my_keyboard_dongle" \
  -DBOARD_ROOT="$keyboard_repo" \
  -DZMK_CONFIG="$keyboard_repo/config" \
  -DZMK_EXTRA_MODULES="$theme_repo/hardware/prospector-zmk-module;$theme_repo"
```

若按本仓库独立工作区初始化，`theme_repo` 通常是 `$workspace_dir/zmk-prospector`，
而不是 `modules/zmk-prospector`，请相应修改。已有其他模块时加入同一分号列表。
本项目通过模块元数据额外注册本地 kblade board；`BOARD_ROOT` 则让构建找到自己的 Shield。

输出为 `build-my-keyboard-dongle/zephyr/zmk.uf2`，只用于这块 kblade 接收器。
左右手分别按各自主控和 peripheral Shield 构建，不能给三块设备刷同一个接收器 UF2。
本项目的 `build-ms2-dya.sh` 和默认 Actions 仍指定 `corne_dongle`；其他键盘应使用
上面的独立构建命令，或在自己工作流中替换为真实 Shield/配置目录。

## 5. 输入与显示验收

按顺序核对，保留现有绑定：

1. 接收器通过 USB 工作；左右手都能连接它。
2. 所有键位与位置正确，左右手边界、层和传感器映射正确。
3. DYA 读取到完整 keymap，改键与 Save/重启恢复正确。
4. 修饰键、USB/BLE 输出状态、WPM 与实际键盘行为一致。
5. 左右电量来源正确；有需要再交换 `left_hand_slot/right_hand_slot`。
6. 选择九个主题，核对长层名、三位数 WPM、未知电量及 MONO 曲线。

换主题不需要清空蓝牙配对。是否需要更新绑定，要根据真实连接状态决定，不能把刷
settings reset 当作引用主题的固定步骤。

## 只要主题，不要完整 Dongle 功能

参见 [GUIDE 的主题引用章节](../GUIDE.md#主题引用)。复制完整 `display-themes/` 并保留
许可证、字体来源；它仍需要兼容的 ms2 数据/字体接口与 LVGL 9。其他屏幕和主板需要
适配，不能仅增加一个 west 项目就保证有相同 UI。

## 已验证范围

本指南给出的是接入方法，不代表任意键盘已经验证。当前工程的主题主机检查、接收器
构建结果与真实左右手配对是不同层次的证据。将自己的键盘接入后，需要分别完成
对应三块设备的构建和上述输入/显示验收。
