# ms2 original scanner renderers + YADS + DYA settings

The module restores FIELD / OPERATOR / RADII from
`S7venYoung/prospector-zmk-module@7ff9302680c7c5dc9e12b4f3497204c99881de08`,
`boards/shields/prospector_scanner/src/`. Their MIT notices and the upstream
license are retained. Fonts are linked from the existing ms2 hardware module.

The ms2 hardware/data bridge is pinned to
`kbladewht/prospector-zmk-module@37146e417cd95ec95ab144ed9ebb6632da06bd8e`,
its **master** branch. The default main branch does not contain kblade/ms2.
The pinned board application partition starts at **0x1000** and ends at 0xE4000;
this port does not change those boundaries or the storage/boot partitions.

The YADS portable input model and information hierarchy are adapted from
`S7venYoung/zmk-sofle-dongle-dya@9cdaf992b7c840c4eaa4320fb0768c5a40436b99`
(`4.1-display-themes`), especially
`boards/shields/dongle_display/widgets/typed_keys_status.c` and
`custom_status_screen.c`. The user requested **black background / white text**.

The original module manager had the three renderer calls commented out. This
module excludes that manager on the app target and supplies its own, retaining
ms2's existing custom-screen host, log-page host hooks, and numeric layout IDs.
The four IDs are 1=FIELD, 2=OPERATOR, 3=RADII, 4=YADS. ID 4 was YADS2 upstream;
this module deliberately supplies the requested OLED-derived YADS in that slot.

For the original kbladewht core without DYA, the optional individual shields
`prospector_theme_field`, `prospector_theme_operator`, `prospector_theme_radii`,
and `prospector_theme_yads` provide fixed boot defaults. The helper is
`scripts/build-ms2-theme.sh WORKSPACE THEME [--dry-run]`. The full DYA project
uses no fixed-theme shield: select the theme in Custom Settings instead.

## DYA-core integration

The root west manifest pins the same `main+dya` stack used by the Prospector
Dongle project, including its custom Studio messages and Custom Settings API.
The stock kbladewht master does not provide that API; merely setting
CONFIG_ZMK_STUDIO=y on that core is insufficient.

The complete config enables the listed DYA feature modules. The local Runtime
Macro implementation, headers, binding, and key-statistics source were reused
from `zmk-prospector` main. External feature module SHAs are recorded in west.yml.

`dongle_display_settings` uses the standard Custom Settings module for schema,
access rules, Write, Save and NVS restoration. Setting events only flag a pending
update. A 50 ms LVGL timer performs the actual theme/palette/brightness changes.
No LVGL object is changed from the RPC/event callback.

The DYA battery adapter uses the public split-transport list of reachable source
IDs plus the central battery cache, and feeds ms2's existing ble_battery_update
hook. Disconnected sources are explicitly reported as unavailable. Unlike the
original kbladewht BLS extension, this DYA core identifies hands by split source
slot, configurable through left_hand_slot/right_hand_slot. Both values must be
different. The upstream cache uses 0 to mean unknown/disconnected, so 0% remains
indistinguishable from an unavailable reading on screen. No dongle battery is
invented for either hand.

DYA layer names are obtained from the live keymap by layer index-to-ID conversion,
rather than the legacy four-byte advertisement name. Existing GPIOs, display
rotation, PWM inversion and UF2 partitions stay in the hardware module.

## Host validation

Use LVGL at exactly `zmkfirmware/lvgl@f1db87ee98f1810328a8419572fa42a3b5f352ae`:

```sh
python3 tests/ms2_host/run.py LVGL_SOURCE MS2_HARDWARE_MODULE OUTPUT_DIR
clang -std=c99 -Wall -Wextra -Werror -I themes/scanner-original/src \
  tests/ms2_host/history_test.c themes/scanner-original/src/yads_history.c \
  -o /tmp/yads-history-test
/tmp/yads-history-test
```

The render test checks the conservative R43 px corner envelope with a further
2 px margin. Actual label, arc, image and bar rectangles are inspected after
LVGL layout, including long names, all modifiers, missing hand data, and layer 9.
It covers 4 themes x 4 palettes x 4 states (YADS stays monochrome), plus repeated
next/previous switching and a mocked DYA event/setting contract. It emits PPMs.
The mock does not stand in for a Zephyr, serial-RPC, NVS or physical test.

The attached reference firmware was read only:
`zmk__oled_ms_1759.uf2`, SHA256
`69ad3248848c1dec285a3ed376d046bddcb76efd7ddfaca422c31c796197cfb3`.
It contains nRF52840 family `0xADA52840`, Zephyr 4.1.0, ST7789V and
DongleProspector strings, and UF2 application data beginning at 0x1000. It was
not modified, flashed, or treated as a substitute for editable source.
