# ZMK Prospector Themes

Receiver-only display themes for the Prospector dongle. This branch contains no keyboard keymap,
macro, encoder, RGB, or power-management configuration.

## Layout

Each theme owns a directory below `themes/`:

```text
themes/
  walle/
    README.md
    src/
      theme.c
    fonts/       # optional
    assets/      # optional
```

The matching entry in `boards/shields/` only enables that theme during a receiver build.

## Add the module

Add this project to the keyboard repository's `config/west.yml`:

```yaml
remotes:
  - name: s7venyoung
    url-base: https://github.com/S7venYoung

projects:
  - name: zmk-prospector-themes
    remote: s7venyoung
    repo-path: zmk-prospector
    revision: prospector-themes
    path: modules/zmk-prospector-themes
```

## Select a theme

Append the theme shield only to the Prospector receiver build:

```yaml
shield: eyelash_sofle_prospector_dongle prospector_adapter prospector_theme_walle
```

Remove `prospector_theme_walle` to build the receiver with its normal display.

## Display geometry

The Waveshare panel is 240x280 physically and becomes a 280x240 LVGL canvas after rotation.
Its four display corners are R5 mm (about 43 pixels at 0.11655 mm/pixel). Themes may paint
backgrounds to the canvas edge, but foreground content must respect the rounded-corner safe area.
