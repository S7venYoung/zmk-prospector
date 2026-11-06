# MS18 twelve-theme touch scanner

Target: `kblade//zmk`, `prospector_ms18 scanner_display`. Standalone BLE observer.
Display: ST7789, 240×280 portrait / 280×240 landscape. Touch: CST816T,
using Zephyr CST816-family driver (hynitron,cst816s); hardware compatibility remains unverified.

MS18 touch wiring:

| Signal | MCU pin |
|---|---|
| TP_SCL | P0.08 |
| TP_SDA | P0.06 |
| TP_RST | P0.04 |
| TP_INT | P0.29 |

The CST816T uses Zephyr's CST816-family driver (`hynitron,cst816s`) at I2C
address `0x15`. Display wiring retains the MS18 overlay.

On the theme page, swipe down/up for next/previous theme (all twelve wrap).
Double tap cycles palette on FIELD/OPERATOR/RADII. Swipe right opens a compact
navigation page; swipe down opens brightness settings. Drag the slider to set
1–100%; swipe up returns to navigation, then left returns to themes.
This port preserves scanner-mode's gesture directions and coordinate rotation.
The overlay is laid out for the complete 280x240 landscape viewport: the
navigation cards use symmetric 24-pixel margins, the brightness percentage and
slider are centered, and the slider has an extended touch area.
Touch and DYA share setting values; changes are saved after two seconds of inactivity.

Build with `bash scripts/build-ms18-scanner.sh /path/to/west-workspace`, or run
the manual Actions workflow on `kl-touch`. No automatic flashing.

Validation before this sync: 288 host layout states, zero unsafe/clipped/overlapping
rectangles; receiver tests passed. Firmware compilation and physical touch testing
have not yet completed. Do not treat these source checks as device validation.
