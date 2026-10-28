#!/usr/bin/env bash
set -euo pipefail
here=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)
if [[ $# -ne 2 ]]; then echo "Usage: $0 LVGL_SOURCE KBLADE_MODULE" >&2; exit 2; fi
lvgl=$(cd -- "$1" && pwd)
hardware=$(cd -- "$2" && pwd)
out=$(mktemp -d)
trap 'rm -rf "$out"' EXIT
clang -std=c11 -g -fsanitize=address,undefined -fno-omit-frame-pointer \
  -DLV_CONF_SKIP -DCONFIG_ZMK_PROSPECTOR_DYA_SETTINGS=1 -DCONFIG_ZMK_LOG_LEVEL=1 -DCONFIG_PROSPECTOR_MODE_SCANNER=1 \
  -DCONFIG_ZMK_BATTERY_REPORTING=0 -DCONFIG_USB_DEVICE_STACK=0 \
  -DCONFIG_PROSPECTOR_MAX_KEYBOARDS=3 -DCONFIG_PROSPECTOR_SCANNER_TIMEOUT_MS=5000 \
  -I"$here/stubs" -I"$here/../include" -I"$lvgl" -I"$lvgl/src" \
  -I"$hardware/boards/shields/src" -I"$here/../../display-themes/src" \
  "$here/receiver_test.c" "$here/../src/ms2_scanner_bridge.c" -o "$out/receiver-test"
"$out/receiver-test"
