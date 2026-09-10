- [中文](README.md)
- [English](README_EN.md)

# ZMK Sofle Prospector

本仓库以原 Sofle + DYA Studio keymap 为基础，将 USB central dongle 更换为
Prospector 触摸屏硬件（Seeed XIAO BLE nRF52840 + 240×280 ST7789）。

## 固件产物

- `eyelash_sofle_prospector_dongle`：Prospector USB central 接收器
- `eyelash_sofle_peripheral_left`：Sofle 左手
- `eyelash_sofle_peripheral_right`：Sofle 右手
- `settings_reset_nice_nano_v2`：左右手清除配对信息
- `settings_reset_xiao_ble`：Prospector 清除配对信息

> 当前里程碑完成彩屏接收器和 DYA Studio 基础迁移。CST816S 触摸设置页、
> DYA 休眠/RGB 参数控制和 macOS Codex 上位机将在后续里程碑加入。

# 更新列表

- 2026/9/10
  1. 接收器构建目标由 `nice_nano_v2 + SH1106 OLED` 更换为 Prospector（Seeed XIAO BLE nRF52840 + 240×280 ST7789）。
  2. Prospector 继续作为左右手的 split central，并保留 USB 键盘、DYA Studio RPC、轨迹球、旋钮和按键统计功能。
  3. 固件产物名为 `eyelash_sofle_prospector_dongle`。首次切换 central 前，左右手和接收器都需要先刷 `settings_reset`，随后重新配对。
  4. 本次先完成接收器与彩屏基础迁移；CST816S 触摸配置和触摸式 DYA 设置界面将在后续阶段加入。

- 2026/7/29
  1. 增加接收器 OLED 按键统计功能。
  2. 屏幕显示历史总计 `T` 和今日总计 `D`，仅统计按键，不统计编码器和摇杆。

- 2026/3/6
  1. 全部模型都有修改，绘制了2各版本的手托
  2. 修复了旋钮失效的问题
  3. 修改了防抖时间，如果用的轴体性能号可以缩减防抖时间。如果用的轴体品质一半，可以拉长防抖时间。
  
- 2024/12/21
  
- 2024/10/24
  1. 修改供电模式，功耗降低。
  2. 修正RGB供电自动关闭的功能。
 
-2026/6/22 键盘支持DYA STUDIO改键了中文用户联系店主索取中文版DYA STUDIO安装包。这个上位机软件改键比ZMK studio更好用。

> 请更新最新的固件。
> 
---
# 联系我

如需3D打印的模型文件或者键盘有任何异常和故障，请联系380465425@qq.com

# Sofle键位图

<img src="keymap-drawer/eyelash_sofle.svg" >
