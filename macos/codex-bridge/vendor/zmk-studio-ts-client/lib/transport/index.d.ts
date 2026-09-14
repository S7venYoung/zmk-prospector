export interface RpcTransport {
    label: string;
    abortController: AbortController;
    readable: ReadableStream<Uint8Array>;
    writable: WritableStream<Uint8Array>;
}
