import * as _m0 from "protobufjs/minimal";
export declare const protobufPackage = "zmk.core";
export declare enum LockState {
    ZMK_STUDIO_CORE_LOCK_STATE_LOCKED = 0,
    ZMK_STUDIO_CORE_LOCK_STATE_UNLOCKED = 1,
    UNRECOGNIZED = -1
}
export declare function lockStateFromJSON(object: any): LockState;
export declare function lockStateToJSON(object: LockState): string;
export interface Request {
    getDeviceInfo?: boolean | undefined;
    getLockState?: boolean | undefined;
    lock?: boolean | undefined;
    resetSettings?: boolean | undefined;
}
export interface Response {
    getDeviceInfo?: GetDeviceInfoResponse | undefined;
    getLockState?: LockState | undefined;
    resetSettings?: boolean | undefined;
}
export interface GetDeviceInfoResponse {
    name: string;
    serialNumber: Uint8Array;
}
export interface Notification {
    lockStateChanged?: LockState | undefined;
}
export declare const Request: {
    encode(message: Request, writer?: _m0.Writer): _m0.Writer;
    decode(input: _m0.Reader | Uint8Array, length?: number): Request;
    fromJSON(object: any): Request;
    toJSON(message: Request): unknown;
    create<I extends {
        getDeviceInfo?: boolean | undefined;
        getLockState?: boolean | undefined;
        lock?: boolean | undefined;
        resetSettings?: boolean | undefined;
    } & {
        getDeviceInfo?: boolean | undefined;
        getLockState?: boolean | undefined;
        lock?: boolean | undefined;
        resetSettings?: boolean | undefined;
    } & { [K in Exclude<keyof I, keyof Request>]: never; }>(base?: I | undefined): Request;
    fromPartial<I_1 extends {
        getDeviceInfo?: boolean | undefined;
        getLockState?: boolean | undefined;
        lock?: boolean | undefined;
        resetSettings?: boolean | undefined;
    } & {
        getDeviceInfo?: boolean | undefined;
        getLockState?: boolean | undefined;
        lock?: boolean | undefined;
        resetSettings?: boolean | undefined;
    } & { [K_1 in Exclude<keyof I_1, keyof Request>]: never; }>(object: I_1): Request;
};
export declare const Response: {
    encode(message: Response, writer?: _m0.Writer): _m0.Writer;
    decode(input: _m0.Reader | Uint8Array, length?: number): Response;
    fromJSON(object: any): Response;
    toJSON(message: Response): unknown;
    create<I extends {
        getDeviceInfo?: {
            name?: string | undefined;
            serialNumber?: Uint8Array | undefined;
        } | undefined;
        getLockState?: LockState | undefined;
        resetSettings?: boolean | undefined;
    } & {
        getDeviceInfo?: ({
            name?: string | undefined;
            serialNumber?: Uint8Array | undefined;
        } & {
            name?: string | undefined;
            serialNumber?: Uint8Array | undefined;
        } & { [K in Exclude<keyof I["getDeviceInfo"], keyof GetDeviceInfoResponse>]: never; }) | undefined;
        getLockState?: LockState | undefined;
        resetSettings?: boolean | undefined;
    } & { [K_1 in Exclude<keyof I, keyof Response>]: never; }>(base?: I | undefined): Response;
    fromPartial<I_1 extends {
        getDeviceInfo?: {
            name?: string | undefined;
            serialNumber?: Uint8Array | undefined;
        } | undefined;
        getLockState?: LockState | undefined;
        resetSettings?: boolean | undefined;
    } & {
        getDeviceInfo?: ({
            name?: string | undefined;
            serialNumber?: Uint8Array | undefined;
        } & {
            name?: string | undefined;
            serialNumber?: Uint8Array | undefined;
        } & { [K_2 in Exclude<keyof I_1["getDeviceInfo"], keyof GetDeviceInfoResponse>]: never; }) | undefined;
        getLockState?: LockState | undefined;
        resetSettings?: boolean | undefined;
    } & { [K_3 in Exclude<keyof I_1, keyof Response>]: never; }>(object: I_1): Response;
};
export declare const GetDeviceInfoResponse: {
    encode(message: GetDeviceInfoResponse, writer?: _m0.Writer): _m0.Writer;
    decode(input: _m0.Reader | Uint8Array, length?: number): GetDeviceInfoResponse;
    fromJSON(object: any): GetDeviceInfoResponse;
    toJSON(message: GetDeviceInfoResponse): unknown;
    create<I extends {
        name?: string | undefined;
        serialNumber?: Uint8Array | undefined;
    } & {
        name?: string | undefined;
        serialNumber?: Uint8Array | undefined;
    } & { [K in Exclude<keyof I, keyof GetDeviceInfoResponse>]: never; }>(base?: I | undefined): GetDeviceInfoResponse;
    fromPartial<I_1 extends {
        name?: string | undefined;
        serialNumber?: Uint8Array | undefined;
    } & {
        name?: string | undefined;
        serialNumber?: Uint8Array | undefined;
    } & { [K_1 in Exclude<keyof I_1, keyof GetDeviceInfoResponse>]: never; }>(object: I_1): GetDeviceInfoResponse;
};
export declare const Notification: {
    encode(message: Notification, writer?: _m0.Writer): _m0.Writer;
    decode(input: _m0.Reader | Uint8Array, length?: number): Notification;
    fromJSON(object: any): Notification;
    toJSON(message: Notification): unknown;
    create<I extends {
        lockStateChanged?: LockState | undefined;
    } & {
        lockStateChanged?: LockState | undefined;
    } & { [K in Exclude<keyof I, "lockStateChanged">]: never; }>(base?: I | undefined): Notification;
    fromPartial<I_1 extends {
        lockStateChanged?: LockState | undefined;
    } & {
        lockStateChanged?: LockState | undefined;
    } & { [K_1 in Exclude<keyof I_1, "lockStateChanged">]: never; }>(object: I_1): Notification;
};
type Builtin = Date | Function | Uint8Array | string | number | boolean | undefined;
export type DeepPartial<T> = T extends Builtin ? T : T extends globalThis.Array<infer U> ? globalThis.Array<DeepPartial<U>> : T extends ReadonlyArray<infer U> ? ReadonlyArray<DeepPartial<U>> : T extends {} ? {
    [K in keyof T]?: DeepPartial<T[K]>;
} : Partial<T>;
type KeysOfUnion<T> = T extends T ? keyof T : never;
export type Exact<P, I extends P> = P extends Builtin ? P : P & {
    [K in keyof P]: Exact<P[K], I[K]>;
} & {
    [K in Exclude<keyof I, KeysOfUnion<P>>]: never;
};
export {};
