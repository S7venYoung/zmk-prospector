const { app, BrowserWindow, ipcMain, session } = require("electron");
const fs = require("node:fs");
const os = require("node:os");
const path = require("node:path");

function dayPath(date) {
  const part = (n) => String(n).padStart(2, "0");
  return path.join(os.homedir(), ".codex", "sessions", String(date.getFullYear()),
    part(date.getMonth() + 1), part(date.getDate()));
}

function jsonLines(file) {
  try {
    return fs.readFileSync(file, "utf8").split("\n").filter(Boolean);
  } catch {
    return [];
  }
}

function readMetrics() {
  const now = new Date();
  const midnight = new Date(now.getFullYear(), now.getMonth(), now.getDate()).toISOString();
  let totalTokens = 0;
  const today = dayPath(now);
  if (fs.existsSync(today)) {
    for (const name of fs.readdirSync(today).filter((x) => x.endsWith(".jsonl"))) {
      for (const line of jsonLines(path.join(today, name))) {
        if (!line.includes('"token_count"')) continue;
        try {
          const event = JSON.parse(line);
          if ((event.timestamp || "") < midnight || event.payload?.type !== "token_count") continue;
          const usage = event.payload?.info?.last_token_usage;
          if (!usage) continue;
          totalTokens += Number(usage.input_tokens || 0) + Number(usage.cache_write_input_tokens || 0)
            + Number(usage.output_tokens || 0);
        } catch {}
      }
    }
  }

  let newest = "";
  let usedPercent = null;
  for (let back = 0; back < 4; back++) {
    const date = new Date(now);
    date.setDate(date.getDate() - back);
    const dir = dayPath(date);
    if (!fs.existsSync(dir)) continue;
    for (const name of fs.readdirSync(dir).filter((x) => x.endsWith(".jsonl"))) {
      for (const line of jsonLines(path.join(dir, name))) {
        if (!line.includes('"used_percent"')) continue;
        try {
          const event = JSON.parse(line);
          const used = event.payload?.rate_limits?.primary?.used_percent;
          if (typeof used === "number" && (event.timestamp || "") > newest) {
            newest = event.timestamp;
            usedPercent = Math.max(0, Math.min(100, Math.round(used)));
          }
        } catch {}
      }
    }
  }
  return { usedPercent, totalTokens, updatedAt: Math.floor(Date.now() / 1000) };
}

function createWindow() {
  const win = new BrowserWindow({
    width: 460,
    height: 560,
    minWidth: 420,
    minHeight: 500,
    title: "Prospector Codex",
    webPreferences: { preload: path.join(__dirname, "preload.cjs"), contextIsolation: true }
  });
  win.loadFile(path.join(__dirname, "..", "dist", "index.html"));
}

app.whenReady().then(() => {
  session.defaultSession.setPermissionCheckHandler((_wc, permission) => permission === "serial");
  session.defaultSession.setDevicePermissionHandler((details) => details.deviceType === "serial");
  session.defaultSession.on("select-serial-port", (event, ports, _wc, callback) => {
    event.preventDefault();
    const usb = ports.find((p) => /usbmodem|usbserial|usb.*serial/i.test(`${p.displayName} ${p.portName}`));
    const named = ports.find((p) => /prospector|zmk|xiao|nice.?nano/i.test(`${p.displayName} ${p.portName}`));
    const safe = ports.find((p) => !/bluetooth|debug|wlan/i.test(`${p.displayName} ${p.portName}`));
    callback((usb || named || safe)?.portId || "");
  });
  ipcMain.handle("codex:metrics", () => readMetrics());
  createWindow();
});

app.on("window-all-closed", () => app.quit());
