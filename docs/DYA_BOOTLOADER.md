# DYA Devtool Bootloader

Connect to the scanner or dongle in DYA, open **Devtool** at the bottom right, then click **Bootloader**. The neighbouring **Reboot** button performs an ordinary reboot. No keymap selection, edit or physical key press is required.

The UI sends `Request { enter_bootloader: {} }` through custom subsystem `cormoran__devtool`. The pinned `cormoran/zmk-module-devtool` commit `9938a2c73c06d6e65ba2c0b68c9aa044a467eafc` already handles this RPC, returns the Bootloader response and schedules a reboot after 250ms. On kblade it sets the retention boot mode and reboots warm using the board's Adafruit nRF52 bootloader mapping.

Both targets enable `CONFIG_ZMK_DEVTOOL=y` and `CONFIG_ZMK_DEVTOOL_STUDIO_RPC=y`. Existing dongle configuration already enabled these; scanner configuration now does too. The incorrectly added keymap action hook has been removed. No dependency source patches are required. Settings, pairing information and key bindings are not erased by this operation. It enters the connected device bootloader only.

## Verification scope

Frontend request/subsystem and firmware handler/protocol were checked at their actual source. The native handler is host-tested with mocked transport, work and reboot APIs. Firmware compilation and physical Devtool-to-BL acceptance remain pending.
