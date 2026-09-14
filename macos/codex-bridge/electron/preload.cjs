const { contextBridge, ipcRenderer } = require("electron");

contextBridge.exposeInMainWorld("codexBridge", {
  readMetrics: () => ipcRenderer.invoke("codex:metrics")
});
