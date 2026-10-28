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
