# Codex Status

A Codex-first dashboard for the rotated 280x240 Prospector display.

The five-hour remaining percentage is the main visual focus, followed by today's total token count.
Keyboard information is condensed into the lower status area: active layer, WPM, left/right
battery and split connection state. Codex is intentionally limited to those two metrics.

Select it by adding `prospector_theme_codex_status` to the receiver's shield list. Do not select it
together with another Prospector theme.

The receiver exposes the unsecured `s7venyoung__codex_metrics` Studio custom subsystem. Values are
kept in RAM and refreshed by the standalone macOS bridge; nothing is written to NVS.
