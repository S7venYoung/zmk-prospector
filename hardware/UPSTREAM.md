# Local kblade / ms2 hardware definitions

`prospector-zmk-module/` is a vendored source snapshot of
`kbladewht/prospector-zmk-module@37146e417cd95ec95ab144ed9ebb6632da06bd8e`.
It includes the kblade board, ms2 shield, display/backlight drivers, shared fonts,
DTS bindings and compatibility headers. The upstream LICENSE and per-file notices
are retained. Upstream source is copied without hardware/pin/partition changes.

The board is defined at `prospector-zmk-module/boards/kblade/` and still builds as
`kblade//zmk`. There is no west project fetching this hardware repository.
Both build scripts add the local module through ZMK_EXTRA_MODULES. Do not add a
second remote prospector-zmk-module project to this workspace: that would duplicate
board/module registration.

The normal Zephyr/ZMK/SDK dependencies remain external; this snapshot replaces the
hardware project only. The source commit above is provenance, not a build fetch.

Local adaptation: when DYA display settings are enabled, its brightness is the
single owner. The stock screen no longer applies its NVS manual brightness,
1% no-keyboard dimming, or fixed-brightness resume override. Backlight boot PWM
matches DYA's default 60% with the ms2 inverted duty mapping. Board/pin/partition
files remain unchanged.
