import { UserCancelledError } from './errors';
const SERVICE_UUID = '00000000-0196-6107-c967-c5cfb1c2482a';
const RPC_CHRC_UUID = '00000001-0196-6107-c967-c5cfb1c2482a';
class Deferred {
    promise;
    resolve;
    reject;
    constructor() {
        this.promise = new Promise((res, rej) => {
            this.resolve = res;
            this.reject = rej;
        });
    }
}
export async function connect(options = {}) {
    const isBluefy = (() => {
        try {
            return navigator.userAgent.includes('Bluefy');
        }
        catch (e) {
            console.warn('Error detecting user agent for Bluefy UUID case sensitivity check:', e);
            return false;
        }
    })();
    const serviceUUID = isBluefy ? SERVICE_UUID.toUpperCase() : SERVICE_UUID;
    const rpcChrcUUID = isBluefy ? RPC_CHRC_UUID.toUpperCase() : RPC_CHRC_UUID;
    const filters = isBluefy ? [{ services: ['battery_service'] }] : [{ services: [serviceUUID] }, { services: ['battery_service'] }];
    let dev = await navigator.bluetooth.requestDevice({
        optionalServices: [serviceUUID],
        // only either acceptAllDevices or filters can be provided
        ...('acceptAllDevices' in options || 'filters' in options) ? options : { filters, ...options },
    }).catch((e) => {
        if (e instanceof DOMException && e.name == "NotFoundError") {
            throw new UserCancelledError("User cancelled the connection attempt", { cause: e });
        }
        else {
            throw e;
        }
    });
    if (!dev.gatt) {
        filters: {
            throw 'No GATT service!';
        }
    }
    let abortController = new AbortController();
    let label = dev.name || 'Unknown';
    if (!dev.gatt.connected) {
        await dev.gatt.connect();
    }
    let svc = await dev.gatt.getPrimaryService(serviceUUID);
    let char = await svc.getCharacteristic(rpcChrcUUID);
    let onStart = new Deferred();
    let readable = new ReadableStream({
        async start(controller) {
            // Reconnect to the same device will lose notifications if we don't first force a stop before starting again.      
            try {
                await char.stopNotifications();
                await char.startNotifications();
                onStart.resolve();
            }
            catch (e) {
                onStart.reject(e);
                throw e;
            }
            let vc = (ev) => {
                let buf = ev.target?.value
                    ?.buffer;
                if (!buf) {
                    return;
                }
                controller.enqueue(new Uint8Array(buf));
            };
            char.addEventListener('characteristicvaluechanged', vc);
            let cb = async () => {
                char.removeEventListener('characteristicvaluechanged', vc);
                dev.removeEventListener('gattserverdisconnected', cb);
                controller.close();
            };
            dev.addEventListener('gattserverdisconnected', cb);
        },
    });
    let writable = new WritableStream({
        async write(chunk) {
            await onStart.promise;
            // Further investigation required for best approach
            // writeValueWithoutResponse might work by chunking into MTU sizes for bluefy
            if (!isBluefy && char.properties.writeWithoutResponse) {
                return await char.writeValueWithoutResponse(chunk);
            }
            const mtu = 20;
            for (let i = 0; i < chunk.length; i += mtu) {
                let end = Math.min(i + mtu, chunk.length);
                let slice = chunk.slice(i, end);
                await char.writeValue(slice);
            }
            return;
        },
    });
    let sig = abortController.signal;
    let abort_cb;
    abort_cb = async (ev) => {
        sig.removeEventListener("abort", abort_cb);
        dev.gatt?.disconnect();
    };
    sig.addEventListener("abort", abort_cb);
    return { label, abortController, readable, writable };
}
