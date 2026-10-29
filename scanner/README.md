# kblade ms2 九主题扫描仪

独立分支：`codex/kblade-scanner-nine-themes`。
基于 Dongle 主题提交 `83bc9c3`，保留原 `codex/kblade-dya-four-themes` 分支。

其他键盘如何加入扫描仪，见 [GUIDE：其他键盘接入 Scanner](../GUIDE.md#其他键盘接入-scanner)。

## 硬件与数据来源

- Board：`kblade//zmk`，显示 Shield：`prospector_ms2`，屏幕 280×240，R43+2 前景安全区。
- 独立 Shield：`scanner_display`，使用无事件 mock kscan，不占用物理按键 GPIO。
- 被动接收键盘广播中的状态；BLE 扫描使用 active scan 获取设备名和状态包，
  不建立 HID 或分体连接。键盘原有主机连接不转移到扫描仪。
- 键盘端必须发送兼容的 Prospector Manufacturer Data；不是任意蓝牙键盘都能显示。
- 继承协议：26 字节包，Company ID `FFFF`、协议签名 `AB CD`。
  层名只有 4 字节，WPM/修饰键/电量/USB/BLE 状态均来自广播，不能伪造完整层名。
- 两个显示电量默认对应 `battery_level`、`peripheral_battery[0]`。键盘主手不一定
  是左手；用 `left_hand_slot/right_hand_slot` 交换显示顺序。

## 主题与控制

DYA Studio 通过 USB 访问**扫描仪自己的屏幕设置**，不能通过扫描仪改接收键盘的键位。

| display_theme | 主题 |
| --- | --- |
| 0 | FIELD |
| 1 | OPERATOR |
| 2 | RADII |
| 3 | YADS，黑底白字 |
| 4 | INDUSTRIAL |
| 5 | SEGMENT |
| 6 | ORBIT |
| 7 | SPLIT |
| 8 | MONO，含最近 60 个每秒 WPM 采样点 |

`display_palette`、`display_brightness` 和 Save 保持可用。
`scanner_keyboard` 为 0–2，选择接收键盘槽位；目标尚未出现时等待其广播，
已存在的键盘可继续显示。频道沿用 ms2 的已保存设置，默认值来自硬件模块。
`left_hand_slot/right_hand_slot` 各取 0/1，必须不同。

主题中的 USB/蓝牙图标代表**被接收键盘的输出状态**，不代表扫描仪 USB 供电或扫描链路。
广播没有逐键字符，所以 YADS 显示广播层名，不提供本机模拟的输入字母历史。
MONO 的曲线在切换发送器、广播超时和离开主题时清空；初始曲线为空，不填入演示数据。
没有有效数据时显示 WAIT/未知电量，并停止曲线采样。

扫描接收列表最多 3 台。超时沿用接收模块的 480000 ms（8 分钟），可在
`config/scanner/scanner_display.conf` 修改。应大于键盘空闲时的广播间隔，
否则低频空闲广播会被误判为掉线。它不是已连接 HID 的断线提示。

## 构建入口

硬件模块已放入本仓库 `hardware/prospector-zmk-module/`，来源提交为
`kbladewht/prospector-zmk-module@37146e417cd95ec95ab144ed9ebb6632da06bd8e`。
west 不再远程引用该硬件项目。接收核心随本目录提供，不依赖已经被该硬件模块移除的旧接收源码。

```sh
# 初始化本分支 west.yml 后，在 west workspace 中：
bash zmk-prospector/scripts/build-ms2-scanner.sh "$PWD" --dry-run
# 用户确认后才实际构建：
bash zmk-prospector/scripts/build-ms2-scanner.sh "$PWD"
```

实际目标：`kblade//zmk` + `prospector_ms2 scanner_display`。
配置目录为 `config/scanner`，输出 `build-ms2-scanner/zephyr/zmk.uf2`。
手动 Actions 产物名为 `kblade_ms2_scanner_nine_themes`，不会自动刷机。
本分支默认工作流只构建扫描仪，原 Dongle 构建脚本仍保留在仓库中。

## 验证

```sh
bash scanner/tests/run.sh /path/to/lvgl /path/to/kblade-hardware-module
python3 display-themes/tests/host/run.py /path/to/lvgl /path/to/kblade-hardware-module /tmp/scan-ui-check
```

接收测试编译真实 `status_scanner.c`、`scanner_core.c`、ms2 适配器，
只替换 BLE 驱动、时钟、内核调度和显示调用；启用 AddressSanitizer/UBSan。
覆盖协议签名、频道、不完整广播、两台发送器、键盘选择、DYA 槽位选择、电量顺序、超时及曲线重置通知。
LVGL 测试覆盖 216 组九主题状态与 2752 个前景边界，以及连接图标、修饰键、
ORBIT 电量弧、MONO 曲线和静止重复刷新检查。

当前已完成主机检查和 Zephyr 编译：提交 `f37afec`，
[Actions 40836266045](https://github.com/S7venYoung/zmk-prospector/actions/runs/40836266045) 成功。
Flash 656496 B（70.61%），RAM 168484 B（64.27%），UF2 文件校验通过。
配置确认 `BT_OBSERVER=y`，`BT_CONN/BT_CENTRAL/BT_PERIPHERAL/ZMK_SPLIT/ZMK_BLE` 均关闭。
仅保留一个 `&none` 占位 keymap 供 ZMK/Studio 初始化，不处理实际键盘按键。
尚未进行实机接收测试。
广播收发、USB DYA Save/重启恢复、RF 并发和实际功耗仍需构建后验证。
