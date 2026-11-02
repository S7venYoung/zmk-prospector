# Enter Bootloader from DYA

Reference: [zmk-elashvelvet nano-dongle](https://github.com/S7venYoung/zmk-elashvelvet/tree/codex/nano-dongle).

Select a valid key in DYA, then choose **Bootloader**. The scanner exposes one placeholder key on its DISPLAY layer; the dongle can use any valid key. The connected device accepts this as a transient administrative action, waits 500ms for the RPC response, then enters its own UF2 bootloader. No physical key press is needed.

This action does not write the selected binding or erase settings, host profiles or split bonds. It affects the connected scanner/dongle only, not the keyboard halves.

## Build integration

Both branches enable `CONFIG_PROSPECTOR_DYA_BOOTLOADER_ACTION=y`. The scanner script `scripts/build-ms2-scanner.sh` and dongle script `scripts/build-ms2-dya.sh` install a guarded action hook in the pinned `zmk/app/src/keymap.c` before invoking west. Repeated application is safe. Unknown upstream code or a partial hook stops the build. `--dry-run` does not modify dependency sources.

When invoking west directly instead of those scripts, first run:

```sh
python3 scripts/patch_dya_bootloader_action.py /path/to/workspace/zmk/app/src/keymap.c
```

## Validation

`python3 scripts/test_dya_bootloader_action.py` compiles the real action and patched setter against mocked work/bootmode/reboot APIs. It covers delayed execution, no binding persistence, duplicate suppression, failures and recovery, ordinary edits and patch guards. The patch was also applied twice to the actual keymap source from pinned ZMK commit `e5c9b6915b56801193e359dd9bad4a167ce0d1b8`.

These are host checks. Firmware build and physical DYA-to-BL acceptance remain pending.
