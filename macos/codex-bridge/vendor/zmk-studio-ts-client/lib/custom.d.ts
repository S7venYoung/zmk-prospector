import * as _m0 from "protobufjs/minimal";
export declare const protobufPackage = "zmk.custom";
export interface ListCustomSubsystemRequest {
}
export interface ListCustomSubsystemResponse {
    subsystems: CustomSubsystemInfo[];
}
export interface CustomSubsystemInfo {
    /** Device specific subsystem index to identify the subsystem in request/response */
    index: number;
    /** Unique subsystem identifier */
    identifier: string;
    /** Subsystem UI URLs which supports interaction with the subsystem */
    uiUrl: string[];
}
export interface CallRequest {
    /**
     * target subsystem index to deliver the payload
     * The value is the same to CustomSubsystemInfo.index. The value can change per compile and maybe even at startup.
     */
    subsystemIndex: number;
    payload: Uint8Array;
}
export interface CallResponse {
    /**
     * target subsystem index to deliver the payload
     * The value is the same to CustomSubsystemInfo.index. The value can change per compile and maybe even at startup.
     */
    subsystemIndex: number;
    payload: Uint8Array;
}
export interface CustomNotification {
    /**
     * target subsystem index to deliver the payload
     * The value is the same to CustomSubsystemInfo.index. The value can change per compile and maybe even at startup.
     */
    subsystemIndex: number;
    payload: Uint8Array;
}
export interface Request {
    listCustomSubsystems?: ListCustomSubsystemRequest | undefined;
    call?: CallRequest | undefined;
}
export interface Response {
    listCustomSubsystems?: ListCustomSubsystemResponse | undefined;
    call?: CallResponse | undefined;
}
export interface Notification {
    customNotification?: CustomNotification | undefined;
}
export declare const ListCustomSubsystemRequest: {
    encode(_: ListCustomSubsystemRequest, writer?: _m0.Writer): _m0.Writer;
    decode(input: _m0.Reader | Uint8Array, length?: number): ListCustomSubsystemRequest;
    fromJSON(_: any): ListCustomSubsystemRequest;
    toJSON(_: ListCustomSubsystemRequest): unknown;
    create<I extends {} & {} & { [K in Exclude<keyof I, never>]: never; }>(base?: I | undefined): ListCustomSubsystemRequest;
    fromPartial<I_1 extends {} & {} & { [K_1 in Exclude<keyof I_1, never>]: never; }>(_: I_1): ListCustomSubsystemRequest;
};
export declare const ListCustomSubsystemResponse: {
    encode(message: ListCustomSubsystemResponse, writer?: _m0.Writer): _m0.Writer;
    decode(input: _m0.Reader | Uint8Array, length?: number): ListCustomSubsystemResponse;
    fromJSON(object: any): ListCustomSubsystemResponse;
    toJSON(message: ListCustomSubsystemResponse): unknown;
    create<I extends {
        subsystems?: {
            index?: number | undefined;
            identifier?: string | undefined;
            uiUrl?: string[] | undefined;
        }[] | undefined;
    } & {
        subsystems?: ({
            index?: number | undefined;
            identifier?: string | undefined;
            uiUrl?: string[] | undefined;
        }[] & ({
            index?: number | undefined;
            identifier?: string | undefined;
            uiUrl?: string[] | undefined;
        } & {
            index?: number | undefined;
            identifier?: string | undefined;
            uiUrl?: (string[] & string[] & { [K in Exclude<keyof I["subsystems"][number]["uiUrl"], keyof string[]>]: never; }) | undefined;
        } & { [K_1 in Exclude<keyof I["subsystems"][number], keyof CustomSubsystemInfo>]: never; })[] & { [K_2 in Exclude<keyof I["subsystems"], keyof {
            index?: number | undefined;
            identifier?: string | undefined;
            uiUrl?: string[] | undefined;
        }[]>]: never; }) | undefined;
    } & { [K_3 in Exclude<keyof I, "subsystems">]: never; }>(base?: I | undefined): ListCustomSubsystemResponse;
    fromPartial<I_1 extends {
        subsystems?: {
            index?: number | undefined;
            identifier?: string | undefined;
            uiUrl?: string[] | undefined;
        }[] | undefined;
    } & {
        subsystems?: ({
            index?: number | undefined;
            identifier?: string | undefined;
            uiUrl?: string[] | undefined;
        }[] & ({
            index?: number | undefined;
            identifier?: string | undefined;
            uiUrl?: string[] | undefined;
        } & {
            index?: number | undefined;
            identifier?: string | undefined;
            uiUrl?: (string[] & string[] & { [K_4 in Exclude<keyof I_1["subsystems"][number]["uiUrl"], keyof string[]>]: never; }) | undefined;
        } & { [K_5 in Exclude<keyof I_1["subsystems"][number], keyof CustomSubsystemInfo>]: never; })[] & { [K_6 in Exclude<keyof I_1["subsystems"], keyof {
            index?: number | undefined;
            identifier?: string | undefined;
            uiUrl?: string[] | undefined;
        }[]>]: never; }) | undefined;
    } & { [K_7 in Exclude<keyof I_1, "subsystems">]: never; }>(object: I_1): ListCustomSubsystemResponse;
};
export declare const CustomSubsystemInfo: {
    encode(message: CustomSubsystemInfo, writer?: _m0.Writer): _m0.Writer;
    decode(input: _m0.Reader | Uint8Array, length?: number): CustomSubsystemInfo;
    fromJSON(object: any): CustomSubsystemInfo;
    toJSON(message: CustomSubsystemInfo): unknown;
    create<I extends {
        index?: number | undefined;
        identifier?: string | undefined;
        uiUrl?: string[] | undefined;
    } & {
        index?: number | undefined;
        identifier?: string | undefined;
        uiUrl?: (string[] & string[] & { [K in Exclude<keyof I["uiUrl"], keyof string[]>]: never; }) | undefined;
    } & { [K_1 in Exclude<keyof I, keyof CustomSubsystemInfo>]: never; }>(base?: I | undefined): CustomSubsystemInfo;
    fromPartial<I_1 extends {
        index?: number | undefined;
        identifier?: string | undefined;
        uiUrl?: string[] | undefined;
    } & {
        index?: number | undefined;
        identifier?: string | undefined;
        uiUrl?: (string[] & string[] & { [K_2 in Exclude<keyof I_1["uiUrl"], keyof string[]>]: never; }) | undefined;
    } & { [K_3 in Exclude<keyof I_1, keyof CustomSubsystemInfo>]: never; }>(object: I_1): CustomSubsystemInfo;
};
export declare const CallRequest: {
    encode(message: CallRequest, writer?: _m0.Writer): _m0.Writer;
    decode(input: _m0.Reader | Uint8Array, length?: number): CallRequest;
    fromJSON(object: any): CallRequest;
    toJSON(message: CallRequest): unknown;
    create<I extends {
        subsystemIndex?: number | undefined;
        payload?: Uint8Array | undefined;
    } & {
        subsystemIndex?: number | undefined;
        payload?: Uint8Array | undefined;
    } & { [K in Exclude<keyof I, keyof CallRequest>]: never; }>(base?: I | undefined): CallRequest;
    fromPartial<I_1 extends {
        subsystemIndex?: number | undefined;
        payload?: Uint8Array | undefined;
    } & {
        subsystemIndex?: number | undefined;
        payload?: Uint8Array | undefined;
    } & { [K_1 in Exclude<keyof I_1, keyof CallRequest>]: never; }>(object: I_1): CallRequest;
};
export declare const CallResponse: {
    encode(message: CallResponse, writer?: _m0.Writer): _m0.Writer;
    decode(input: _m0.Reader | Uint8Array, length?: number): CallResponse;
    fromJSON(object: any): CallResponse;
    toJSON(message: CallResponse): unknown;
    create<I extends {
        subsystemIndex?: number | undefined;
        payload?: Uint8Array | undefined;
    } & {
        subsystemIndex?: number | undefined;
        payload?: Uint8Array | undefined;
    } & { [K in Exclude<keyof I, keyof CallResponse>]: never; }>(base?: I | undefined): CallResponse;
    fromPartial<I_1 extends {
        subsystemIndex?: number | undefined;
        payload?: Uint8Array | undefined;
    } & {
        subsystemIndex?: number | undefined;
        payload?: Uint8Array | undefined;
    } & { [K_1 in Exclude<keyof I_1, keyof CallResponse>]: never; }>(object: I_1): CallResponse;
};
export declare const CustomNotification: {
    encode(message: CustomNotification, writer?: _m0.Writer): _m0.Writer;
    decode(input: _m0.Reader | Uint8Array, length?: number): CustomNotification;
    fromJSON(object: any): CustomNotification;
    toJSON(message: CustomNotification): unknown;
    create<I extends {
        subsystemIndex?: number | undefined;
        payload?: Uint8Array | undefined;
    } & {
        subsystemIndex?: number | undefined;
        payload?: Uint8Array | undefined;
    } & { [K in Exclude<keyof I, keyof CustomNotification>]: never; }>(base?: I | undefined): CustomNotification;
    fromPartial<I_1 extends {
        subsystemIndex?: number | undefined;
        payload?: Uint8Array | undefined;
    } & {
        subsystemIndex?: number | undefined;
        payload?: Uint8Array | undefined;
    } & { [K_1 in Exclude<keyof I_1, keyof CustomNotification>]: never; }>(object: I_1): CustomNotification;
};
export declare const Request: {
    encode(message: Request, writer?: _m0.Writer): _m0.Writer;
    decode(input: _m0.Reader | Uint8Array, length?: number): Request;
    fromJSON(object: any): Request;
    toJSON(message: Request): unknown;
    create<I extends {
        listCustomSubsystems?: {} | undefined;
        call?: {
            subsystemIndex?: number | undefined;
            payload?: Uint8Array | undefined;
        } | undefined;
    } & {
        listCustomSubsystems?: ({} & {} & { [K in Exclude<keyof I["listCustomSubsystems"], never>]: never; }) | undefined;
        call?: ({
            subsystemIndex?: number | undefined;
            payload?: Uint8Array | undefined;
        } & {
            subsystemIndex?: number | undefined;
            payload?: Uint8Array | undefined;
        } & { [K_1 in Exclude<keyof I["call"], keyof CallRequest>]: never; }) | undefined;
    } & { [K_2 in Exclude<keyof I, keyof Request>]: never; }>(base?: I | undefined): Request;
    fromPartial<I_1 extends {
        listCustomSubsystems?: {} | undefined;
        call?: {
            subsystemIndex?: number | undefined;
            payload?: Uint8Array | undefined;
        } | undefined;
    } & {
        listCustomSubsystems?: ({} & {} & { [K_3 in Exclude<keyof I_1["listCustomSubsystems"], never>]: never; }) | undefined;
        call?: ({
            subsystemIndex?: number | undefined;
            payload?: Uint8Array | undefined;
        } & {
            subsystemIndex?: number | undefined;
            payload?: Uint8Array | undefined;
        } & { [K_4 in Exclude<keyof I_1["call"], keyof CallRequest>]: never; }) | undefined;
    } & { [K_5 in Exclude<keyof I_1, keyof Request>]: never; }>(object: I_1): Request;
};
export declare const Response: {
    encode(message: Response, writer?: _m0.Writer): _m0.Writer;
    decode(input: _m0.Reader | Uint8Array, length?: number): Response;
    fromJSON(object: any): Response;
    toJSON(message: Response): unknown;
    create<I extends {
        listCustomSubsystems?: {
            subsystems?: {
                index?: number | undefined;
                identifier?: string | undefined;
                uiUrl?: string[] | undefined;
            }[] | undefined;
        } | undefined;
        call?: {
            subsystemIndex?: number | undefined;
            payload?: Uint8Array | undefined;
        } | undefined;
    } & {
        listCustomSubsystems?: ({
            subsystems?: {
                index?: number | undefined;
                identifier?: string | undefined;
                uiUrl?: string[] | undefined;
            }[] | undefined;
        } & {
            subsystems?: ({
                index?: number | undefined;
                identifier?: string | undefined;
                uiUrl?: string[] | undefined;
            }[] & ({
                index?: number | undefined;
                identifier?: string | undefined;
                uiUrl?: string[] | undefined;
            } & {
                index?: number | undefined;
                identifier?: string | undefined;
                uiUrl?: (string[] & string[] & { [K in Exclude<keyof I["listCustomSubsystems"]["subsystems"][number]["uiUrl"], keyof string[]>]: never; }) | undefined;
            } & { [K_1 in Exclude<keyof I["listCustomSubsystems"]["subsystems"][number], keyof CustomSubsystemInfo>]: never; })[] & { [K_2 in Exclude<keyof I["listCustomSubsystems"]["subsystems"], keyof {
                index?: number | undefined;
                identifier?: string | undefined;
                uiUrl?: string[] | undefined;
            }[]>]: never; }) | undefined;
        } & { [K_3 in Exclude<keyof I["listCustomSubsystems"], "subsystems">]: never; }) | undefined;
        call?: ({
            subsystemIndex?: number | undefined;
            payload?: Uint8Array | undefined;
        } & {
            subsystemIndex?: number | undefined;
            payload?: Uint8Array | undefined;
        } & { [K_4 in Exclude<keyof I["call"], keyof CallResponse>]: never; }) | undefined;
    } & { [K_5 in Exclude<keyof I, keyof Response>]: never; }>(base?: I | undefined): Response;
    fromPartial<I_1 extends {
        listCustomSubsystems?: {
            subsystems?: {
                index?: number | undefined;
                identifier?: string | undefined;
                uiUrl?: string[] | undefined;
            }[] | undefined;
        } | undefined;
        call?: {
            subsystemIndex?: number | undefined;
            payload?: Uint8Array | undefined;
        } | undefined;
    } & {
        listCustomSubsystems?: ({
            subsystems?: {
                index?: number | undefined;
                identifier?: string | undefined;
                uiUrl?: string[] | undefined;
            }[] | undefined;
        } & {
            subsystems?: ({
                index?: number | undefined;
                identifier?: string | undefined;
                uiUrl?: string[] | undefined;
            }[] & ({
                index?: number | undefined;
                identifier?: string | undefined;
                uiUrl?: string[] | undefined;
            } & {
                index?: number | undefined;
                identifier?: string | undefined;
                uiUrl?: (string[] & string[] & { [K_6 in Exclude<keyof I_1["listCustomSubsystems"]["subsystems"][number]["uiUrl"], keyof string[]>]: never; }) | undefined;
            } & { [K_7 in Exclude<keyof I_1["listCustomSubsystems"]["subsystems"][number], keyof CustomSubsystemInfo>]: never; })[] & { [K_8 in Exclude<keyof I_1["listCustomSubsystems"]["subsystems"], keyof {
                index?: number | undefined;
                identifier?: string | undefined;
                uiUrl?: string[] | undefined;
            }[]>]: never; }) | undefined;
        } & { [K_9 in Exclude<keyof I_1["listCustomSubsystems"], "subsystems">]: never; }) | undefined;
        call?: ({
            subsystemIndex?: number | undefined;
            payload?: Uint8Array | undefined;
        } & {
            subsystemIndex?: number | undefined;
            payload?: Uint8Array | undefined;
        } & { [K_10 in Exclude<keyof I_1["call"], keyof CallResponse>]: never; }) | undefined;
    } & { [K_11 in Exclude<keyof I_1, keyof Response>]: never; }>(object: I_1): Response;
};
export declare const Notification: {
    encode(message: Notification, writer?: _m0.Writer): _m0.Writer;
    decode(input: _m0.Reader | Uint8Array, length?: number): Notification;
    fromJSON(object: any): Notification;
    toJSON(message: Notification): unknown;
    create<I extends {
        customNotification?: {
            subsystemIndex?: number | undefined;
            payload?: Uint8Array | undefined;
        } | undefined;
    } & {
        customNotification?: ({
            subsystemIndex?: number | undefined;
            payload?: Uint8Array | undefined;
        } & {
            subsystemIndex?: number | undefined;
            payload?: Uint8Array | undefined;
        } & { [K in Exclude<keyof I["customNotification"], keyof CustomNotification>]: never; }) | undefined;
    } & { [K_1 in Exclude<keyof I, "customNotification">]: never; }>(base?: I | undefined): Notification;
    fromPartial<I_1 extends {
        customNotification?: {
            subsystemIndex?: number | undefined;
            payload?: Uint8Array | undefined;
        } | undefined;
    } & {
        customNotification?: ({
            subsystemIndex?: number | undefined;
            payload?: Uint8Array | undefined;
        } & {
            subsystemIndex?: number | undefined;
            payload?: Uint8Array | undefined;
        } & { [K_2 in Exclude<keyof I_1["customNotification"], keyof CustomNotification>]: never; }) | undefined;
    } & { [K_3 in Exclude<keyof I_1, "customNotification">]: never; }>(object: I_1): Notification;
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
