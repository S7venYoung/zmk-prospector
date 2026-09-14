import "./style.css";
import { create_rpc_connection, call_rpc } from "@zmkfirmware/zmk-studio-ts-client";
import { connect } from "@zmkfirmware/zmk-studio-ts-client/transport/serial";

const SUBSYSTEM = "s7venyoung__codex_metrics";
let connection;
let subsystemIndex;
let timer;

function withTimeout(promise, label, ms = 8000) {
  let timeout;
  const guard = new Promise((_, reject) => {
    timeout = setTimeout(() => reject(new Error(`${label}超时，请确认接收器已连接且 DYA 已退出`)), ms);
  });
  return Promise.race([promise, guard]).finally(() => clearTimeout(timeout));
}

const app = document.querySelector("#app");
app.innerHTML = `
  <header><span class="eyes">● ●</span><h1>CODEX // PROSPECTOR</h1></header>
  <section class="metric"><label>5 HOUR USED</label><strong id="used">--%</strong></section>
  <section class="metric token"><label>TODAY TOTAL TOKEN</label><strong id="tokens">--</strong></section>
  <p id="status">未连接接收器</p>
  <button id="connect">连接并同步</button>
  <small>DYA Studio 使用同一串口；连接本程序前请先退出 DYA。</small>`;

const status = document.querySelector("#status");
const button = document.querySelector("#connect");

function varint(value) {
  let n = BigInt(value);
  const out = [];
  do { let byte = Number(n & 0x7fn); n >>= 7n; if (n) byte |= 0x80; out.push(byte); } while (n);
  return out;
}
function field(tag, value) { return [...varint(tag << 3), ...varint(value)]; }
function message(tag, bytes) { return [...varint((tag << 3) | 2), ...varint(bytes.length), ...bytes]; }
function encodeMetrics(metrics) {
  const body = [
    ...field(1, metrics.usedPercent),
    ...field(2, metrics.totalTokens),
    ...field(3, metrics.updatedAt)
  ];
  return new Uint8Array(message(1, message(1, body)));
}
function compact(tokens) {
  if (tokens >= 1_000_000) return `${(tokens / 1_000_000).toFixed(1)}M`;
  if (tokens >= 1_000) return `${(tokens / 1_000).toFixed(1)}K`;
  return String(tokens);
}

async function sync() {
  const metrics = await window.codexBridge.readMetrics();
  document.querySelector("#used").textContent = metrics.usedPercent == null ? "--%" : `${metrics.usedPercent}%`;
  document.querySelector("#tokens").textContent = compact(metrics.totalTokens);
  if (!connection || subsystemIndex == null || metrics.usedPercent == null) return;
  const response = await withTimeout(call_rpc(connection, { custom: { call: {
    subsystemIndex, payload: encodeMetrics(metrics)
  } } }), "同步 Codex 数据");
  if (!response.custom?.call) throw new Error("接收器未确认数据");
  status.textContent = `已同步 · ${new Date().toLocaleTimeString()}`;
}

button.addEventListener("click", async () => {
  button.disabled = true;
  status.textContent = "正在连接…";
  let transport;
  try {
    status.textContent = "请选择 Prospector 接收器…";
    transport = await withTimeout(connect(), "串口连接");
    connection = create_rpc_connection(transport);
    const listed = await withTimeout(
      call_rpc(connection, { custom: { listCustomSubsystems: {} } }),
      "读取接收器能力"
    );
    const item = listed.custom?.listCustomSubsystems?.subsystems.find((x) => x.identifier === SUBSYSTEM);
    if (!item) throw new Error("固件没有 Codex metrics 子系统");
    subsystemIndex = item.index;
    await sync();
    clearInterval(timer);
    timer = setInterval(() => sync().catch((error) => status.textContent = error.message), 30000);
    button.textContent = "已连接";
  } catch (error) {
    if (transport?.abortController) {
      try { transport.abortController.abort(error); } catch {}
    }
    connection = undefined;
    subsystemIndex = undefined;
    const message = error?.message || String(error);
    status.textContent = /already open|in use|占用/i.test(message)
      ? "串口已被占用，请退出 DYA 和其他 Prospector Codex 实例后重试"
      : message;
    button.disabled = false;
  }
});

sync().catch(() => {});
