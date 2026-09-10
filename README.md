# ZMK Sofle Dongle — DYA Studio

这是为 Sofle 分体键盘和 Prospector 彩屏接收器维护的 ZMK 固件仓库。

本项目基于 `zmk-sofle-dongle-dya` 的 4.1 配置，保留 DYA Studio、运行时配置和原有 keymap，并将 USB 中央接收器更换为 Prospector。

## 功能

- DYA Studio 改键
- Runtime Macro
- Runtime Combo
- Runtime Sensor Rotate 编码器配置
- Runtime Input Processor
- BLE 管理
- Settings RPC
- Device Info（固件、硬件和运行状态诊断）
- Prospector 240×280 彩屏显示
- 左右手电量显示
- Prospector 层级、连接状态和修饰符显示

### 技术栈

- ZMK：`cormoran/zmk#main+dya`
- Zephyr：`v4.1.0+zmk-fixes+nrf-half-duplex-uart`
- DYA Studio Custom Protocol
- `zmk-feature-custom-settings`
- `zmk-feature-device-info`
- `zmk-feature-runtime-macro`
- `zmk-feature-runtime-combo`

## 固件文件

GitHub Actions 构建完成后，在运行记录的 Artifacts 中下载固件压缩包。

| 固件 | 刷写位置 |
| --- | --- |
| `eyelash_sofle_prospector_dongle.uf2` | Prospector 接收器 |
| `eyelash_sofle_peripheral_left...uf2` | 键盘左手 |
| `eyelash_sofle_peripheral_right...uf2` | 键盘右手 |
| `settings_reset...uf2` | 清除 ZMK 配对与设置 |

建议接收器、左手和右手使用同一次 Actions 构建生成的固件，不要混用不同构建批次。

如连接异常，可依次刷入 `settings_reset`，再重新刷接收器、左手和右手固件并重新配对。清除设置会删除已保存的蓝牙配对和运行时配置。

## Runtime Macro

`4.1` 分支已启用 Runtime Macro，现有 keymap 中的静态 Macro 仍然保留，两者互不冲突。

第 4 层左上角按键绑定为：

```dts
&rmacro 0
```

使用方法：

1. 用 USB 连接接收器。
2. 打开 DYA Studio。
3. 进入 Macro 页面。
4. 创建 Macro 并确认其 Slot 编号。
5. Slot 0 对应当前预留的 `&rmacro 0` 按键。
6. 点击保存后，Macro 会写入接收器设置。

刚刷入固件、尚未创建 Slot 0 时，按下该键不会执行任何内容。

## Runtime Combo

`4.1` 分支已启用 Runtime Combo，可以通过 DYA Studio 在运行时创建和修改组合键。

它与 Runtime Macro 可以共存：Combo 负责监听多个按键位置，Macro 负责执行一串行为。现有静态 `softoff` Combo、静态 Macro 和 Runtime Macro 均保持不变。

使用方法：

1. 用 USB 连接接收器并打开 DYA Studio。
2. 进入 Runtime Combo 子系统页面。
3. 选择空 Slot，设置名称、按键位置、输出行为、适用层和超时时间。
4. 保存并测试；需要断电保存时启用持久化选项。

固件只预留运行时 Combo 槽位，没有增加默认 Combo，因此首次刷写不会改变现有按键行为。

## Device Info

`4.1` 分支仅在接收器固件中启用 Device Info。通过 USB 连接接收器并打开 DYA Studio 的 Troubleshooting 页面后，可以查看：

- ZMK、Zephyr、配置仓库及模块的版本信息
- 编译时间、板型和固件 Build ID
- MCU、Flash、SRAM 和上次复位原因
- USB、BLE、分体、显示等编译配置
- 接收器运行时间和 Zephyr 设备初始化状态

设备信息默认遵循 Studio 的安全访问设置。左右手固件不启用该模块；DYA 读取的是 USB 接收器本身的信息。

## 编码器

当前 keymap 中：

- BASE：音量控制
- NAV：音量控制
- SYS：上下滚动
- 第 4 层：固定滚动行为

编码器绑定可通过 DYA Runtime Sensor Rotate 页面修改。旧 OLED 的 `dongle_display_settings` 已完全移除。

## Prospector 主题切换

接收器内置四个主题：`FIELD`、`OPERATOR`、`RADII` 和 `SOFLE // CODEX`。

- 轻点屏幕：切换到下一个主题。
- 向左或向右滑动：切换到上一个或下一个主题。
- 停止切换 750 ms 后，当前主题会保存到 NVS；重启后继续使用该主题。

首次刷写默认使用 `SOFLE // CODEX`。执行接收器 Settings Reset 后也会恢复默认主题。

触摸屏需要在显示接线之外连接四根触摸信号线：`TP_SDA → D4`、`TP_SCL → D5`、
`TP_INT → D0`、`TP_RST → D1`。仅使用带触摸面板的 LCD、但未连接这四根线时，
屏幕可以正常显示，但无法切换主题。

## 编译

仓库使用 GitHub Actions 自动构建：

1. 切换到需要构建的分支。
2. 打开 Actions。
3. 运行 Build workflow，或向该分支提交一次改动。
4. 等待全部 Build Job 完成。
5. 下载 Artifacts。

`4.1` 目前属于开发分支。刷写前必须确认接收器、左右手和 `settings_reset` 均构建成功。

## 注意事项

- 不要将 `main`、`combo` 和 `4.1` 的接收器与左右手固件混刷。
- 修改 DYA 运行时设置前，确保连接的是接收器串口。
- 浏览器提示串口已打开时，关闭其他 DYA Studio 页面或占用串口的软件。
- 刷写新版底层后出现连接问题时，优先执行一次完整的 Settings Reset 和重新配对。
- `4.1` 分支仍需通过 Actions 编译和实机验证后再作为日常固件使用。

## 键位图

![Sofle 键位图](keymap-drawer/eyelash_sofle.svg)

## 参考项目

- [DYA Studio Developer Guide](https://studio.dya.cormoran.works/developer-guide)
- [cormoran/zmk-feature-runtime-macro](https://github.com/cormoran/zmk-feature-runtime-macro)
- [cormoran/zmk-feature-custom-settings](https://github.com/cormoran/zmk-feature-custom-settings)
- [englmaxi/zmk-dongle-display](https://github.com/englmaxi/zmk-dongle-display)

## 联系方式

如需 3D 打印模型文件，或键盘出现异常和故障，请联系：

`380465425@qq.com`
