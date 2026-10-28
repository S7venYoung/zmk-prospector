#!/usr/bin/env bash
# Complete DYA target; invoke manually only after reviewing the configuration.
set -euo pipefail
if [[ $# -lt 1 || $# -gt 2 || ( $# -eq 2 && $2 != --dry-run ) ]]; then
  echo "Usage: $0 /path/to/west-workspace [--dry-run]" >&2; exit 2
fi
workspace=$(cd -- "$1" && pwd)
repo=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)
module="$repo/hardware/prospector-zmk-module"
for required in "$workspace/zmk/app/include/zmk/studio/custom.h" \
    "$module/boards/kblade/board.yml" \
    "$module/boards/shields/prospector_ms2/prospector_ms2.overlay"; do
  if [[ ! -f "$required" ]]; then
    echo "Missing DYA/hardware dependency: $required. Initialize west from this repository's west.yml." >&2
    exit 1
  fi
done
command=(west build -s "$workspace/zmk/app" -d "$workspace/build-ms2-dya"
  -b kblade//zmk -S studio-rpc-usb-uart -S zmk-usb-logging --
  "-DSHIELD=prospector_ms2 corne_dongle"
  "-DZMK_CONFIG=$repo/config"
  "-DZMK_EXTRA_MODULES=$module;$repo"
  -DCONFIG_ZMK_BLE=y -DCONFIG_ZMK_STUDIO=y
  -DCONFIG_ZMK_PROSPECTOR_DYA_SETTINGS=y
  -DCONFIG_PROSPECTOR_DISPLAY_LOG_PAGE=y
  '-DCONFIG_USB_DEVICE_PRODUCT="DongleProspector"'
  -DCONFIG_LOG_DEFAULT_LEVEL=2)
if [[ ${2:-} == --dry-run ]]; then printf '%q ' "${command[@]}";printf '\n';exit 0;fi
cd -- "$workspace"
"${command[@]}"
