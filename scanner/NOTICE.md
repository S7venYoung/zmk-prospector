# Receiver source provenance

`src/status_scanner.c`, `src/scanner_core.c` and the three headers under
`include/zmk/` are copied from S7venYoung/prospector-zmk-module at
`de4b078856cd50fb3b01a69fb6e1c13c54230286`, the fixed receiver revision used by
the existing zmk-prospector scanner-mode configuration. Existing contributor
copyright and MIT SPDX headers are preserved. The ms2 bridge and standalone
configuration are new adapters. Display code and fonts retain the notices under
`display-themes/`.

The copied core casts keyboard ID bytes to uint32_t before shifting, avoiding
signed-shift undefined behaviour for IDs whose high byte is >= 0x80. The receiver
host test exercises such an ID under UBSan.
