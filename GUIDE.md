# 扫描仪 GUIDE

本页适用于 **`codex/kblade-scanner-nine-themes`** 分支。板卡始终为 `kblade//zmk`，显示屏为 ms2 的
280×240 横屏，前景按 R43+2 px 圆角安全区布局。

## 模式与目录

本分支独立扫描键盘发送的 Prospector 广播，不作为左右手的 split central。
键盘仍连接原来的电脑/接收器；扫描仪只显示收到的状态。
键盘端必须发送兼容广播，普通蓝牙键盘不能仅靠 HID 自动提供这些数据。

| 位置 | 内容 |
| --- | --- |
| [本地 board](hardware/prospector-zmk-module/boards/kblade/) | kblade DTS、引脚、分区、板级配置 |
| [ms2 Shield](hardware/prospector-zmk-module/boards/shields/prospector_ms2/) | 屏幕及背光配置 |
| [display-themes](display-themes/README.md) | 九个主题、DYA 适配、字体及主机测试 |
| [config](config/) | 固件配置和起始 keymap |
| [硬件来源](hardware/UPSTREAM.md) | 本地硬件模块的固定来源与校验信息 |

硬件模块已经收进本仓库，不再通过 west 下载 `prospector-zmk-module`。
不要再添加同名远程硬件模块，否则可能重复注册 board/Shield。
Zephyr、ZMK、DYA 扩展、工具链仍是正常构建依赖，本仓库并非完整离线 SDK。

## 从零构建本分支

先准备 Git、west、CMake/Ninja 和可用的 Zephyr 工具链。仓库内依赖版本以
[west.yml](west.yml) 为准；当前 DYA 核心固定在
`cormoran/zmk@e5c9b6915b56801193e359dd9bad4a167ce0d1b8`，不应直接改成任意 ZMK tag。
以下为 Bash 命令；目录名可自行替换。

```sh
mkdir -p zmk-ms2-scanner
cd zmk-ms2-scanner
git clone --single-branch --branch codex/kblade-scanner-nine-themes \
  https://github.com/S7venYoung/zmk-prospector.git zmk-prospector
west init -l zmk-prospector
west update --narrow --fetch-opt=--depth=1
west zephyr-export

# 只打印命令：
bash zmk-prospector/scripts/build-ms2-scanner.sh "$PWD" --dry-run
# 实际构建：
bash zmk-prospector/scripts/build-ms2-scanner.sh "$PWD"
```

脚本使用：

```text
board:    kblade//zmk
shield:   prospector_ms2 scanner_display
output:   build-ms2-scanner/zephyr/zmk.uf2
artifact: kblade_ms2_scanner_nine_themes
```

脚本自动加入本地 `hardware/prospector-zmk-module` 和仓库根模块。
请从 **west workspace** 传入路径，不要将 `zmk-prospector` 子目录当成 workspace。
修改板卡或配置后，若旧 CMake 缓存保留了其他硬件路径，应使用独立构建目录或先 pristine。

### GitHub Actions

打开仓库 Actions → `build.yml` → Run workflow，分支选择 `codex/kblade-scanner-nine-themes`。
该工作流只有手动触发入口，普通提交不自动构建。完成后下载 `kblade_ms2_scanner_nine_themes`。
检查运行的提交号与自己准备刷入的版本一致，再提取 `zmk.uf2`。

`zmk.uf2` 是刷写文件，`zmk.elf` 用于调试，`.config` 用于核对功能配置。
进入板卡的 UF2 bootloader 后复制对应 UF2；构建脚本和文档命令不会自动刷写设备。
本分支目标是 nRF52840 的 kblade，不适用于将同一 UF2 刷入不同主控/引脚/分区的板卡。

## DYA 与主题选择

连接接收器/扫描仪的 Studio USB 接口，在 DYA Studio 的 Settings 中找到
`dongle_display_settings`。这个名称是共用协议标识，扫描仪版也沿用它。

| DYA `display_theme` | 主题 | 特点 |
| --- | --- | --- |
| 0 | FIELD | 原版网格与状态布局 |
| 1 | OPERATOR | 原版层指示与信息面板 |
| 2 | RADII | 原版圆形层指示 |
| 3 | YADS | 黑底白字 |
| 4 | INDUSTRIAL | 黄黑工业风、长电池条 |
| 5 | SEGMENT | 琥珀色数码管、大号 WPM |
| 6 | ORBIT | 青色圆环、镜像电量弧 |
| 7 | SPLIT | 大层名与蓝色侧栏 |
| 8 | MONO | 米白/橙色、竖排修饰键、WPM 曲线 |


| 其他设置 | 说明 |
| --- | --- |
| `display_palette` | 0–3，仅 FIELD/OPERATOR/RADII 使用；其余主题固定配色 |
| `display_brightness` | 1–100，沿用 ms2 背光反向 PWM 逻辑 |
| `left_hand_slot` / `right_hand_slot` | 各取 0/1，必须不同，可调换两个显示电量的来源 |

Write 应用到显示线程，Save 保存后重启恢复。
未知电量显示占位符，INDUSTRIAL/SEGMENT 不显示百分号。
新主题的 USB/蓝牙图标随状态切换；同时有 USB 和 BLE 时优先显示 USB。
MONO 每秒记录一个 WPM 值，保留最近 60 个有效采样点，离开主题会清空。

### 广播与键盘选择

接收格式为 26 字节 Manufacturer Data：Company ID `FFFF`、协议签名 `AB CD`。
使用的是 BLE 广告扫描；active scan 用于请求广播响应中的设备名，不建立键盘 HID 连接。
WPM、修饰键、层、电量、USB/BLE 输出状态均来自发送器，不读取扫描仪自己的键位。

DYA 的 `scanner_keyboard` 取 `0`–`2`，选择接收槽位。槽位按接收器发现设备的顺序形成，
不是左手/右手编号；目标暂未出现时等待其广播。最多维护 3 个接收槽位。
频道沿用 ms2 的保存设置；发送器与接收器频道必须匹配，或使用协议允许的全部频道设置。

广播层名只有 4 字节，不能从扫描仪的占位 keymap 推断键盘完整层名。
YADS 保留黑白布局，但广播没有逐键字符，因此不显示字母输入历史。
USB/蓝牙图标代表发送键盘的输出状态，不代表扫描仪的 USB 供电状态。

接收超时配置是 `config/scanner/scanner_display.conf` 中的
`CONFIG_PROSPECTOR_SCANNER_TIMEOUT_MS=480000`（8 分钟）。它应大于发送器的空闲广播间隔。
切换发送器或超时后清空旧的 MONO 曲线；无有效数据时显示 WAIT 和未知电量。

## 其他键盘接入 Scanner

Scanner 不要求将键盘改成 Dongle 三件套。键盘继续连接原来的电脑/接收器，
由**主手、独立键盘或现有 Dongle**额外发送兼容的状态广播；Scanner 只接收和显示。
普通蓝牙 HID 广告不包含完整层、WPM、修饰键与电量信息，不能直接替代状态广播。

### 1. 在键盘发送端加入广播模块

在键盘工程中引入与其 ZMK/Zephyr 版本兼容、且确实包含发送实现的 Prospector
状态广播模块。典型功能开关是：

```conf
CONFIG_ZMK_STATUS_ADVERTISEMENT=y
```

只有配置开关不够：发送源码必须实际参与编译、完成初始化，并在键盘状态变化时
更新广播。具体 west 模块版本及附加配置应以发送模块为准，不能混用不同 ZMK 版本。
本分支本地 `hardware/prospector-zmk-module/` 的发送入口已被注释，且不含
`src/status_advertisement.c`；它用于接收器硬件，**不能作为现成发送端模块直接引用**。

通常只在键盘主手或现有 Dongle 上开启广播；左右手同时广播可能占用不同接收槽位。
保留原 keymap、主机输出与绑定，不需要为了接入 Scanner 默认清配对。

### 2. 核对发送协议与数据

本分支接收 [scanner/include/zmk/status_advertisement.h](scanner/include/zmk/status_advertisement.h)
定义的 26 字节 Manufacturer Data：

| 内容 | 要求 |
| --- | --- |
| Company ID | 字节 `FF FF` |
| 协议签名 | 字节 `AB CD` |
| 键盘标识 | 稳定的 `keyboard_id`、角色及设备标识 |
| 层 | `active_layer` 与 4 字节 `layer_name` |
| 输入状态 | HID 修饰键位图、`wpm_value` |
| 电量 | `battery_level`、`peripheral_battery[]` |
| 输出状态 | USB HID ready、BLE connected/bonded 等状态位 |
| 频道 | `channel` |

字段布局、长度和位定义必须匹配，不能只复用 Company ID。WPM 和副手电量还需发送端
具有相应数据源；Scanner 不会凭空计算远端按键速度或伪造电量。
广播没有逐键字符，因此 YADS 只显示广播层名，不显示字母输入历史。

### 3. 匹配频道并选择键盘

先确认键盘实际发出了兼容广播，再在 Scanner 中核对频道。当前接收代码规则是：

- Scanner 频道为 `0` 或 `>=10` 时接收所有频道。
- 发送端频道为 `0` 时可被任意 Scanner 频道接收。
- 其他情况，双方频道必须相同。

这些是本分支协议规则，不应套用于其他广播模块。
Scanner 运行时频道沿用 ms2 的保存设置；`CONFIG_PROSPECTOR_SCANNER_CHANNEL`
是没有运行时频道接口时的回退值，修改它不一定覆盖已保存设置。

收到广播后，用 USB 将 Scanner 接入 DYA Studio，在
`dongle_display_settings → scanner_keyboard` 选择 `0–2` 槽位。
槽位由接收器发现设备的顺序形成，不是左右手编号。
若主手电量对应的左右方向不同，使用 `left_hand_slot/right_hand_slot` 交换。
全部九个主题共用这份状态，无须为每个键盘逐一修改主题。

### 4. 分别构建并验收

发送端需要按自己的键盘工程重新编译；Scanner 按本指南构建。
两端主控、引脚、分区和功能不同，不刷同一个 UF2。

核对顺序：键盘原有输入正常 → 广播被发现 → 选中正确槽位 → 层/修饰键/WPM 正确 →
电量来源正确 → USB/BLE 图标反映键盘输出 → 主题切换与 MONO 曲线正常。
断开发送器后，需等待配置的接收超时再判定掉线；超时要大于键盘空闲广播间隔。

本接入说明不是一份适用于所有键盘的发送固件。实际移植需要键盘仓库、分支、主手/
接收器角色与 ZMK 版本，以确定可用发送模块及构建配置。

## 主题引用

### A. 引用完整工程

其他配置仓库可将本仓库加入 west manifest：

```yaml
manifest:
  projects:
    - name: zmk-prospector
      url: https://github.com/S7venYoung/zmk-prospector
      revision: codex/kblade-scanner-nine-themes  # 发布自己的配置时，建议锁定已验证的 commit
      path: modules/zmk-prospector
```

这段**只增加项目**，不会自动替换你已有的 ZMK、Zephyr 或 DYA 依赖。
完整 DYA 功能应沿用本仓库 [west.yml](west.yml) 的依赖集合，或合并并固定到相同版本。
若选择 `import: {file: west.yml}` 导入本项目依赖，需同步处理上层 manifest 中已有的
同名 ZMK/Zephyr 项目：上层声明可能覆盖导入的版本，不能认为加一行 import 就完成升级。

构建时通过 `ZMK_EXTRA_MODULES` 加入：

```text
/path/to/zmk-prospector/hardware/prospector-zmk-module
/path/to/zmk-prospector
```

两个路径用分号分隔，并保留自己已有的额外模块。
如果本仓库已被 west 自动发现，根模块只需注册一次；不要再额外加载其 `display-themes/`。
完整 Dongle 改成其他键盘时，不能只换一个主题设置，参见
[其他键盘接入 Dongle](docs/OTHER_KEYBOARD_DONGLE.md)。

### B. 只复制/引用主题模块

将 [display-themes/](display-themes/) 整个目录复制到自己的项目，保留许可证与字体来源。
使用兼容 ms2 接口时，可单独加载：

```text
-DZMK_EXTRA_MODULES="/path/to/hardware/prospector-zmk-module;/path/to/display-themes"
```

模块入口为 `display-themes/zephyr/module.yml`，最小主题配置为：

```conf
CONFIG_ZMK_DISPLAY=y
CONFIG_ZMK_DISPLAY_STATUS_SCREEN_CUSTOM=y
CONFIG_ZMK_PROSPECTOR_SCANNER_ORIGINAL=y
# 仅在已有兼容 DYA/Studio/Custom Settings 时启用：
# CONFIG_ZMK_PROSPECTOR_DYA_SETTINGS=y
```

- 不要同时加载同一份代码的仓库根模块和主题子模块，否则会重复注册配置/源码。
- 主题接口依赖 ms2 的 `prospector_layouts.h`、`display_settings.h`、公共字体及状态桥；
  LVGL 基线为 `f1db87ee98f1810328a8419572fa42a3b5f352ae`（9.x API）。
- 其他分辨率、屏幕驱动或非 ms2 项目需要状态/字体/画布适配，不是任意 ZMK 屏幕即插即用。
- 扫描仪版需要完整仓库的 [scanner/](scanner/README.md) 接收适配（仅扫描仪分支提供），
  单独复制主题不会自动获得广播接收功能。
- 内部布局 ID 是 1–9，DYA 值是 0–8，二者相差 1。硬件默认布局 Kconfig 仍限 0–4；
  选新主题请使用 DYA，或在 LVGL 线程初始化布局后调用 `prospector_layouts_set_style()`。
  不能把 `CONFIG_PROSPECTOR_DEFAULT_LAYOUT` 写成 8 来选择 MONO。

## 检查与排错

1. 找不到 board：确认本地 `hardware/prospector-zmk-module/zephyr/module.yml` 存在，
   且构建命令包含该目录；移除旧的同名远程硬件模块和旧构建缓存。
2. DYA 没有主题设置：检查 `.config` 的 Studio、Custom Settings、主题模块及 DYA 设置
   是否实际为 `y`，并确认连接的是 RPC USB 接口而非日志串口。
3. 电量方向/来源不对：先确认真实数据，再调整两个 slot；不要以固定数字冒充电量。
4. 新板卡/新键盘先验证原生按键或广播数据，再验证屏幕，避免只凭 UI 判定输入链路正常。

主机布局测试从仓库根目录运行：

```sh
python3 display-themes/tests/host/run.py /path/to/lvgl \
  hardware/prospector-zmk-module /tmp/ms2-theme-check
```

真实固件是否编译成功，以当前提交对应 Actions 和 `.config` 为准；主机渲染与 dry-run
不等于 Zephyr 构建。当前图标/曲线、本地硬件迁移需在新固件中验证；不使用旧 UF2
代替当前源码的验证结果。

## 扫描仪构建记录

`f37afec` 已通过 [Actions 40836266045](https://github.com/S7venYoung/zmk-prospector/actions/runs/40836266045)，
包含本地 kblade/ms2、全部九个主题、连接图标和 MONO 曲线。Flash 70.61%，RAM 64.27%。
UF2 校验和观察者角色配置已核对；实机广播接收和 USB DYA 操作仍待验证。

## DYA 提示不支持 Settings RPC

若设备已经显示为已连接，但“设置”页提示不支持 Settings RPC，请核对实际刷入固件的
`.config`，以下两项必须为 `y`：

```conf
CONFIG_ZMK_SETTINGS_RPC=y
CONFIG_ZMK_SETTINGS_RPC_STUDIO=y
```

这两项提供 DYA 的核心设置接口；主题设置还需要原有的
`CONFIG_ZMK_CUSTOM_SETTINGS_STUDIO_RPC=y` 与
`CONFIG_ZMK_PROSPECTOR_DYA_SETTINGS=y`，两套接口不能互相替代。
`f37afec` 构建缺少核心 Settings RPC，后续源码已补齐，需重新构建并刷入才能生效。
本次配置修复不启用 split 或键盘 BLE 角色。

## 启动后背光突然变暗

旧扫描屏幕在没有收到键盘广播时会应用 `PROSPECTOR_SCANNER_TIMEOUT_BRIGHTNESS=1`，
覆盖 DYA 亮度。后续源码在启用 DYA 显示设置时禁用了这条旧的亮度覆盖，以及键盘
恢复时的固定亮度覆盖；背光由 `display_brightness` 控制。无需清除 NVS 或配对。
启动 PWM 与 DYA 默认 60% 对齐，保存过的 DYA 亮度会在设置加载后应用。
这不修改“无有效键盘广播”的状态判断或接收超时，只改变背光控制归属。

## USB 能识别，但 DYA 握手超时或拔插后才能恢复

当前 kblade 板默认 CDC、Studio RPC CDC 和日志 CDC 一共三个串口。
Zephyr 4.1 的旧 USB CDC 驱动在配置设备时为每个串口提交一项 OUT 接收和一项
IN 零长度传输；USB 传输池默认只有 4 项，不能覆盖最坏情况下的 6 项请求。
本机旧固件日志曾出现 `usb_transfer: No transfer slot available`。提交失败后
CDC 驱动没有重试，可能留下已枚举但不能收发的串口。

扫描仪配置将 `CONFIG_USB_MAX_NUM_TRANSFERS` 设为 8，并在编译时检查传输池
至少覆盖所有启用 CDC 的两倍数量。保留原有接口顺序，Studio 和日志仍分别使用
自己的 CDC。`CONFIG_ZMK_LOGGING_MINIMAL=y` 取消 ZMK 强制 level 4 的默认值，配合
`CONFIG_ZMK_LOG_LEVEL_WRN=y` 关闭广播回调的大量 DEBUG 日志；
仅设置 `CONFIG_LOG_DEFAULT_LEVEL=2` 不能覆盖 USB logging 对 ZMK 日志级别的默认值。

这是基于旧固件配置、USB 驱动和日志定位到的容量缺陷；修复后的反复连接、关闭网页、
USB 拔插仍须实机验证。若网页提示 `The port is already open`，还需要检查浏览器
是否仍持有串口；这个浏览器错误本身不等于固件占用了 Mac 的串口句柄。
无需清除 NVS、配对或刷 settings reset。

## 开机动画：扫描线版

`CONFIG_ZMK_PROSPECTOR_BOOT_ANIMATION=y` 启用一次性开机动画：黑底琥珀色扫描线，
从左到右揭示 `CORNE`，显示 `SCANNER` 和上下细线，最后淡入已保存的主题。
总时长约 1.1 秒，仅第一次创建主题时播放；切换主题或离开再返回不会重播。
设为 `n` 可在编译时关闭，目前没有新增 DYA 动态开关。

动画使用独立 LVGL 顶层覆盖层和 32 ms 定时器，没有 sleep 或等待 USB/蓝牙的循环；
主题恢复与广播接收照常进行。销毁主题会取消未完成动画，结束后删除覆盖层和定时器。
使用现有字体源生成五字母 2bpp 字库，不分配全屏位图或模糊缓冲区。
视觉基于扫描线概念稿，以原生 LVGL 线条和窄体字实现。

主机 LVGL 测试覆盖动画分镜的圆角安全区、完成后的覆盖层清理、一次播放，以及
原有 216 项主题状态。主机渲染不能代替实机帧率、SPI 刷新与启动时序验证。

## 修饰键延迟与快速短按

接收端原先将广播延迟 50 ms 后批处理，界面每 100 ms 才读取一次最新快照。
同一批中收到按下和松开时，中间状态会被最终快照覆盖。现在广播到达即通过
`k_work_reschedule` 唤醒处理，扫描仪界面改为每 20 ms 取状态；空闲维护仍每 100 ms
执行，WPM/电池等低优先级更新仍保留原节奏。

接收端记录当前选中键盘已收到的修饰键上升沿，在界面首次消费该状态后至少显示
80 ms，然后恢复实际状态。此机制只影响显示，不修改键盘快照、广播协议或 HID；
长按仍持续点亮，切换键盘或接收超时会清除旧的显示脉冲。重复短按可能合并成一次
连续亮起，显示层不是按键事件记录器。

这不能还原键盘没有广播或无线丢失的按下包。若长按仍明显滞后，应进一步检查键盘
发送端的广播间隔和修饰键事件触发，不能仅靠扫描仪增大刷新率保证无线无丢包。
主机接收测试覆盖同批按下/松开、80 ms 熄灭、长按、多修饰键和原始状态不被修改；
尚需实机验证端到端延迟。

### 约 2 秒修饰键延迟的时序诊断

`CONFIG_ZMK_PROSPECTOR_SCANNER_TIMING=y` 临时启用修饰键变化日志：
- `SCAN_RX`：修改后的广播进入扫描仪队列，到处理时的 `queue_ms`；
  `packet_gap_ms` 是同一键盘相邻广播到达间隔。
- `SCAN_VIEW`：这份广播到界面取走状态的 `rx_to_view_ms`，以及显示值/原始值。

两项都不包含物理按键到广播到达的无线等待，VIEW 也不代表最终 SPI 刷屏完成。
因此不能仅凭 VIEW 时间短就宣称端到端快：还需对照按键时刻、键盘发送端和屏幕。
日志只记录修饰键变化，不输出周边设备名称，不恢复全量 DEBUG。排查后可关闭。
