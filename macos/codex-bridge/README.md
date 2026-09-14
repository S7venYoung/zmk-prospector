# Prospector Codex Bridge for macOS

Standalone macOS desktop bridge. It reads aggregate data from local Codex JSONL sessions and sends
only the five-hour used percentage, today's total token count, and an update timestamp to the
Prospector receiver over its existing ZMK Studio USB serial transport.

```sh
npm install
npm run build
npm start
```

Flash a receiver firmware containing `prospector_theme_codex_status` first. Quit DYA Studio before
connecting because both applications use the same serial device. No API key, session text, prompt,
or response content is transmitted to the receiver.
