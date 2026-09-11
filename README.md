# ZMK Prospector Themes

Receiver-only display themes for the Prospector dongle. This branch contains no keyboard keymap,
macro, encoder, RGB, or power-management configuration.

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
