#!/usr/bin/env bash
# Run manually in the kbladewht workspace; this script never flashes a device.
set -euo pipefail
if [[ $# -lt 2 || $# -gt 3 ]]; then
  echo "Usage: $0 /path/to/kbladewht-zmk field|operator|radii|yads [--dry-run]" >&2
  exit 2
fi
workspace=$(cd -- "$1" && pwd)
theme=$2
if [[ $# -eq 3 && $3 != --dry-run ]]; then echo "Unknown option: $3" >&2; exit 2; fi
case "$theme" in field|operator|radii|yads) ;; *) echo "Unknown theme: $theme" >&2; exit 2 ;; esac
themes_dir=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)
module_dir="$workspace/prospector-zmk-module"
for required in "$workspace/app/CMakeLists.txt" "$module_dir/boards/kblade/board.yml" \
    "$module_dir/boards/shields/prospector_ms2/prospector_ms2.overlay" \
    "$module_dir/boards/shields/src/s7789_update.h"; do
  if [[ ! -f "$required" ]]; then echo "Missing target definition: $required" >&2; exit 1; fi
done
export Zephyr_DIR="${Zephyr_DIR:-$workspace/zephyr/share/zephyr-package/cmake}"
command=(west build -s "$workspace/app" -d "$workspace/app/build-ms2-$theme"
  -b kblade//zmk -S zmk-usb-logging --
  "-DSHIELD=prospector_ms2 corne_dongle prospector_theme_$theme"
  -DCONFIG_ZMK_BLE=y -DCONFIG_PROSPECTOR_DISPLAY_LOG_PAGE=y
  '-DCONFIG_USB_DEVICE_PRODUCT="DongleProspector"'
  -DCONFIG_LOG_DEFAULT_LEVEL=2
  "-DZMK_EXTRA_MODULES=$module_dir;$themes_dir")
if [[ ${3:-} == --dry-run ]]; then printf '%q ' "${command[@]}"; printf '\n'; exit 0; fi
cd -- "$workspace"
"${command[@]}"
