import * as _m0 from "protobufjs/minimal";
export declare const protobufPackage = "zmk.meta";
export declare enum ErrorConditions {
    GENERIC = 0,
    UNLOCK_REQUIRED = 1,
    RPC_NOT_FOUND = 2,
    MSG_DECODE_FAILED = 3,
    MSG_ENCODE_FAILED = 4,
    UNRECOGNIZED = -1
}
export declare function errorConditionsFromJSON(object: any): ErrorConditions;
export declare function errorConditionsToJSON(object: ErrorConditions): string;
export interface Response {
    noResponse?: boolean | undefined;
    simpleError?: ErrorConditions | undefined;
}
export declare const Response: {
    encode(message: Response, writer?: _m0.Writer): _m0.Writer;
    decode(input: _m0.Reader | Uint8Array, length?: number): Response;
    fromJSON(object: any): Response;
    toJSON(message: Response): unknown;
    create<I extends {
        noResponse?: boolean | undefined;
        simpleError?: ErrorConditions | undefined;
    } & {
        noResponse?: boolean | undefined;
        simpleError?: ErrorConditions | undefined;
    } & { [K in Exclude<keyof I, keyof Response>]: never; }>(base?: I | undefined): Response;
    fromPartial<I_1 extends {
        noResponse?: boolean | undefined;
        simpleError?: ErrorConditions | undefined;
    } & {
        noResponse?: boolean | undefined;
        simpleError?: ErrorConditions | undefined;
    } & { [K_1 in Exclude<keyof I_1, keyof Response>]: never; }>(object: I_1): Response;
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
