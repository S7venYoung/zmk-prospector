#!/usr/bin/env bash
set -euo pipefail
if [[ $# -lt 1 || $# -gt 2 || ( $# -eq 2 && $2 != --dry-run ) ]]; then
  echo "Usage: $0 /path/to/west-workspace [--dry-run]" >&2; exit 2
fi
workspace=$(cd -- "$1" && pwd)
repo=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)
module="$workspace/prospector-zmk-module"
for required in "$workspace/zmk/app/include/zmk/studio/custom.h" \
    "$module/boards/kblade/board.yml" "$module/boards/shields/prospector_ms2/prospector_ms2.overlay"; do
  [[ -f "$required" ]] || { echo "Missing pinned dependency: $required" >&2; exit 1; }
done
command=(west build -s "$workspace/zmk/app" -d "$workspace/build-ms2-scanner"
  -b kblade//zmk -S studio-rpc-usb-uart -S zmk-usb-logging --
  "-DSHIELD=prospector_ms2 scanner_display"
  "-DZMK_CONFIG=$repo/config/scanner"
  "-DZMK_EXTRA_MODULES=$module;$repo")
if [[ ${2:-} == --dry-run ]]; then printf '%q ' "${command[@]}";printf '\n';exit 0;fi
cd -- "$workspace"
"${command[@]}"
