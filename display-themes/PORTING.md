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
The IDs are 1=FIELD, 2=OPERATOR, 3=RADII, 4=YADS, 5=INDUSTRIAL. ID 4 was YADS2 upstream;
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
python3 display-themes/tests/host/run.py LVGL_SOURCE MS2_HARDWARE_MODULE OUTPUT_DIR
clang -std=c99 -Wall -Wextra -Werror -I display-themes/src \
  display-themes/tests/host/history_test.c display-themes/src/yads_history.c \
  -o /tmp/yads-history-test
/tmp/yads-history-test
```

The render test checks the conservative R43 px corner envelope with a further
2 px margin. Actual label, arc, image and bar rectangles are inspected after
LVGL layout, including full-length DYA names, all modifier combinations, 1%/100%
batteries, missing hand data, and layer 9. Label rectangles are additionally checked
against every clipping ancestor and against other labels.
It covers 5 themes x 4 palettes x 6 states (YADS stays monochrome and INDUSTRIAL stays yellow/black), plus repeated
next/previous switching and a mocked DYA event/setting contract. It emits PPMs.
The mock does not stand in for a Zephyr, serial-RPC, NVS or physical test.

The attached reference firmware was read only:
`zmk__oled_ms_1759.uf2`, SHA256
`69ad3248848c1dec285a3ed376d046bddcb76efd7ddfaca422c31c796197cfb3`.
It contains nRF52840 family `0xADA52840`, Zephyr 4.1.0, ST7789V and
DongleProspector strings, and UF2 application data beginning at 0x1000. It was
not modified, flashed, or treated as a substitute for editable source.

## Font/layout corrections

- OPERATOR's FG Medium 20 font has a 25 px line box. Its former 24 px row clipped
  the bottom pixel; the row is now 26 px. Battery number boxes are 32x25 so three
  DINish digits (100) fit without cropping.
- Layer labels have explicit one-line heights derived from each actual font.
  LV_LABEL_LONG_DOT alone does not prevent an auto-height label from wrapping.
- FIELD's output row clears the full layer line box, its modifier glyphs use a
  common centered column, and animated segments avoid measured foreground text.
- RADII uses equal-width centered modifier cells and an explicitly aligned
  canvas backing buffer. Long layer names stay on one line.
- YADS aligns the top status to the safe left/right edges and centers the visible
  modifier group including its underline width. Labels start empty, so a symbol
  font is never temporarily asked to draw LVGL's default "Text" string.
- FIELD/OPERATOR caches retain up to 31 bytes of the live DYA layer name across
  palette changes instead of silently reverting to a seven-byte cached name.

## INDUSTRIAL reference design

The fifth renderer is a native-LVGL interpretation of the user-supplied robot-eye
industrial dashboard. The supplied portrait composition is rearranged into the
existing 280x240 landscape viewport, retaining the R43+2 px foreground envelope.
The bottom region was explicitly changed to CMD/OPT/CTRL/SHIFT status: corresponding
left/right HID modifier bits are combined, inactive cells are muted, and held
cells use yellow backgrounds with black glyphs. Battery cards move up 5 px to
make room for the original 33 px symbol-font line box.

The renderers use original Impact assets already in this repository,
with the existing FR Regular 36 as an intermediate long-name size. Background
hatches use LVGL triangle drawing; no large raster texture or transform buffer
is allocated. Actual firmware Flash/RAM impact remains to be measured by a
confirmed build. Host tests cover five-way DYA selection, 50 forward/backward
switches, rounded geometry, parent clipping, text overlap, 1%/100% batteries,
long names, and left/right modifier state colours.

DYA value 4 maps to local renderer ID 5. The external ms2 module's enum is not
modified. Its fixed-default Kconfig range remains unchanged; INDUSTRIAL is
selected through DYA and persisted by Custom Settings, not a new fixed shield.
No Actions build is dispatched merely by adding this theme.

The latest INDUSTRIAL revision uses the short CORNE header, 32 px WPM digits
with a 20 px unit label, and 32 px battery percentages beside 32x24 px battery
icons. The L/R captions are removed; left/right placement keeps their identity.
The two battery cards are 120x42 and retain enough room for 100%. Numeric glyphs
are a 14-character, 2298-byte 4 bpp subset derived reproducibly from the existing
Impact 56 asset by display-themes/scripts/make-industrial-fonts.py. No font scaling
layer is allocated at runtime.

The V3 composition widens the main 56 px display glyphs by 1.70x at generation
time, uses a 28 px CORNE brand, 30x30 eyes and a 54x30 USB badge. Modifier cells
are 48 px wide. These generated fonts require no runtime transform framebuffer.
The native suite passed 120 states and 1199 foreground rectangle checks with
zero unsafe, clipped or overlapping elements. This is host validation only.

## Latest source changes

INDUSTRIAL uses 60x24 battery bars, narrow 32 px battery numbers without percent
signs, and shortened stock BASE/LOWER/RAISE labels. WPM keeps its wider digits.
