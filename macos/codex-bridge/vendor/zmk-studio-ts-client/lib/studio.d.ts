import * as _m0 from "protobufjs/minimal";
import { Request as Request2, Response as Response7 } from "./behaviors";
import { Notification as Notification10, Request as Request1, Response as Response6 } from "./core";
import { Notification as Notification12, Request as Request4, Response as Response9 } from "./custom";
import { Notification as Notification11, Request as Request3, Response as Response8 } from "./keymap";
import { Response as Response5 } from "./meta";
export declare const protobufPackage = "zmk.studio";
/** Requests */
export interface Request {
    requestId: number;
    core?: Request1 | undefined;
    behaviors?: Request2 | undefined;
    keymap?: Request3 | undefined;
    custom?: Request4 | undefined;
}
export interface Response {
    requestResponse?: RequestResponse | undefined;
    notification?: Notification | undefined;
}
export interface RequestResponse {
    requestId: number;
    meta?: Response5 | undefined;
    core?: Response6 | undefined;
    behaviors?: Response7 | undefined;
    keymap?: Response8 | undefined;
    custom?: Response9 | undefined;
}
export interface Notification {
    core?: Notification10 | undefined;
    keymap?: Notification11 | undefined;
    custom?: Notification12 | undefined;
}
export declare const Request: {
    encode(message: Request, writer?: _m0.Writer): _m0.Writer;
    decode(input: _m0.Reader | Uint8Array, length?: number): Request;
    fromJSON(object: any): Request;
    toJSON(message: Request): unknown;
    create<I extends {
        requestId?: number | undefined;
        core?: {
            getDeviceInfo?: boolean | undefined;
            getLockState?: boolean | undefined;
            lock?: boolean | undefined;
            resetSettings?: boolean | undefined;
        } | undefined;
        behaviors?: {
            listAllBehaviors?: boolean | undefined;
            getBehaviorDetails?: {
                behaviorId?: number | undefined;
            } | undefined;
        } | undefined;
        keymap?: {
            getKeymap?: boolean | undefined;
            setLayerBinding?: {
                layerId?: number | undefined;
                keyPosition?: number | undefined;
                binding?: {
                    behaviorId?: number | undefined;
                    param1?: number | undefined;
                    param2?: number | undefined;
                } | undefined;
            } | undefined;
            checkUnsavedChanges?: boolean | undefined;
            saveChanges?: boolean | undefined;
            discardChanges?: boolean | undefined;
            getPhysicalLayouts?: boolean | undefined;
            setActivePhysicalLayout?: number | undefined;
            moveLayer?: {
                startIndex?: number | undefined;
                destIndex?: number | undefined;
            } | undefined;
            addLayer?: {} | undefined;
            removeLayer?: {
                layerIndex?: number | undefined;
            } | undefined;
            restoreLayer?: {
                layerId?: number | undefined;
                atIndex?: number | undefined;
            } | undefined;
            setLayerProps?: {
                layerId?: number | undefined;
                name?: string | undefined;
            } | undefined;
        } | undefined;
        custom?: {
            listCustomSubsystems?: {} | undefined;
            call?: {
                subsystemIndex?: number | undefined;
                payload?: Uint8Array | undefined;
            } | undefined;
        } | undefined;
    } & {
        requestId?: number | undefined;
        core?: ({
            getDeviceInfo?: boolean | undefined;
            getLockState?: boolean | undefined;
            lock?: boolean | undefined;
            resetSettings?: boolean | undefined;
        } & {
            getDeviceInfo?: boolean | undefined;
            getLockState?: boolean | undefined;
            lock?: boolean | undefined;
            resetSettings?: boolean | undefined;
        } & { [K in Exclude<keyof I["core"], keyof Request1>]: never; }) | undefined;
        behaviors?: ({
            listAllBehaviors?: boolean | undefined;
            getBehaviorDetails?: {
                behaviorId?: number | undefined;
            } | undefined;
        } & {
            listAllBehaviors?: boolean | undefined;
            getBehaviorDetails?: ({
                behaviorId?: number | undefined;
            } & {
                behaviorId?: number | undefined;
            } & { [K_1 in Exclude<keyof I["behaviors"]["getBehaviorDetails"], "behaviorId">]: never; }) | undefined;
        } & { [K_2 in Exclude<keyof I["behaviors"], keyof Request2>]: never; }) | undefined;
        keymap?: ({
            getKeymap?: boolean | undefined;
            setLayerBinding?: {
                layerId?: number | undefined;
                keyPosition?: number | undefined;
                binding?: {
                    behaviorId?: number | undefined;
                    param1?: number | undefined;
                    param2?: number | undefined;
                } | undefined;
            } | undefined;
            checkUnsavedChanges?: boolean | undefined;
            saveChanges?: boolean | undefined;
            discardChanges?: boolean | undefined;
            getPhysicalLayouts?: boolean | undefined;
            setActivePhysicalLayout?: number | undefined;
            moveLayer?: {
                startIndex?: number | undefined;
                destIndex?: number | undefined;
            } | undefined;
            addLayer?: {} | undefined;
            removeLayer?: {
                layerIndex?: number | undefined;
            } | undefined;
            restoreLayer?: {
                layerId?: number | undefined;
                atIndex?: number | undefined;
            } | undefined;
            setLayerProps?: {
                layerId?: number | undefined;
                name?: string | undefined;
            } | undefined;
        } & {
            getKeymap?: boolean | undefined;
            setLayerBinding?: ({
                layerId?: number | undefined;
                keyPosition?: number | undefined;
                binding?: {
                    behaviorId?: number | undefined;
                    param1?: number | undefined;
                    param2?: number | undefined;
                } | undefined;
            } & {
                layerId?: number | undefined;
                keyPosition?: number | undefined;
                binding?: ({
                    behaviorId?: number | undefined;
                    param1?: number | undefined;
                    param2?: number | undefined;
                } & {
                    behaviorId?: number | undefined;
                    param1?: number | undefined;
                    param2?: number | undefined;
                } & { [K_3 in Exclude<keyof I["keymap"]["setLayerBinding"]["binding"], keyof import("./keymap").BehaviorBinding>]: never; }) | undefined;
            } & { [K_4 in Exclude<keyof I["keymap"]["setLayerBinding"], keyof import("./keymap").SetLayerBindingRequest>]: never; }) | undefined;
            checkUnsavedChanges?: boolean | undefined;
            saveChanges?: boolean | undefined;
            discardChanges?: boolean | undefined;
            getPhysicalLayouts?: boolean | undefined;
            setActivePhysicalLayout?: number | undefined;
            moveLayer?: ({
                startIndex?: number | undefined;
                destIndex?: number | undefined;
            } & {
                startIndex?: number | undefined;
                destIndex?: number | undefined;
            } & { [K_5 in Exclude<keyof I["keymap"]["moveLayer"], keyof import("./keymap").MoveLayerRequest>]: never; }) | undefined;
            addLayer?: ({} & {} & { [K_6 in Exclude<keyof I["keymap"]["addLayer"], never>]: never; }) | undefined;
            removeLayer?: ({
                layerIndex?: number | undefined;
            } & {
                layerIndex?: number | undefined;
            } & { [K_7 in Exclude<keyof I["keymap"]["removeLayer"], "layerIndex">]: never; }) | undefined;
            restoreLayer?: ({
                layerId?: number | undefined;
                atIndex?: number | undefined;
            } & {
                layerId?: number | undefined;
                atIndex?: number | undefined;
            } & { [K_8 in Exclude<keyof I["keymap"]["restoreLayer"], keyof import("./keymap").RestoreLayerRequest>]: never; }) | undefined;
            setLayerProps?: ({
                layerId?: number | undefined;
                name?: string | undefined;
            } & {
                layerId?: number | undefined;
                name?: string | undefined;
            } & { [K_9 in Exclude<keyof I["keymap"]["setLayerProps"], keyof import("./keymap").SetLayerPropsRequest>]: never; }) | undefined;
        } & { [K_10 in Exclude<keyof I["keymap"], keyof Request3>]: never; }) | undefined;
        custom?: ({
            listCustomSubsystems?: {} | undefined;
            call?: {
                subsystemIndex?: number | undefined;
                payload?: Uint8Array | undefined;
            } | undefined;
        } & {
            listCustomSubsystems?: ({} & {} & { [K_11 in Exclude<keyof I["custom"]["listCustomSubsystems"], never>]: never; }) | undefined;
            call?: ({
                subsystemIndex?: number | undefined;
                payload?: Uint8Array | undefined;
            } & {
                subsystemIndex?: number | undefined;
                payload?: Uint8Array | undefined;
            } & { [K_12 in Exclude<keyof I["custom"]["call"], keyof import("./custom").CallRequest>]: never; }) | undefined;
        } & { [K_13 in Exclude<keyof I["custom"], keyof Request4>]: never; }) | undefined;
    } & { [K_14 in Exclude<keyof I, keyof Request>]: never; }>(base?: I | undefined): Request;
    fromPartial<I_1 extends {
        requestId?: number | undefined;
        core?: {
            getDeviceInfo?: boolean | undefined;
            getLockState?: boolean | undefined;
            lock?: boolean | undefined;
            resetSettings?: boolean | undefined;
        } | undefined;
        behaviors?: {
            listAllBehaviors?: boolean | undefined;
            getBehaviorDetails?: {
                behaviorId?: number | undefined;
            } | undefined;
        } | undefined;
        keymap?: {
            getKeymap?: boolean | undefined;
            setLayerBinding?: {
                layerId?: number | undefined;
                keyPosition?: number | undefined;
                binding?: {
                    behaviorId?: number | undefined;
                    param1?: number | undefined;
                    param2?: number | undefined;
                } | undefined;
            } | undefined;
            checkUnsavedChanges?: boolean | undefined;
            saveChanges?: boolean | undefined;
            discardChanges?: boolean | undefined;
            getPhysicalLayouts?: boolean | undefined;
            setActivePhysicalLayout?: number | undefined;
            moveLayer?: {
                startIndex?: number | undefined;
                destIndex?: number | undefined;
            } | undefined;
            addLayer?: {} | undefined;
            removeLayer?: {
                layerIndex?: number | undefined;
            } | undefined;
            restoreLayer?: {
                layerId?: number | undefined;
                atIndex?: number | undefined;
            } | undefined;
            setLayerProps?: {
                layerId?: number | undefined;
                name?: string | undefined;
            } | undefined;
        } | undefined;
        custom?: {
            listCustomSubsystems?: {} | undefined;
            call?: {
                subsystemIndex?: number | undefined;
                payload?: Uint8Array | undefined;
            } | undefined;
        } | undefined;
    } & {
        requestId?: number | undefined;
        core?: ({
            getDeviceInfo?: boolean | undefined;
            getLockState?: boolean | undefined;
            lock?: boolean | undefined;
            resetSettings?: boolean | undefined;
        } & {
            getDeviceInfo?: boolean | undefined;
            getLockState?: boolean | undefined;
            lock?: boolean | undefined;
            resetSettings?: boolean | undefined;
        } & { [K_15 in Exclude<keyof I_1["core"], keyof Request1>]: never; }) | undefined;
        behaviors?: ({
            listAllBehaviors?: boolean | undefined;
            getBehaviorDetails?: {
                behaviorId?: number | undefined;
            } | undefined;
        } & {
            listAllBehaviors?: boolean | undefined;
            getBehaviorDetails?: ({
                behaviorId?: number | undefined;
            } & {
                behaviorId?: number | undefined;
            } & { [K_16 in Exclude<keyof I_1["behaviors"]["getBehaviorDetails"], "behaviorId">]: never; }) | undefined;
        } & { [K_17 in Exclude<keyof I_1["behaviors"], keyof Request2>]: never; }) | undefined;
        keymap?: ({
            getKeymap?: boolean | undefined;
            setLayerBinding?: {
                layerId?: number | undefined;
                keyPosition?: number | undefined;
                binding?: {
                    behaviorId?: number | undefined;
                    param1?: number | undefined;
                    param2?: number | undefined;
                } | undefined;
            } | undefined;
            checkUnsavedChanges?: boolean | undefined;
            saveChanges?: boolean | undefined;
            discardChanges?: boolean | undefined;
            getPhysicalLayouts?: boolean | undefined;
            setActivePhysicalLayout?: number | undefined;
            moveLayer?: {
                startIndex?: number | undefined;
                destIndex?: number | undefined;
            } | undefined;
            addLayer?: {} | undefined;
            removeLayer?: {
                layerIndex?: number | undefined;
            } | undefined;
            restoreLayer?: {
                layerId?: number | undefined;
                atIndex?: number | undefined;
            } | undefined;
            setLayerProps?: {
                layerId?: number | undefined;
                name?: string | undefined;
            } | undefined;
        } & {
            getKeymap?: boolean | undefined;
            setLayerBinding?: ({
                layerId?: number | undefined;
                keyPosition?: number | undefined;
                binding?: {
                    behaviorId?: number | undefined;
                    param1?: number | undefined;
                    param2?: number | undefined;
                } | undefined;
            } & {
                layerId?: number | undefined;
                keyPosition?: number | undefined;
                binding?: ({
                    behaviorId?: number | undefined;
                    param1?: number | undefined;
                    param2?: number | undefined;
                } & {
                    behaviorId?: number | undefined;
                    param1?: number | undefined;
                    param2?: number | undefined;
                } & { [K_18 in Exclude<keyof I_1["keymap"]["setLayerBinding"]["binding"], keyof import("./keymap").BehaviorBinding>]: never; }) | undefined;
            } & { [K_19 in Exclude<keyof I_1["keymap"]["setLayerBinding"], keyof import("./keymap").SetLayerBindingRequest>]: never; }) | undefined;
            checkUnsavedChanges?: boolean | undefined;
            saveChanges?: boolean | undefined;
            discardChanges?: boolean | undefined;
            getPhysicalLayouts?: boolean | undefined;
            setActivePhysicalLayout?: number | undefined;
            moveLayer?: ({
                startIndex?: number | undefined;
                destIndex?: number | undefined;
            } & {
                startIndex?: number | undefined;
                destIndex?: number | undefined;
            } & { [K_20 in Exclude<keyof I_1["keymap"]["moveLayer"], keyof import("./keymap").MoveLayerRequest>]: never; }) | undefined;
            addLayer?: ({} & {} & { [K_21 in Exclude<keyof I_1["keymap"]["addLayer"], never>]: never; }) | undefined;
            removeLayer?: ({
                layerIndex?: number | undefined;
            } & {
                layerIndex?: number | undefined;
            } & { [K_22 in Exclude<keyof I_1["keymap"]["removeLayer"], "layerIndex">]: never; }) | undefined;
            restoreLayer?: ({
                layerId?: number | undefined;
                atIndex?: number | undefined;
            } & {
                layerId?: number | undefined;
                atIndex?: number | undefined;
            } & { [K_23 in Exclude<keyof I_1["keymap"]["restoreLayer"], keyof import("./keymap").RestoreLayerRequest>]: never; }) | undefined;
            setLayerProps?: ({
                layerId?: number | undefined;
                name?: string | undefined;
            } & {
                layerId?: number | undefined;
                name?: string | undefined;
            } & { [K_24 in Exclude<keyof I_1["keymap"]["setLayerProps"], keyof import("./keymap").SetLayerPropsRequest>]: never; }) | undefined;
        } & { [K_25 in Exclude<keyof I_1["keymap"], keyof Request3>]: never; }) | undefined;
        custom?: ({
            listCustomSubsystems?: {} | undefined;
            call?: {
                subsystemIndex?: number | undefined;
                payload?: Uint8Array | undefined;
            } | undefined;
        } & {
            listCustomSubsystems?: ({} & {} & { [K_26 in Exclude<keyof I_1["custom"]["listCustomSubsystems"], never>]: never; }) | undefined;
            call?: ({
                subsystemIndex?: number | undefined;
                payload?: Uint8Array | undefined;
            } & {
                subsystemIndex?: number | undefined;
                payload?: Uint8Array | undefined;
            } & { [K_27 in Exclude<keyof I_1["custom"]["call"], keyof import("./custom").CallRequest>]: never; }) | undefined;
        } & { [K_28 in Exclude<keyof I_1["custom"], keyof Request4>]: never; }) | undefined;
    } & { [K_29 in Exclude<keyof I_1, keyof Request>]: never; }>(object: I_1): Request;
};
export declare const Response: {
    encode(message: Response, writer?: _m0.Writer): _m0.Writer;
    decode(input: _m0.Reader | Uint8Array, length?: number): Response;
    fromJSON(object: any): Response;
    toJSON(message: Response): unknown;
    create<I extends {
        requestResponse?: {
            requestId?: number | undefined;
            meta?: {
                noResponse?: boolean | undefined;
                simpleError?: import("./meta").ErrorConditions | undefined;
            } | undefined;
            core?: {
                getDeviceInfo?: {
                    name?: string | undefined;
                    serialNumber?: Uint8Array | undefined;
                } | undefined;
                getLockState?: import("./core").LockState | undefined;
                resetSettings?: boolean | undefined;
            } | undefined;
            behaviors?: {
                listAllBehaviors?: {
                    behaviors?: number[] | undefined;
                } | undefined;
                getBehaviorDetails?: {
                    id?: number | undefined;
                    displayName?: string | undefined;
                    metadata?: {
                        param1?: {
                            name?: string | undefined;
                            nil?: {} | undefined;
                            constant?: number | undefined;
                            range?: {
                                min?: number | undefined;
                                max?: number | undefined;
                            } | undefined;
                            hidUsage?: {
                                keyboardMax?: number | undefined;
                                consumerMax?: number | undefined;
                            } | undefined;
                            layerId?: {} | undefined;
                        }[] | undefined;
                        param2?: {
                            name?: string | undefined;
                            nil?: {} | undefined;
                            constant?: number | undefined;
                            range?: {
                                min?: number | undefined;
                                max?: number | undefined;
                            } | undefined;
                            hidUsage?: {
                                keyboardMax?: number | undefined;
                                consumerMax?: number | undefined;
                            } | undefined;
                            layerId?: {} | undefined;
                        }[] | undefined;
                    }[] | undefined;
                } | undefined;
            } | undefined;
            keymap?: {
                getKeymap?: {
                    layers?: {
                        id?: number | undefined;
                        name?: string | undefined;
                        bindings?: {
                            behaviorId?: number | undefined;
                            param1?: number | undefined;
                            param2?: number | undefined;
                        }[] | undefined;
                    }[] | undefined;
                    availableLayers?: number | undefined;
                    maxLayerNameLength?: number | undefined;
                } | undefined;
                setLayerBinding?: import("./keymap").SetLayerBindingResponse | undefined;
                checkUnsavedChanges?: boolean | undefined;
                saveChanges?: {
                    ok?: boolean | undefined;
                    err?: import("./keymap").SaveChangesErrorCode | undefined;
                } | undefined;
                discardChanges?: boolean | undefined;
                getPhysicalLayouts?: {
                    activeLayoutIndex?: number | undefined;
                    layouts?: {
                        name?: string | undefined;
                        keys?: {
                            width?: number | undefined;
                            height?: number | undefined;
                            x?: number | undefined;
                            y?: number | undefined;
                            r?: number | undefined;
                            rx?: number | undefined;
                            ry?: number | undefined;
                        }[] | undefined;
                    }[] | undefined;
                } | undefined;
                setActivePhysicalLayout?: {
                    ok?: {
                        layers?: {
                            id?: number | undefined;
                            name?: string | undefined;
                            bindings?: {
                                behaviorId?: number | undefined;
                                param1?: number | undefined;
                                param2?: number | undefined;
                            }[] | undefined;
                        }[] | undefined;
                        availableLayers?: number | undefined;
                        maxLayerNameLength?: number | undefined;
                    } | undefined;
                    err?: import("./keymap").SetActivePhysicalLayoutErrorCode | undefined;
                } | undefined;
                moveLayer?: {
                    ok?: {
                        layers?: {
                            id?: number | undefined;
                            name?: string | undefined;
                            bindings?: {
                                behaviorId?: number | undefined;
                                param1?: number | undefined;
                                param2?: number | undefined;
                            }[] | undefined;
                        }[] | undefined;
                        availableLayers?: number | undefined;
                        maxLayerNameLength?: number | undefined;
                    } | undefined;
                    err?: import("./keymap").MoveLayerErrorCode | undefined;
                } | undefined;
                addLayer?: {
                    ok?: {
                        index?: number | undefined;
                        layer?: {
                            id?: number | undefined;
                            name?: string | undefined;
                            bindings?: {
                                behaviorId?: number | undefined;
                                param1?: number | undefined;
                                param2?: number | undefined;
                            }[] | undefined;
                        } | undefined;
                    } | undefined;
                    err?: import("./keymap").AddLayerErrorCode | undefined;
                } | undefined;
                removeLayer?: {
                    ok?: {} | undefined;
                    err?: import("./keymap").RemoveLayerErrorCode | undefined;
                } | undefined;
                restoreLayer?: {
                    ok?: {
                        id?: number | undefined;
                        name?: string | undefined;
                        bindings?: {
                            behaviorId?: number | undefined;
                            param1?: number | undefined;
                            param2?: number | undefined;
                        }[] | undefined;
                    } | undefined;
                    err?: import("./keymap").RestoreLayerErrorCode | undefined;
                } | undefined;
                setLayerProps?: import("./keymap").SetLayerPropsResponse | undefined;
            } | undefined;
            custom?: {
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
            } | undefined;
        } | undefined;
        notification?: {
            core?: {
                lockStateChanged?: import("./core").LockState | undefined;
            } | undefined;
            keymap?: {
                unsavedChangesStatusChanged?: boolean | undefined;
            } | undefined;
            custom?: {
                customNotification?: {
                    subsystemIndex?: number | undefined;
                    payload?: Uint8Array | undefined;
                } | undefined;
            } | undefined;
        } | undefined;
    } & {
        requestResponse?: ({
            requestId?: number | undefined;
            meta?: {
                noResponse?: boolean | undefined;
                simpleError?: import("./meta").ErrorConditions | undefined;
            } | undefined;
            core?: {
                getDeviceInfo?: {
                    name?: string | undefined;
                    serialNumber?: Uint8Array | undefined;
                } | undefined;
                getLockState?: import("./core").LockState | undefined;
                resetSettings?: boolean | undefined;
            } | undefined;
            behaviors?: {
                listAllBehaviors?: {
                    behaviors?: number[] | undefined;
                } | undefined;
                getBehaviorDetails?: {
                    id?: number | undefined;
                    displayName?: string | undefined;
                    metadata?: {
                        param1?: {
                            name?: string | undefined;
                            nil?: {} | undefined;
                            constant?: number | undefined;
                            range?: {
                                min?: number | undefined;
                                max?: number | undefined;
                            } | undefined;
                            hidUsage?: {
                                keyboardMax?: number | undefined;
                                consumerMax?: number | undefined;
                            } | undefined;
                            layerId?: {} | undefined;
                        }[] | undefined;
                        param2?: {
                            name?: string | undefined;
                            nil?: {} | undefined;
                            constant?: number | undefined;
                            range?: {
                                min?: number | undefined;
                                max?: number | undefined;
                            } | undefined;
                            hidUsage?: {
                                keyboardMax?: number | undefined;
                                consumerMax?: number | undefined;
                            } | undefined;
                            layerId?: {} | undefined;
                        }[] | undefined;
                    }[] | undefined;
                } | undefined;
            } | undefined;
            keymap?: {
                getKeymap?: {
                    layers?: {
                        id?: number | undefined;
                        name?: string | undefined;
                        bindings?: {
                            behaviorId?: number | undefined;
                            param1?: number | undefined;
                            param2?: number | undefined;
                        }[] | undefined;
                    }[] | undefined;
                    availableLayers?: number | undefined;
                    maxLayerNameLength?: number | undefined;
                } | undefined;
                setLayerBinding?: import("./keymap").SetLayerBindingResponse | undefined;
                checkUnsavedChanges?: boolean | undefined;
                saveChanges?: {
                    ok?: boolean | undefined;
                    err?: import("./keymap").SaveChangesErrorCode | undefined;
                } | undefined;
                discardChanges?: boolean | undefined;
                getPhysicalLayouts?: {
                    activeLayoutIndex?: number | undefined;
                    layouts?: {
                        name?: string | undefined;
                        keys?: {
                            width?: number | undefined;
                            height?: number | undefined;
                            x?: number | undefined;
                            y?: number | undefined;
                            r?: number | undefined;
                            rx?: number | undefined;
                            ry?: number | undefined;
                        }[] | undefined;
                    }[] | undefined;
                } | undefined;
                setActivePhysicalLayout?: {
                    ok?: {
                        layers?: {
                            id?: number | undefined;
                            name?: string | undefined;
                            bindings?: {
                                behaviorId?: number | undefined;
                                param1?: number | undefined;
                                param2?: number | undefined;
                            }[] | undefined;
                        }[] | undefined;
                        availableLayers?: number | undefined;
                        maxLayerNameLength?: number | undefined;
                    } | undefined;
                    err?: import("./keymap").SetActivePhysicalLayoutErrorCode | undefined;
                } | undefined;
                moveLayer?: {
                    ok?: {
                        layers?: {
                            id?: number | undefined;
                            name?: string | undefined;
                            bindings?: {
                                behaviorId?: number | undefined;
                                param1?: number | undefined;
                                param2?: number | undefined;
                            }[] | undefined;
                        }[] | undefined;
                        availableLayers?: number | undefined;
                        maxLayerNameLength?: number | undefined;
                    } | undefined;
                    err?: import("./keymap").MoveLayerErrorCode | undefined;
                } | undefined;
                addLayer?: {
                    ok?: {
                        index?: number | undefined;
                        layer?: {
                            id?: number | undefined;
                            name?: string | undefined;
                            bindings?: {
                                behaviorId?: number | undefined;
                                param1?: number | undefined;
                                param2?: number | undefined;
                            }[] | undefined;
                        } | undefined;
                    } | undefined;
                    err?: import("./keymap").AddLayerErrorCode | undefined;
                } | undefined;
                removeLayer?: {
                    ok?: {} | undefined;
                    err?: import("./keymap").RemoveLayerErrorCode | undefined;
                } | undefined;
                restoreLayer?: {
                    ok?: {
                        id?: number | undefined;
                        name?: string | undefined;
                        bindings?: {
                            behaviorId?: number | undefined;
                            param1?: number | undefined;
                            param2?: number | undefined;
                        }[] | undefined;
                    } | undefined;
                    err?: import("./keymap").RestoreLayerErrorCode | undefined;
                } | undefined;
                setLayerProps?: import("./keymap").SetLayerPropsResponse | undefined;
            } | undefined;
            custom?: {
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
            } | undefined;
        } & {
            requestId?: number | undefined;
            meta?: ({
                noResponse?: boolean | undefined;
                simpleError?: import("./meta").ErrorConditions | undefined;
            } & {
                noResponse?: boolean | undefined;
                simpleError?: import("./meta").ErrorConditions | undefined;
            } & { [K in Exclude<keyof I["requestResponse"]["meta"], keyof Response5>]: never; }) | undefined;
            core?: ({
                getDeviceInfo?: {
                    name?: string | undefined;
                    serialNumber?: Uint8Array | undefined;
                } | undefined;
                getLockState?: import("./core").LockState | undefined;
                resetSettings?: boolean | undefined;
            } & {
                getDeviceInfo?: ({
                    name?: string | undefined;
                    serialNumber?: Uint8Array | undefined;
                } & {
                    name?: string | undefined;
                    serialNumber?: Uint8Array | undefined;
                } & { [K_1 in Exclude<keyof I["requestResponse"]["core"]["getDeviceInfo"], keyof import("./core").GetDeviceInfoResponse>]: never; }) | undefined;
                getLockState?: import("./core").LockState | undefined;
                resetSettings?: boolean | undefined;
            } & { [K_2 in Exclude<keyof I["requestResponse"]["core"], keyof Response6>]: never; }) | undefined;
            behaviors?: ({
                listAllBehaviors?: {
                    behaviors?: number[] | undefined;
                } | undefined;
                getBehaviorDetails?: {
                    id?: number | undefined;
                    displayName?: string | undefined;
                    metadata?: {
                        param1?: {
                            name?: string | undefined;
                            nil?: {} | undefined;
                            constant?: number | undefined;
                            range?: {
                                min?: number | undefined;
                                max?: number | undefined;
                            } | undefined;
                            hidUsage?: {
                                keyboardMax?: number | undefined;
                                consumerMax?: number | undefined;
                            } | undefined;
                            layerId?: {} | undefined;
                        }[] | undefined;
                        param2?: {
                            name?: string | undefined;
                            nil?: {} | undefined;
                            constant?: number | undefined;
                            range?: {
                                min?: number | undefined;
                                max?: number | undefined;
                            } | undefined;
                            hidUsage?: {
                                keyboardMax?: number | undefined;
                                consumerMax?: number | undefined;
                            } | undefined;
                            layerId?: {} | undefined;
                        }[] | undefined;
                    }[] | undefined;
                } | undefined;
            } & {
                listAllBehaviors?: ({
                    behaviors?: number[] | undefined;
                } & {
                    behaviors?: (number[] & number[] & { [K_3 in Exclude<keyof I["requestResponse"]["behaviors"]["listAllBehaviors"]["behaviors"], keyof number[]>]: never; }) | undefined;
                } & { [K_4 in Exclude<keyof I["requestResponse"]["behaviors"]["listAllBehaviors"], "behaviors">]: never; }) | undefined;
                getBehaviorDetails?: ({
                    id?: number | undefined;
                    displayName?: string | undefined;
                    metadata?: {
                        param1?: {
                            name?: string | undefined;
                            nil?: {} | undefined;
                            constant?: number | undefined;
                            range?: {
                                min?: number | undefined;
                                max?: number | undefined;
                            } | undefined;
                            hidUsage?: {
                                keyboardMax?: number | undefined;
                                consumerMax?: number | undefined;
                            } | undefined;
                            layerId?: {} | undefined;
                        }[] | undefined;
                        param2?: {
                            name?: string | undefined;
                            nil?: {} | undefined;
                            constant?: number | undefined;
                            range?: {
                                min?: number | undefined;
                                max?: number | undefined;
                            } | undefined;
                            hidUsage?: {
                                keyboardMax?: number | undefined;
                                consumerMax?: number | undefined;
                            } | undefined;
                            layerId?: {} | undefined;
                        }[] | undefined;
                    }[] | undefined;
                } & {
                    id?: number | undefined;
                    displayName?: string | undefined;
                    metadata?: ({
                        param1?: {
                            name?: string | undefined;
                            nil?: {} | undefined;
                            constant?: number | undefined;
                            range?: {
                                min?: number | undefined;
                                max?: number | undefined;
                            } | undefined;
                            hidUsage?: {
                                keyboardMax?: number | undefined;
                                consumerMax?: number | undefined;
                            } | undefined;
                            layerId?: {} | undefined;
                        }[] | undefined;
                        param2?: {
                            name?: string | undefined;
                            nil?: {} | undefined;
                            constant?: number | undefined;
                            range?: {
                                min?: number | undefined;
                                max?: number | undefined;
                            } | undefined;
                            hidUsage?: {
                                keyboardMax?: number | undefined;
                                consumerMax?: number | undefined;
                            } | undefined;
                            layerId?: {} | undefined;
                        }[] | undefined;
                    }[] & ({
                        param1?: {
                            name?: string | undefined;
                            nil?: {} | undefined;
                            constant?: number | undefined;
                            range?: {
                                min?: number | undefined;
                                max?: number | undefined;
                            } | undefined;
                            hidUsage?: {
                                keyboardMax?: number | undefined;
                                consumerMax?: number | undefined;
                            } | undefined;
                            layerId?: {} | undefined;
                        }[] | undefined;
                        param2?: {
                            name?: string | undefined;
                            nil?: {} | undefined;
                            constant?: number | undefined;
                            range?: {
                                min?: number | undefined;
                                max?: number | undefined;
                            } | undefined;
                            hidUsage?: {
                                keyboardMax?: number | undefined;
                                consumerMax?: number | undefined;
                            } | undefined;
                            layerId?: {} | undefined;
                        }[] | undefined;
                    } & {
                        param1?: ({
                            name?: string | undefined;
                            nil?: {} | undefined;
                            constant?: number | undefined;
                            range?: {
                                min?: number | undefined;
                                max?: number | undefined;
                            } | undefined;
                            hidUsage?: {
                                keyboardMax?: number | undefined;
                                consumerMax?: number | undefined;
                            } | undefined;
                            layerId?: {} | undefined;
                        }[] & ({
                            name?: string | undefined;
                            nil?: {} | undefined;
                            constant?: number | undefined;
                            range?: {
                                min?: number | undefined;
                                max?: number | undefined;
                            } | undefined;
                            hidUsage?: {
                                keyboardMax?: number | undefined;
                                consumerMax?: number | undefined;
                            } | undefined;
                            layerId?: {} | undefined;
                        } & {
                            name?: string | undefined;
                            nil?: ({} & {} & { [K_5 in Exclude<keyof I["requestResponse"]["behaviors"]["getBehaviorDetails"]["metadata"][number]["param1"][number]["nil"], never>]: never; }) | undefined;
                            constant?: number | undefined;
                            range?: ({
                                min?: number | undefined;
                                max?: number | undefined;
                            } & {
                                min?: number | undefined;
                                max?: number | undefined;
                            } & { [K_6 in Exclude<keyof I["requestResponse"]["behaviors"]["getBehaviorDetails"]["metadata"][number]["param1"][number]["range"], keyof import("./behaviors").BehaviorParameterValueDescriptionRange>]: never; }) | undefined;
                            hidUsage?: ({
                                keyboardMax?: number | undefined;
                                consumerMax?: number | undefined;
                            } & {
                                keyboardMax?: number | undefined;
                                consumerMax?: number | undefined;
                            } & { [K_7 in Exclude<keyof I["requestResponse"]["behaviors"]["getBehaviorDetails"]["metadata"][number]["param1"][number]["hidUsage"], keyof import("./behaviors").BehaviorParameterHidUsage>]: never; }) | undefined;
                            layerId?: ({} & {} & { [K_8 in Exclude<keyof I["requestResponse"]["behaviors"]["getBehaviorDetails"]["metadata"][number]["param1"][number]["layerId"], never>]: never; }) | undefined;
                        } & { [K_9 in Exclude<keyof I["requestResponse"]["behaviors"]["getBehaviorDetails"]["metadata"][number]["param1"][number], keyof import("./behaviors").BehaviorParameterValueDescription>]: never; })[] & { [K_10 in Exclude<keyof I["requestResponse"]["behaviors"]["getBehaviorDetails"]["metadata"][number]["param1"], keyof {
                            name?: string | undefined;
                            nil?: {} | undefined;
                            constant?: number | undefined;
                            range?: {
                                min?: number | undefined;
                                max?: number | undefined;
                            } | undefined;
                            hidUsage?: {
                                keyboardMax?: number | undefined;
                                consumerMax?: number | undefined;
                            } | undefined;
                            layerId?: {} | undefined;
                        }[]>]: never; }) | undefined;
                        param2?: ({
                            name?: string | undefined;
                            nil?: {} | undefined;
                            constant?: number | undefined;
                            range?: {
                                min?: number | undefined;
                                max?: number | undefined;
                            } | undefined;
                            hidUsage?: {
                                keyboardMax?: number | undefined;
                                consumerMax?: number | undefined;
                            } | undefined;
                            layerId?: {} | undefined;
                        }[] & ({
                            name?: string | undefined;
                            nil?: {} | undefined;
                            constant?: number | undefined;
                            range?: {
                                min?: number | undefined;
                                max?: number | undefined;
                            } | undefined;
                            hidUsage?: {
                                keyboardMax?: number | undefined;
                                consumerMax?: number | undefined;
                            } | undefined;
                            layerId?: {} | undefined;
                        } & {
                            name?: string | undefined;
                            nil?: ({} & {} & { [K_11 in Exclude<keyof I["requestResponse"]["behaviors"]["getBehaviorDetails"]["metadata"][number]["param2"][number]["nil"], never>]: never; }) | undefined;
                            constant?: number | undefined;
                            range?: ({
                                min?: number | undefined;
                                max?: number | undefined;
                            } & {
                                min?: number | undefined;
                                max?: number | undefined;
                            } & { [K_12 in Exclude<keyof I["requestResponse"]["behaviors"]["getBehaviorDetails"]["metadata"][number]["param2"][number]["range"], keyof import("./behaviors").BehaviorParameterValueDescriptionRange>]: never; }) | undefined;
                            hidUsage?: ({
                                keyboardMax?: number | undefined;
                                consumerMax?: number | undefined;
                            } & {
                                keyboardMax?: number | undefined;
                                consumerMax?: number | undefined;
                            } & { [K_13 in Exclude<keyof I["requestResponse"]["behaviors"]["getBehaviorDetails"]["metadata"][number]["param2"][number]["hidUsage"], keyof import("./behaviors").BehaviorParameterHidUsage>]: never; }) | undefined;
                            layerId?: ({} & {} & { [K_14 in Exclude<keyof I["requestResponse"]["behaviors"]["getBehaviorDetails"]["metadata"][number]["param2"][number]["layerId"], never>]: never; }) | undefined;
                        } & { [K_15 in Exclude<keyof I["requestResponse"]["behaviors"]["getBehaviorDetails"]["metadata"][number]["param2"][number], keyof import("./behaviors").BehaviorParameterValueDescription>]: never; })[] & { [K_16 in Exclude<keyof I["requestResponse"]["behaviors"]["getBehaviorDetails"]["metadata"][number]["param2"], keyof {
                            name?: string | undefined;
                            nil?: {} | undefined;
                            constant?: number | undefined;
                            range?: {
                                min?: number | undefined;
                                max?: number | undefined;
                            } | undefined;
                            hidUsage?: {
                                keyboardMax?: number | undefined;
                                consumerMax?: number | undefined;
                            } | undefined;
                            layerId?: {} | undefined;
                        }[]>]: never; }) | undefined;
                    } & { [K_17 in Exclude<keyof I["requestResponse"]["behaviors"]["getBehaviorDetails"]["metadata"][number], keyof import("./behaviors").BehaviorBindingParametersSet>]: never; })[] & { [K_18 in Exclude<keyof I["requestResponse"]["behaviors"]["getBehaviorDetails"]["metadata"], keyof {
                        param1?: {
                            name?: string | undefined;
                            nil?: {} | undefined;
                            constant?: number | undefined;
                            range?: {
                                min?: number | undefined;
                                max?: number | undefined;
                            } | undefined;
                            hidUsage?: {
                                keyboardMax?: number | undefined;
                                consumerMax?: number | undefined;
                            } | undefined;
                            layerId?: {} | undefined;
                        }[] | undefined;
                        param2?: {
                            name?: string | undefined;
                            nil?: {} | undefined;
                            constant?: number | undefined;
                            range?: {
                                min?: number | undefined;
                                max?: number | undefined;
                            } | undefined;
                            hidUsage?: {
                                keyboardMax?: number | undefined;
                                consumerMax?: number | undefined;
                            } | undefined;
                            layerId?: {} | undefined;
                        }[] | undefined;
                    }[]>]: never; }) | undefined;
                } & { [K_19 in Exclude<keyof I["requestResponse"]["behaviors"]["getBehaviorDetails"], keyof import("./behaviors").GetBehaviorDetailsResponse>]: never; }) | undefined;
            } & { [K_20 in Exclude<keyof I["requestResponse"]["behaviors"], keyof Response7>]: never; }) | undefined;
            keymap?: ({
                getKeymap?: {
                    layers?: {
                        id?: number | undefined;
                        name?: string | undefined;
                        bindings?: {
                            behaviorId?: number | undefined;
                            param1?: number | undefined;
                            param2?: number | undefined;
                        }[] | undefined;
                    }[] | undefined;
                    availableLayers?: number | undefined;
                    maxLayerNameLength?: number | undefined;
                } | undefined;
                setLayerBinding?: import("./keymap").SetLayerBindingResponse | undefined;
                checkUnsavedChanges?: boolean | undefined;
                saveChanges?: {
                    ok?: boolean | undefined;
                    err?: import("./keymap").SaveChangesErrorCode | undefined;
                } | undefined;
                discardChanges?: boolean | undefined;
                getPhysicalLayouts?: {
                    activeLayoutIndex?: number | undefined;
                    layouts?: {
                        name?: string | undefined;
                        keys?: {
                            width?: number | undefined;
                            height?: number | undefined;
                            x?: number | undefined;
                            y?: number | undefined;
                            r?: number | undefined;
                            rx?: number | undefined;
                            ry?: number | undefined;
                        }[] | undefined;
                    }[] | undefined;
                } | undefined;
                setActivePhysicalLayout?: {
                    ok?: {
                        layers?: {
                            id?: number | undefined;
                            name?: string | undefined;
                            bindings?: {
                                behaviorId?: number | undefined;
                                param1?: number | undefined;
                                param2?: number | undefined;
                            }[] | undefined;
                        }[] | undefined;
                        availableLayers?: number | undefined;
                        maxLayerNameLength?: number | undefined;
                    } | undefined;
                    err?: import("./keymap").SetActivePhysicalLayoutErrorCode | undefined;
                } | undefined;
                moveLayer?: {
                    ok?: {
                        layers?: {
                            id?: number | undefined;
                            name?: string | undefined;
                            bindings?: {
                                behaviorId?: number | undefined;
                                param1?: number | undefined;
                                param2?: number | undefined;
                            }[] | undefined;
                        }[] | undefined;
                        availableLayers?: number | undefined;
                        maxLayerNameLength?: number | undefined;
                    } | undefined;
                    err?: import("./keymap").MoveLayerErrorCode | undefined;
                } | undefined;
                addLayer?: {
                    ok?: {
                        index?: number | undefined;
                        layer?: {
                            id?: number | undefined;
                            name?: string | undefined;
                            bindings?: {
                                behaviorId?: number | undefined;
                                param1?: number | undefined;
                                param2?: number | undefined;
                            }[] | undefined;
                        } | undefined;
                    } | undefined;
                    err?: import("./keymap").AddLayerErrorCode | undefined;
                } | undefined;
                removeLayer?: {
                    ok?: {} | undefined;
                    err?: import("./keymap").RemoveLayerErrorCode | undefined;
                } | undefined;
                restoreLayer?: {
                    ok?: {
                        id?: number | undefined;
                        name?: string | undefined;
                        bindings?: {
                            behaviorId?: number | undefined;
                            param1?: number | undefined;
                            param2?: number | undefined;
                        }[] | undefined;
                    } | undefined;
                    err?: import("./keymap").RestoreLayerErrorCode | undefined;
                } | undefined;
                setLayerProps?: import("./keymap").SetLayerPropsResponse | undefined;
            } & {
                getKeymap?: ({
                    layers?: {
                        id?: number | undefined;
                        name?: string | undefined;
                        bindings?: {
                            behaviorId?: number | undefined;
                            param1?: number | undefined;
                            param2?: number | undefined;
                        }[] | undefined;
                    }[] | undefined;
                    availableLayers?: number | undefined;
                    maxLayerNameLength?: number | undefined;
                } & {
                    layers?: ({
                        id?: number | undefined;
                        name?: string | undefined;
                        bindings?: {
                            behaviorId?: number | undefined;
                            param1?: number | undefined;
                            param2?: number | undefined;
                        }[] | undefined;
                    }[] & ({
                        id?: number | undefined;
                        name?: string | undefined;
                        bindings?: {
                            behaviorId?: number | undefined;
                            param1?: number | undefined;
                            param2?: number | undefined;
                        }[] | undefined;
                    } & {
                        id?: number | undefined;
                        name?: string | undefined;
                        bindings?: ({
                            behaviorId?: number | undefined;
                            param1?: number | undefined;
                            param2?: number | undefined;
                        }[] & ({
                            behaviorId?: number | undefined;
                            param1?: number | undefined;
                            param2?: number | undefined;
                        } & {
                            behaviorId?: number | undefined;
                            param1?: number | undefined;
                            param2?: number | undefined;
                        } & { [K_21 in Exclude<keyof I["requestResponse"]["keymap"]["getKeymap"]["layers"][number]["bindings"][number], keyof import("./keymap").BehaviorBinding>]: never; })[] & { [K_22 in Exclude<keyof I["requestResponse"]["keymap"]["getKeymap"]["layers"][number]["bindings"], keyof {
                            behaviorId?: number | undefined;
                            param1?: number | undefined;
                            param2?: number | undefined;
                        }[]>]: never; }) | undefined;
                    } & { [K_23 in Exclude<keyof I["requestResponse"]["keymap"]["getKeymap"]["layers"][number], keyof import("./keymap").Layer>]: never; })[] & { [K_24 in Exclude<keyof I["requestResponse"]["keymap"]["getKeymap"]["layers"], keyof {
                        id?: number | undefined;
                        name?: string | undefined;
                        bindings?: {
                            behaviorId?: number | undefined;
                            param1?: number | undefined;
                            param2?: number | undefined;
                        }[] | undefined;
                    }[]>]: never; }) | undefined;
                    availableLayers?: number | undefined;
                    maxLayerNameLength?: number | undefined;
                } & { [K_25 in Exclude<keyof I["requestResponse"]["keymap"]["getKeymap"], keyof import("./keymap").Keymap>]: never; }) | undefined;
                setLayerBinding?: import("./keymap").SetLayerBindingResponse | undefined;
                checkUnsavedChanges?: boolean | undefined;
                saveChanges?: ({
                    ok?: boolean | undefined;
                    err?: import("./keymap").SaveChangesErrorCode | undefined;
                } & {
                    ok?: boolean | undefined;
                    err?: import("./keymap").SaveChangesErrorCode | undefined;
                } & { [K_26 in Exclude<keyof I["requestResponse"]["keymap"]["saveChanges"], keyof import("./keymap").SaveChangesResponse>]: never; }) | undefined;
                discardChanges?: boolean | undefined;
                getPhysicalLayouts?: ({
                    activeLayoutIndex?: number | undefined;
                    layouts?: {
                        name?: string | undefined;
                        keys?: {
                            width?: number | undefined;
                            height?: number | undefined;
                            x?: number | undefined;
                            y?: number | undefined;
                            r?: number | undefined;
                            rx?: number | undefined;
                            ry?: number | undefined;
                        }[] | undefined;
                    }[] | undefined;
                } & {
                    activeLayoutIndex?: number | undefined;
                    layouts?: ({
                        name?: string | undefined;
                        keys?: {
                            width?: number | undefined;
                            height?: number | undefined;
                            x?: number | undefined;
                            y?: number | undefined;
                            r?: number | undefined;
                            rx?: number | undefined;
                            ry?: number | undefined;
                        }[] | undefined;
                    }[] & ({
                        name?: string | undefined;
                        keys?: {
                            width?: number | undefined;
                            height?: number | undefined;
                            x?: number | undefined;
                            y?: number | undefined;
                            r?: number | undefined;
                            rx?: number | undefined;
                            ry?: number | undefined;
                        }[] | undefined;
                    } & {
                        name?: string | undefined;
                        keys?: ({
                            width?: number | undefined;
                            height?: number | undefined;
                            x?: number | undefined;
                            y?: number | undefined;
                            r?: number | undefined;
                            rx?: number | undefined;
                            ry?: number | undefined;
                        }[] & ({
                            width?: number | undefined;
                            height?: number | undefined;
                            x?: number | undefined;
                            y?: number | undefined;
                            r?: number | undefined;
                            rx?: number | undefined;
                            ry?: number | undefined;
                        } & {
                            width?: number | undefined;
                            height?: number | undefined;
                            x?: number | undefined;
                            y?: number | undefined;
                            r?: number | undefined;
                            rx?: number | undefined;
                            ry?: number | undefined;
                        } & { [K_27 in Exclude<keyof I["requestResponse"]["keymap"]["getPhysicalLayouts"]["layouts"][number]["keys"][number], keyof import("./keymap").KeyPhysicalAttrs>]: never; })[] & { [K_28 in Exclude<keyof I["requestResponse"]["keymap"]["getPhysicalLayouts"]["layouts"][number]["keys"], keyof {
                            width?: number | undefined;
                            height?: number | undefined;
                            x?: number | undefined;
                            y?: number | undefined;
                            r?: number | undefined;
                            rx?: number | undefined;
                            ry?: number | undefined;
                        }[]>]: never; }) | undefined;
                    } & { [K_29 in Exclude<keyof I["requestResponse"]["keymap"]["getPhysicalLayouts"]["layouts"][number], keyof import("./keymap").PhysicalLayout>]: never; })[] & { [K_30 in Exclude<keyof I["requestResponse"]["keymap"]["getPhysicalLayouts"]["layouts"], keyof {
                        name?: string | undefined;
                        keys?: {
                            width?: number | undefined;
                            height?: number | undefined;
                            x?: number | undefined;
                            y?: number | undefined;
                            r?: number | undefined;
                            rx?: number | undefined;
                            ry?: number | undefined;
                        }[] | undefined;
                    }[]>]: never; }) | undefined;
                } & { [K_31 in Exclude<keyof I["requestResponse"]["keymap"]["getPhysicalLayouts"], keyof import("./keymap").PhysicalLayouts>]: never; }) | undefined;
                setActivePhysicalLayout?: ({
                    ok?: {
                        layers?: {
                            id?: number | undefined;
                            name?: string | undefined;
                            bindings?: {
                                behaviorId?: number | undefined;
                                param1?: number | undefined;
                                param2?: number | undefined;
                            }[] | undefined;
                        }[] | undefined;
                        availableLayers?: number | undefined;
                        maxLayerNameLength?: number | undefined;
                    } | undefined;
                    err?: import("./keymap").SetActivePhysicalLayoutErrorCode | undefined;
                } & {
                    ok?: ({
                        layers?: {
                            id?: number | undefined;
                            name?: string | undefined;
                            bindings?: {
                                behaviorId?: number | undefined;
                                param1?: number | undefined;
                                param2?: number | undefined;
                            }[] | undefined;
                        }[] | undefined;
                        availableLayers?: number | undefined;
                        maxLayerNameLength?: number | undefined;
                    } & {
                        layers?: ({
                            id?: number | undefined;
                            name?: string | undefined;
                            bindings?: {
                                behaviorId?: number | undefined;
                                param1?: number | undefined;
                                param2?: number | undefined;
                            }[] | undefined;
                        }[] & ({
                            id?: number | undefined;
                            name?: string | undefined;
                            bindings?: {
                                behaviorId?: number | undefined;
                                param1?: number | undefined;
                                param2?: number | undefined;
                            }[] | undefined;
                        } & {
                            id?: number | undefined;
                            name?: string | undefined;
                            bindings?: ({
                                behaviorId?: number | undefined;
                                param1?: number | undefined;
                                param2?: number | undefined;
                            }[] & ({
                                behaviorId?: number | undefined;
                                param1?: number | undefined;
                                param2?: number | undefined;
                            } & {
                                behaviorId?: number | undefined;
                                param1?: number | undefined;
                                param2?: number | undefined;
                            } & { [K_32 in Exclude<keyof I["requestResponse"]["keymap"]["setActivePhysicalLayout"]["ok"]["layers"][number]["bindings"][number], keyof import("./keymap").BehaviorBinding>]: never; })[] & { [K_33 in Exclude<keyof I["requestResponse"]["keymap"]["setActivePhysicalLayout"]["ok"]["layers"][number]["bindings"], keyof {
                                behaviorId?: number | undefined;
                                param1?: number | undefined;
                                param2?: number | undefined;
                            }[]>]: never; }) | undefined;
                        } & { [K_34 in Exclude<keyof I["requestResponse"]["keymap"]["setActivePhysicalLayout"]["ok"]["layers"][number], keyof import("./keymap").Layer>]: never; })[] & { [K_35 in Exclude<keyof I["requestResponse"]["keymap"]["setActivePhysicalLayout"]["ok"]["layers"], keyof {
                            id?: number | undefined;
                            name?: string | undefined;
                            bindings?: {
                                behaviorId?: number | undefined;
                                param1?: number | undefined;
                                param2?: number | undefined;
                            }[] | undefined;
                        }[]>]: never; }) | undefined;
                        availableLayers?: number | undefined;
                        maxLayerNameLength?: number | undefined;
                    } & { [K_36 in Exclude<keyof I["requestResponse"]["keymap"]["setActivePhysicalLayout"]["ok"], keyof import("./keymap").Keymap>]: never; }) | undefined;
                    err?: import("./keymap").SetActivePhysicalLayoutErrorCode | undefined;
                } & { [K_37 in Exclude<keyof I["requestResponse"]["keymap"]["setActivePhysicalLayout"], keyof import("./keymap").SetActivePhysicalLayoutResponse>]: never; }) | undefined;
                moveLayer?: ({
                    ok?: {
                        layers?: {
                            id?: number | undefined;
                            name?: string | undefined;
                            bindings?: {
                                behaviorId?: number | undefined;
                                param1?: number | undefined;
                                param2?: number | undefined;
                            }[] | undefined;
                        }[] | undefined;
                        availableLayers?: number | undefined;
                        maxLayerNameLength?: number | undefined;
                    } | undefined;
                    err?: import("./keymap").MoveLayerErrorCode | undefined;
                } & {
                    ok?: ({
                        layers?: {
                            id?: number | undefined;
                            name?: string | undefined;
                            bindings?: {
                                behaviorId?: number | undefined;
                                param1?: number | undefined;
                                param2?: number | undefined;
                            }[] | undefined;
                        }[] | undefined;
                        availableLayers?: number | undefined;
                        maxLayerNameLength?: number | undefined;
                    } & {
                        layers?: ({
                            id?: number | undefined;
                            name?: string | undefined;
                            bindings?: {
                                behaviorId?: number | undefined;
                                param1?: number | undefined;
                                param2?: number | undefined;
                            }[] | undefined;
                        }[] & ({
                            id?: number | undefined;
                            name?: string | undefined;
                            bindings?: {
                                behaviorId?: number | undefined;
                                param1?: number | undefined;
                                param2?: number | undefined;
                            }[] | undefined;
                        } & {
                            id?: number | undefined;
                            name?: string | undefined;
                            bindings?: ({
                                behaviorId?: number | undefined;
                                param1?: number | undefined;
                                param2?: number | undefined;
                            }[] & ({
                                behaviorId?: number | undefined;
                                param1?: number | undefined;
                                param2?: number | undefined;
                            } & {
                                behaviorId?: number | undefined;
                                param1?: number | undefined;
                                param2?: number | undefined;
                            } & { [K_38 in Exclude<keyof I["requestResponse"]["keymap"]["moveLayer"]["ok"]["layers"][number]["bindings"][number], keyof import("./keymap").BehaviorBinding>]: never; })[] & { [K_39 in Exclude<keyof I["requestResponse"]["keymap"]["moveLayer"]["ok"]["layers"][number]["bindings"], keyof {
                                behaviorId?: number | undefined;
                                param1?: number | undefined;
                                param2?: number | undefined;
                            }[]>]: never; }) | undefined;
                        } & { [K_40 in Exclude<keyof I["requestResponse"]["keymap"]["moveLayer"]["ok"]["layers"][number], keyof import("./keymap").Layer>]: never; })[] & { [K_41 in Exclude<keyof I["requestResponse"]["keymap"]["moveLayer"]["ok"]["layers"], keyof {
                            id?: number | undefined;
                            name?: string | undefined;
                            bindings?: {
                                behaviorId?: number | undefined;
                                param1?: number | undefined;
                                param2?: number | undefined;
                            }[] | undefined;
                        }[]>]: never; }) | undefined;
                        availableLayers?: number | undefined;
                        maxLayerNameLength?: number | undefined;
                    } & { [K_42 in Exclude<keyof I["requestResponse"]["keymap"]["moveLayer"]["ok"], keyof import("./keymap").Keymap>]: never; }) | undefined;
                    err?: import("./keymap").MoveLayerErrorCode | undefined;
                } & { [K_43 in Exclude<keyof I["requestResponse"]["keymap"]["moveLayer"], keyof import("./keymap").MoveLayerResponse>]: never; }) | undefined;
                addLayer?: ({
                    ok?: {
                        index?: number | undefined;
                        layer?: {
                            id?: number | undefined;
                            name?: string | undefined;
                            bindings?: {
                                behaviorId?: number | undefined;
                                param1?: number | undefined;
                                param2?: number | undefined;
                            }[] | undefined;
                        } | undefined;
                    } | undefined;
                    err?: import("./keymap").AddLayerErrorCode | undefined;
                } & {
                    ok?: ({
                        index?: number | undefined;
                        layer?: {
                            id?: number | undefined;
                            name?: string | undefined;
                            bindings?: {
                                behaviorId?: number | undefined;
                                param1?: number | undefined;
                                param2?: number | undefined;
                            }[] | undefined;
                        } | undefined;
                    } & {
                        index?: number | undefined;
                        layer?: ({
                            id?: number | undefined;
                            name?: string | undefined;
                            bindings?: {
                                behaviorId?: number | undefined;
                                param1?: number | undefined;
                                param2?: number | undefined;
                            }[] | undefined;
                        } & {
                            id?: number | undefined;
                            name?: string | undefined;
                            bindings?: ({
                                behaviorId?: number | undefined;
                                param1?: number | undefined;
                                param2?: number | undefined;
                            }[] & ({
                                behaviorId?: number | undefined;
                                param1?: number | undefined;
                                param2?: number | undefined;
                            } & {
                                behaviorId?: number | undefined;
                                param1?: number | undefined;
                                param2?: number | undefined;
                            } & { [K_44 in Exclude<keyof I["requestResponse"]["keymap"]["addLayer"]["ok"]["layer"]["bindings"][number], keyof import("./keymap").BehaviorBinding>]: never; })[] & { [K_45 in Exclude<keyof I["requestResponse"]["keymap"]["addLayer"]["ok"]["layer"]["bindings"], keyof {
                                behaviorId?: number | undefined;
                                param1?: number | undefined;
                                param2?: number | undefined;
                            }[]>]: never; }) | undefined;
                        } & { [K_46 in Exclude<keyof I["requestResponse"]["keymap"]["addLayer"]["ok"]["layer"], keyof import("./keymap").Layer>]: never; }) | undefined;
                    } & { [K_47 in Exclude<keyof I["requestResponse"]["keymap"]["addLayer"]["ok"], keyof import("./keymap").AddLayerResponseDetails>]: never; }) | undefined;
                    err?: import("./keymap").AddLayerErrorCode | undefined;
                } & { [K_48 in Exclude<keyof I["requestResponse"]["keymap"]["addLayer"], keyof import("./keymap").AddLayerResponse>]: never; }) | undefined;
                removeLayer?: ({
                    ok?: {} | undefined;
                    err?: import("./keymap").RemoveLayerErrorCode | undefined;
                } & {
                    ok?: ({} & {} & { [K_49 in Exclude<keyof I["requestResponse"]["keymap"]["removeLayer"]["ok"], never>]: never; }) | undefined;
                    err?: import("./keymap").RemoveLayerErrorCode | undefined;
                } & { [K_50 in Exclude<keyof I["requestResponse"]["keymap"]["removeLayer"], keyof import("./keymap").RemoveLayerResponse>]: never; }) | undefined;
                restoreLayer?: ({
                    ok?: {
                        id?: number | undefined;
                        name?: string | undefined;
                        bindings?: {
                            behaviorId?: number | undefined;
                            param1?: number | undefined;
                            param2?: number | undefined;
                        }[] | undefined;
                    } | undefined;
                    err?: import("./keymap").RestoreLayerErrorCode | undefined;
                } & {
                    ok?: ({
                        id?: number | undefined;
                        name?: string | undefined;
                        bindings?: {
                            behaviorId?: number | undefined;
                            param1?: number | undefined;
                            param2?: number | undefined;
                        }[] | undefined;
                    } & {
                        id?: number | undefined;
                        name?: string | undefined;
                        bindings?: ({
                            behaviorId?: number | undefined;
                            param1?: number | undefined;
                            param2?: number | undefined;
                        }[] & ({
                            behaviorId?: number | undefined;
                            param1?: number | undefined;
                            param2?: number | undefined;
                        } & {
                            behaviorId?: number | undefined;
                            param1?: number | undefined;
                            param2?: number | undefined;
                        } & { [K_51 in Exclude<keyof I["requestResponse"]["keymap"]["restoreLayer"]["ok"]["bindings"][number], keyof import("./keymap").BehaviorBinding>]: never; })[] & { [K_52 in Exclude<keyof I["requestResponse"]["keymap"]["restoreLayer"]["ok"]["bindings"], keyof {
                            behaviorId?: number | undefined;
                            param1?: number | undefined;
                            param2?: number | undefined;
                        }[]>]: never; }) | undefined;
                    } & { [K_53 in Exclude<keyof I["requestResponse"]["keymap"]["restoreLayer"]["ok"], keyof import("./keymap").Layer>]: never; }) | undefined;
                    err?: import("./keymap").RestoreLayerErrorCode | undefined;
                } & { [K_54 in Exclude<keyof I["requestResponse"]["keymap"]["restoreLayer"], keyof import("./keymap").RestoreLayerResponse>]: never; }) | undefined;
                setLayerProps?: import("./keymap").SetLayerPropsResponse | undefined;
            } & { [K_55 in Exclude<keyof I["requestResponse"]["keymap"], keyof Response8>]: never; }) | undefined;
            custom?: ({
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
                        uiUrl?: (string[] & string[] & { [K_56 in Exclude<keyof I["requestResponse"]["custom"]["listCustomSubsystems"]["subsystems"][number]["uiUrl"], keyof string[]>]: never; }) | undefined;
                    } & { [K_57 in Exclude<keyof I["requestResponse"]["custom"]["listCustomSubsystems"]["subsystems"][number], keyof import("./custom").CustomSubsystemInfo>]: never; })[] & { [K_58 in Exclude<keyof I["requestResponse"]["custom"]["listCustomSubsystems"]["subsystems"], keyof {
                        index?: number | undefined;
                        identifier?: string | undefined;
                        uiUrl?: string[] | undefined;
                    }[]>]: never; }) | undefined;
                } & { [K_59 in Exclude<keyof I["requestResponse"]["custom"]["listCustomSubsystems"], "subsystems">]: never; }) | undefined;
                call?: ({
                    subsystemIndex?: number | undefined;
                    payload?: Uint8Array | undefined;
                } & {
                    subsystemIndex?: number | undefined;
                    payload?: Uint8Array | undefined;
                } & { [K_60 in Exclude<keyof I["requestResponse"]["custom"]["call"], keyof import("./custom").CallResponse>]: never; }) | undefined;
            } & { [K_61 in Exclude<keyof I["requestResponse"]["custom"], keyof Response9>]: never; }) | undefined;
        } & { [K_62 in Exclude<keyof I["requestResponse"], keyof RequestResponse>]: never; }) | undefined;
        notification?: ({
            core?: {
                lockStateChanged?: import("./core").LockState | undefined;
            } | undefined;
            keymap?: {
                unsavedChangesStatusChanged?: boolean | undefined;
            } | undefined;
            custom?: {
                customNotification?: {
                    subsystemIndex?: number | undefined;
                    payload?: Uint8Array | undefined;
                } | undefined;
            } | undefined;
        } & {
            core?: ({
                lockStateChanged?: import("./core").LockState | undefined;
            } & {
                lockStateChanged?: import("./core").LockState | undefined;
            } & { [K_63 in Exclude<keyof I["notification"]["core"], "lockStateChanged">]: never; }) | undefined;
            keymap?: ({
                unsavedChangesStatusChanged?: boolean | undefined;
            } & {
                unsavedChangesStatusChanged?: boolean | undefined;
            } & { [K_64 in Exclude<keyof I["notification"]["keymap"], "unsavedChangesStatusChanged">]: never; }) | undefined;
            custom?: ({
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
                } & { [K_65 in Exclude<keyof I["notification"]["custom"]["customNotification"], keyof import("./custom").CustomNotification>]: never; }) | undefined;
            } & { [K_66 in Exclude<keyof I["notification"]["custom"], "customNotification">]: never; }) | undefined;
        } & { [K_67 in Exclude<keyof I["notification"], keyof Notification>]: never; }) | undefined;
    } & { [K_68 in Exclude<keyof I, keyof Response>]: never; }>(base?: I | undefined): Response;
    fromPartial<I_1 extends {
        requestResponse?: {
            requestId?: number | undefined;
            meta?: {
                noResponse?: boolean | undefined;
                simpleError?: import("./meta").ErrorConditions | undefined;
            } | undefined;
            core?: {
                getDeviceInfo?: {
                    name?: string | undefined;
                    serialNumber?: Uint8Array | undefined;
                } | undefined;
                getLockState?: import("./core").LockState | undefined;
                resetSettings?: boolean | undefined;
            } | undefined;
            behaviors?: {
                listAllBehaviors?: {
                    behaviors?: number[] | undefined;
                } | undefined;
                getBehaviorDetails?: {
                    id?: number | undefined;
                    displayName?: string | undefined;
                    metadata?: {
                        param1?: {
                            name?: string | undefined;
                            nil?: {} | undefined;
                            constant?: number | undefined;
                            range?: {
                                min?: number | undefined;
                                max?: number | undefined;
                            } | undefined;
                            hidUsage?: {
                                keyboardMax?: number | undefined;
                                consumerMax?: number | undefined;
                            } | undefined;
                            layerId?: {} | undefined;
                        }[] | undefined;
                        param2?: {
                            name?: string | undefined;
                            nil?: {} | undefined;
                            constant?: number | undefined;
                            range?: {
                                min?: number | undefined;
                                max?: number | undefined;
                            } | undefined;
                            hidUsage?: {
                                keyboardMax?: number | undefined;
                                consumerMax?: number | undefined;
                            } | undefined;
                            layerId?: {} | undefined;
                        }[] | undefined;
                    }[] | undefined;
                } | undefined;
            } | undefined;
            keymap?: {
                getKeymap?: {
                    layers?: {
                        id?: number | undefined;
                        name?: string | undefined;
                        bindings?: {
                            behaviorId?: number | undefined;
                            param1?: number | undefined;
                            param2?: number | undefined;
                        }[] | undefined;
                    }[] | undefined;
                    availableLayers?: number | undefined;
                    maxLayerNameLength?: number | undefined;
                } | undefined;
                setLayerBinding?: import("./keymap").SetLayerBindingResponse | undefined;
                checkUnsavedChanges?: boolean | undefined;
                saveChanges?: {
                    ok?: boolean | undefined;
                    err?: import("./keymap").SaveChangesErrorCode | undefined;
                } | undefined;
                discardChanges?: boolean | undefined;
                getPhysicalLayouts?: {
                    activeLayoutIndex?: number | undefined;
                    layouts?: {
                        name?: string | undefined;
                        keys?: {
                            width?: number | undefined;
                            height?: number | undefined;
                            x?: number | undefined;
                            y?: number | undefined;
                            r?: number | undefined;
                            rx?: number | undefined;
                            ry?: number | undefined;
                        }[] | undefined;
                    }[] | undefined;
                } | undefined;
                setActivePhysicalLayout?: {
                    ok?: {
                        layers?: {
                            id?: number | undefined;
                            name?: string | undefined;
                            bindings?: {
                                behaviorId?: number | undefined;
                                param1?: number | undefined;
                                param2?: number | undefined;
                            }[] | undefined;
                        }[] | undefined;
                        availableLayers?: number | undefined;
                        maxLayerNameLength?: number | undefined;
                    } | undefined;
                    err?: import("./keymap").SetActivePhysicalLayoutErrorCode | undefined;
                } | undefined;
                moveLayer?: {
                    ok?: {
                        layers?: {
                            id?: number | undefined;
                            name?: string | undefined;
                            bindings?: {
                                behaviorId?: number | undefined;
                                param1?: number | undefined;
                                param2?: number | undefined;
                            }[] | undefined;
                        }[] | undefined;
                        availableLayers?: number | undefined;
                        maxLayerNameLength?: number | undefined;
                    } | undefined;
                    err?: import("./keymap").MoveLayerErrorCode | undefined;
                } | undefined;
                addLayer?: {
                    ok?: {
                        index?: number | undefined;
                        layer?: {
                            id?: number | undefined;
                            name?: string | undefined;
                            bindings?: {
                                behaviorId?: number | undefined;
                                param1?: number | undefined;
                                param2?: number | undefined;
                            }[] | undefined;
                        } | undefined;
                    } | undefined;
                    err?: import("./keymap").AddLayerErrorCode | undefined;
                } | undefined;
                removeLayer?: {
                    ok?: {} | undefined;
                    err?: import("./keymap").RemoveLayerErrorCode | undefined;
                } | undefined;
                restoreLayer?: {
                    ok?: {
                        id?: number | undefined;
                        name?: string | undefined;
                        bindings?: {
                            behaviorId?: number | undefined;
                            param1?: number | undefined;
                            param2?: number | undefined;
                        }[] | undefined;
                    } | undefined;
                    err?: import("./keymap").RestoreLayerErrorCode | undefined;
                } | undefined;
                setLayerProps?: import("./keymap").SetLayerPropsResponse | undefined;
            } | undefined;
            custom?: {
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
            } | undefined;
        } | undefined;
        notification?: {
            core?: {
                lockStateChanged?: import("./core").LockState | undefined;
            } | undefined;
            keymap?: {
                unsavedChangesStatusChanged?: boolean | undefined;
            } | undefined;
            custom?: {
                customNotification?: {
                    subsystemIndex?: number | undefined;
                    payload?: Uint8Array | undefined;
                } | undefined;
            } | undefined;
        } | undefined;
    } & {
        requestResponse?: ({
            requestId?: number | undefined;
            meta?: {
                noResponse?: boolean | undefined;
                simpleError?: import("./meta").ErrorConditions | undefined;
            } | undefined;
            core?: {
                getDeviceInfo?: {
                    name?: string | undefined;
                    serialNumber?: Uint8Array | undefined;
                } | undefined;
                getLockState?: import("./core").LockState | undefined;
                resetSettings?: boolean | undefined;
            } | undefined;
            behaviors?: {
                listAllBehaviors?: {
                    behaviors?: number[] | undefined;
                } | undefined;
                getBehaviorDetails?: {
                    id?: number | undefined;
                    displayName?: string | undefined;
                    metadata?: {
                        param1?: {
                            name?: string | undefined;
                            nil?: {} | undefined;
                            constant?: number | undefined;
                            range?: {
                                min?: number | undefined;
                                max?: number | undefined;
                            } | undefined;
                            hidUsage?: {
                                keyboardMax?: number | undefined;
                                consumerMax?: number | undefined;
                            } | undefined;
                            layerId?: {} | undefined;
                        }[] | undefined;
                        param2?: {
                            name?: string | undefined;
                            nil?: {} | undefined;
                            constant?: number | undefined;
                            range?: {
                                min?: number | undefined;
                                max?: number | undefined;
                            } | undefined;
                            hidUsage?: {
                                keyboardMax?: number | undefined;
                                consumerMax?: number | undefined;
                            } | undefined;
                            layerId?: {} | undefined;
                        }[] | undefined;
                    }[] | undefined;
                } | undefined;
            } | undefined;
            keymap?: {
                getKeymap?: {
                    layers?: {
                        id?: number | undefined;
                        name?: string | undefined;
                        bindings?: {
                            behaviorId?: number | undefined;
                            param1?: number | undefined;
                            param2?: number | undefined;
                        }[] | undefined;
                    }[] | undefined;
                    availableLayers?: number | undefined;
                    maxLayerNameLength?: number | undefined;
                } | undefined;
                setLayerBinding?: import("./keymap").SetLayerBindingResponse | undefined;
                checkUnsavedChanges?: boolean | undefined;
                saveChanges?: {
                    ok?: boolean | undefined;
                    err?: import("./keymap").SaveChangesErrorCode | undefined;
                } | undefined;
                discardChanges?: boolean | undefined;
                getPhysicalLayouts?: {
                    activeLayoutIndex?: number | undefined;
                    layouts?: {
                        name?: string | undefined;
                        keys?: {
                            width?: number | undefined;
                            height?: number | undefined;
                            x?: number | undefined;
                            y?: number | undefined;
                            r?: number | undefined;
                            rx?: number | undefined;
                            ry?: number | undefined;
                        }[] | undefined;
                    }[] | undefined;
                } | undefined;
                setActivePhysicalLayout?: {
                    ok?: {
                        layers?: {
                            id?: number | undefined;
                            name?: string | undefined;
                            bindings?: {
                                behaviorId?: number | undefined;
                                param1?: number | undefined;
                                param2?: number | undefined;
                            }[] | undefined;
                        }[] | undefined;
                        availableLayers?: number | undefined;
                        maxLayerNameLength?: number | undefined;
                    } | undefined;
                    err?: import("./keymap").SetActivePhysicalLayoutErrorCode | undefined;
                } | undefined;
                moveLayer?: {
                    ok?: {
                        layers?: {
                            id?: number | undefined;
                            name?: string | undefined;
                            bindings?: {
                                behaviorId?: number | undefined;
                                param1?: number | undefined;
                                param2?: number | undefined;
                            }[] | undefined;
                        }[] | undefined;
                        availableLayers?: number | undefined;
                        maxLayerNameLength?: number | undefined;
                    } | undefined;
                    err?: import("./keymap").MoveLayerErrorCode | undefined;
                } | undefined;
                addLayer?: {
                    ok?: {
                        index?: number | undefined;
                        layer?: {
                            id?: number | undefined;
                            name?: string | undefined;
                            bindings?: {
                                behaviorId?: number | undefined;
                                param1?: number | undefined;
                                param2?: number | undefined;
                            }[] | undefined;
                        } | undefined;
                    } | undefined;
                    err?: import("./keymap").AddLayerErrorCode | undefined;
                } | undefined;
                removeLayer?: {
                    ok?: {} | undefined;
                    err?: import("./keymap").RemoveLayerErrorCode | undefined;
                } | undefined;
                restoreLayer?: {
                    ok?: {
                        id?: number | undefined;
                        name?: string | undefined;
                        bindings?: {
                            behaviorId?: number | undefined;
                            param1?: number | undefined;
                            param2?: number | undefined;
                        }[] | undefined;
                    } | undefined;
                    err?: import("./keymap").RestoreLayerErrorCode | undefined;
                } | undefined;
                setLayerProps?: import("./keymap").SetLayerPropsResponse | undefined;
            } | undefined;
            custom?: {
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
            } | undefined;
        } & {
            requestId?: number | undefined;
            meta?: ({
                noResponse?: boolean | undefined;
                simpleError?: import("./meta").ErrorConditions | undefined;
            } & {
                noResponse?: boolean | undefined;
                simpleError?: import("./meta").ErrorConditions | undefined;
            } & { [K_69 in Exclude<keyof I_1["requestResponse"]["meta"], keyof Response5>]: never; }) | undefined;
            core?: ({
                getDeviceInfo?: {
                    name?: string | undefined;
                    serialNumber?: Uint8Array | undefined;
                } | undefined;
                getLockState?: import("./core").LockState | undefined;
                resetSettings?: boolean | undefined;
            } & {
                getDeviceInfo?: ({
                    name?: string | undefined;
                    serialNumber?: Uint8Array | undefined;
                } & {
                    name?: string | undefined;
                    serialNumber?: Uint8Array | undefined;
                } & { [K_70 in Exclude<keyof I_1["requestResponse"]["core"]["getDeviceInfo"], keyof import("./core").GetDeviceInfoResponse>]: never; }) | undefined;
                getLockState?: import("./core").LockState | undefined;
                resetSettings?: boolean | undefined;
            } & { [K_71 in Exclude<keyof I_1["requestResponse"]["core"], keyof Response6>]: never; }) | undefined;
            behaviors?: ({
                listAllBehaviors?: {
                    behaviors?: number[] | undefined;
                } | undefined;
                getBehaviorDetails?: {
                    id?: number | undefined;
                    displayName?: string | undefined;
                    metadata?: {
                        param1?: {
                            name?: string | undefined;
                            nil?: {} | undefined;
                            constant?: number | undefined;
                            range?: {
                                min?: number | undefined;
                                max?: number | undefined;
                            } | undefined;
                            hidUsage?: {
                                keyboardMax?: number | undefined;
                                consumerMax?: number | undefined;
                            } | undefined;
                            layerId?: {} | undefined;
                        }[] | undefined;
                        param2?: {
                            name?: string | undefined;
                            nil?: {} | undefined;
                            constant?: number | undefined;
                            range?: {
                                min?: number | undefined;
                                max?: number | undefined;
                            } | undefined;
                            hidUsage?: {
                                keyboardMax?: number | undefined;
                                consumerMax?: number | undefined;
                            } | undefined;
                            layerId?: {} | undefined;
                        }[] | undefined;
                    }[] | undefined;
                } | undefined;
            } & {
                listAllBehaviors?: ({
                    behaviors?: number[] | undefined;
                } & {
                    behaviors?: (number[] & number[] & { [K_72 in Exclude<keyof I_1["requestResponse"]["behaviors"]["listAllBehaviors"]["behaviors"], keyof number[]>]: never; }) | undefined;
                } & { [K_73 in Exclude<keyof I_1["requestResponse"]["behaviors"]["listAllBehaviors"], "behaviors">]: never; }) | undefined;
                getBehaviorDetails?: ({
                    id?: number | undefined;
                    displayName?: string | undefined;
                    metadata?: {
                        param1?: {
                            name?: string | undefined;
                            nil?: {} | undefined;
                            constant?: number | undefined;
                            range?: {
                                min?: number | undefined;
                                max?: number | undefined;
                            } | undefined;
                            hidUsage?: {
                                keyboardMax?: number | undefined;
                                consumerMax?: number | undefined;
                            } | undefined;
                            layerId?: {} | undefined;
                        }[] | undefined;
                        param2?: {
                            name?: string | undefined;
                            nil?: {} | undefined;
                            constant?: number | undefined;
                            range?: {
                                min?: number | undefined;
                                max?: number | undefined;
                            } | undefined;
                            hidUsage?: {
                                keyboardMax?: number | undefined;
                                consumerMax?: number | undefined;
                            } | undefined;
                            layerId?: {} | undefined;
                        }[] | undefined;
                    }[] | undefined;
                } & {
                    id?: number | undefined;
                    displayName?: string | undefined;
                    metadata?: ({
                        param1?: {
                            name?: string | undefined;
                            nil?: {} | undefined;
                            constant?: number | undefined;
                            range?: {
                                min?: number | undefined;
                                max?: number | undefined;
                            } | undefined;
                            hidUsage?: {
                                keyboardMax?: number | undefined;
                                consumerMax?: number | undefined;
                            } | undefined;
                            layerId?: {} | undefined;
                        }[] | undefined;
                        param2?: {
                            name?: string | undefined;
                            nil?: {} | undefined;
                            constant?: number | undefined;
                            range?: {
                                min?: number | undefined;
                                max?: number | undefined;
                            } | undefined;
                            hidUsage?: {
                                keyboardMax?: number | undefined;
                                consumerMax?: number | undefined;
                            } | undefined;
                            layerId?: {} | undefined;
                        }[] | undefined;
                    }[] & ({
                        param1?: {
                            name?: string | undefined;
                            nil?: {} | undefined;
                            constant?: number | undefined;
                            range?: {
                                min?: number | undefined;
                                max?: number | undefined;
                            } | undefined;
                            hidUsage?: {
                                keyboardMax?: number | undefined;
                                consumerMax?: number | undefined;
                            } | undefined;
                            layerId?: {} | undefined;
                        }[] | undefined;
                        param2?: {
                            name?: string | undefined;
                            nil?: {} | undefined;
                            constant?: number | undefined;
                            range?: {
                                min?: number | undefined;
                                max?: number | undefined;
                            } | undefined;
                            hidUsage?: {
                                keyboardMax?: number | undefined;
                                consumerMax?: number | undefined;
                            } | undefined;
                            layerId?: {} | undefined;
                        }[] | undefined;
                    } & {
                        param1?: ({
                            name?: string | undefined;
                            nil?: {} | undefined;
                            constant?: number | undefined;
                            range?: {
                                min?: number | undefined;
                                max?: number | undefined;
                            } | undefined;
                            hidUsage?: {
                                keyboardMax?: number | undefined;
                                consumerMax?: number | undefined;
                            } | undefined;
                            layerId?: {} | undefined;
                        }[] & ({
                            name?: string | undefined;
                            nil?: {} | undefined;
                            constant?: number | undefined;
                            range?: {
                                min?: number | undefined;
                                max?: number | undefined;
                            } | undefined;
                            hidUsage?: {
                                keyboardMax?: number | undefined;
                                consumerMax?: number | undefined;
                            } | undefined;
                            layerId?: {} | undefined;
                        } & {
                            name?: string | undefined;
                            nil?: ({} & {} & { [K_74 in Exclude<keyof I_1["requestResponse"]["behaviors"]["getBehaviorDetails"]["metadata"][number]["param1"][number]["nil"], never>]: never; }) | undefined;
                            constant?: number | undefined;
                            range?: ({
                                min?: number | undefined;
                                max?: number | undefined;
                            } & {
                                min?: number | undefined;
                                max?: number | undefined;
                            } & { [K_75 in Exclude<keyof I_1["requestResponse"]["behaviors"]["getBehaviorDetails"]["metadata"][number]["param1"][number]["range"], keyof import("./behaviors").BehaviorParameterValueDescriptionRange>]: never; }) | undefined;
                            hidUsage?: ({
                                keyboardMax?: number | undefined;
                                consumerMax?: number | undefined;
                            } & {
                                keyboardMax?: number | undefined;
                                consumerMax?: number | undefined;
                            } & { [K_76 in Exclude<keyof I_1["requestResponse"]["behaviors"]["getBehaviorDetails"]["metadata"][number]["param1"][number]["hidUsage"], keyof import("./behaviors").BehaviorParameterHidUsage>]: never; }) | undefined;
                            layerId?: ({} & {} & { [K_77 in Exclude<keyof I_1["requestResponse"]["behaviors"]["getBehaviorDetails"]["metadata"][number]["param1"][number]["layerId"], never>]: never; }) | undefined;
                        } & { [K_78 in Exclude<keyof I_1["requestResponse"]["behaviors"]["getBehaviorDetails"]["metadata"][number]["param1"][number], keyof import("./behaviors").BehaviorParameterValueDescription>]: never; })[] & { [K_79 in Exclude<keyof I_1["requestResponse"]["behaviors"]["getBehaviorDetails"]["metadata"][number]["param1"], keyof {
                            name?: string | undefined;
                            nil?: {} | undefined;
                            constant?: number | undefined;
                            range?: {
                                min?: number | undefined;
                                max?: number | undefined;
                            } | undefined;
                            hidUsage?: {
                                keyboardMax?: number | undefined;
                                consumerMax?: number | undefined;
                            } | undefined;
                            layerId?: {} | undefined;
                        }[]>]: never; }) | undefined;
                        param2?: ({
                            name?: string | undefined;
                            nil?: {} | undefined;
                            constant?: number | undefined;
                            range?: {
                                min?: number | undefined;
                                max?: number | undefined;
                            } | undefined;
                            hidUsage?: {
                                keyboardMax?: number | undefined;
                                consumerMax?: number | undefined;
                            } | undefined;
                            layerId?: {} | undefined;
                        }[] & ({
                            name?: string | undefined;
                            nil?: {} | undefined;
                            constant?: number | undefined;
                            range?: {
                                min?: number | undefined;
                                max?: number | undefined;
                            } | undefined;
                            hidUsage?: {
                                keyboardMax?: number | undefined;
                                consumerMax?: number | undefined;
                            } | undefined;
                            layerId?: {} | undefined;
                        } & {
                            name?: string | undefined;
                            nil?: ({} & {} & { [K_80 in Exclude<keyof I_1["requestResponse"]["behaviors"]["getBehaviorDetails"]["metadata"][number]["param2"][number]["nil"], never>]: never; }) | undefined;
                            constant?: number | undefined;
                            range?: ({
                                min?: number | undefined;
                                max?: number | undefined;
                            } & {
                                min?: number | undefined;
                                max?: number | undefined;
                            } & { [K_81 in Exclude<keyof I_1["requestResponse"]["behaviors"]["getBehaviorDetails"]["metadata"][number]["param2"][number]["range"], keyof import("./behaviors").BehaviorParameterValueDescriptionRange>]: never; }) | undefined;
                            hidUsage?: ({
                                keyboardMax?: number | undefined;
                                consumerMax?: number | undefined;
                            } & {
                                keyboardMax?: number | undefined;
                                consumerMax?: number | undefined;
                            } & { [K_82 in Exclude<keyof I_1["requestResponse"]["behaviors"]["getBehaviorDetails"]["metadata"][number]["param2"][number]["hidUsage"], keyof import("./behaviors").BehaviorParameterHidUsage>]: never; }) | undefined;
                            layerId?: ({} & {} & { [K_83 in Exclude<keyof I_1["requestResponse"]["behaviors"]["getBehaviorDetails"]["metadata"][number]["param2"][number]["layerId"], never>]: never; }) | undefined;
                        } & { [K_84 in Exclude<keyof I_1["requestResponse"]["behaviors"]["getBehaviorDetails"]["metadata"][number]["param2"][number], keyof import("./behaviors").BehaviorParameterValueDescription>]: never; })[] & { [K_85 in Exclude<keyof I_1["requestResponse"]["behaviors"]["getBehaviorDetails"]["metadata"][number]["param2"], keyof {
                            name?: string | undefined;
                            nil?: {} | undefined;
                            constant?: number | undefined;
                            range?: {
                                min?: number | undefined;
                                max?: number | undefined;
                            } | undefined;
                            hidUsage?: {
                                keyboardMax?: number | undefined;
                                consumerMax?: number | undefined;
                            } | undefined;
                            layerId?: {} | undefined;
                        }[]>]: never; }) | undefined;
                    } & { [K_86 in Exclude<keyof I_1["requestResponse"]["behaviors"]["getBehaviorDetails"]["metadata"][number], keyof import("./behaviors").BehaviorBindingParametersSet>]: never; })[] & { [K_87 in Exclude<keyof I_1["requestResponse"]["behaviors"]["getBehaviorDetails"]["metadata"], keyof {
                        param1?: {
                            name?: string | undefined;
                            nil?: {} | undefined;
                            constant?: number | undefined;
                            range?: {
                                min?: number | undefined;
                                max?: number | undefined;
                            } | undefined;
                            hidUsage?: {
                                keyboardMax?: number | undefined;
                                consumerMax?: number | undefined;
                            } | undefined;
                            layerId?: {} | undefined;
                        }[] | undefined;
                        param2?: {
                            name?: string | undefined;
                            nil?: {} | undefined;
                            constant?: number | undefined;
                            range?: {
                                min?: number | undefined;
                                max?: number | undefined;
                            } | undefined;
                            hidUsage?: {
                                keyboardMax?: number | undefined;
                                consumerMax?: number | undefined;
                            } | undefined;
                            layerId?: {} | undefined;
                        }[] | undefined;
                    }[]>]: never; }) | undefined;
                } & { [K_88 in Exclude<keyof I_1["requestResponse"]["behaviors"]["getBehaviorDetails"], keyof import("./behaviors").GetBehaviorDetailsResponse>]: never; }) | undefined;
            } & { [K_89 in Exclude<keyof I_1["requestResponse"]["behaviors"], keyof Response7>]: never; }) | undefined;
            keymap?: ({
                getKeymap?: {
                    layers?: {
                        id?: number | undefined;
                        name?: string | undefined;
                        bindings?: {
                            behaviorId?: number | undefined;
                            param1?: number | undefined;
                            param2?: number | undefined;
                        }[] | undefined;
                    }[] | undefined;
                    availableLayers?: number | undefined;
                    maxLayerNameLength?: number | undefined;
                } | undefined;
                setLayerBinding?: import("./keymap").SetLayerBindingResponse | undefined;
                checkUnsavedChanges?: boolean | undefined;
                saveChanges?: {
                    ok?: boolean | undefined;
                    err?: import("./keymap").SaveChangesErrorCode | undefined;
                } | undefined;
                discardChanges?: boolean | undefined;
                getPhysicalLayouts?: {
                    activeLayoutIndex?: number | undefined;
                    layouts?: {
                        name?: string | undefined;
                        keys?: {
                            width?: number | undefined;
                            height?: number | undefined;
                            x?: number | undefined;
                            y?: number | undefined;
                            r?: number | undefined;
                            rx?: number | undefined;
                            ry?: number | undefined;
                        }[] | undefined;
                    }[] | undefined;
                } | undefined;
                setActivePhysicalLayout?: {
                    ok?: {
                        layers?: {
                            id?: number | undefined;
                            name?: string | undefined;
                            bindings?: {
                                behaviorId?: number | undefined;
                                param1?: number | undefined;
                                param2?: number | undefined;
                            }[] | undefined;
                        }[] | undefined;
                        availableLayers?: number | undefined;
                        maxLayerNameLength?: number | undefined;
                    } | undefined;
                    err?: import("./keymap").SetActivePhysicalLayoutErrorCode | undefined;
                } | undefined;
                moveLayer?: {
                    ok?: {
                        layers?: {
                            id?: number | undefined;
                            name?: string | undefined;
                            bindings?: {
                                behaviorId?: number | undefined;
                                param1?: number | undefined;
                                param2?: number | undefined;
                            }[] | undefined;
                        }[] | undefined;
                        availableLayers?: number | undefined;
                        maxLayerNameLength?: number | undefined;
                    } | undefined;
                    err?: import("./keymap").MoveLayerErrorCode | undefined;
                } | undefined;
                addLayer?: {
                    ok?: {
                        index?: number | undefined;
                        layer?: {
                            id?: number | undefined;
                            name?: string | undefined;
                            bindings?: {
                                behaviorId?: number | undefined;
                                param1?: number | undefined;
                                param2?: number | undefined;
                            }[] | undefined;
                        } | undefined;
                    } | undefined;
                    err?: import("./keymap").AddLayerErrorCode | undefined;
                } | undefined;
                removeLayer?: {
                    ok?: {} | undefined;
                    err?: import("./keymap").RemoveLayerErrorCode | undefined;
                } | undefined;
                restoreLayer?: {
                    ok?: {
                        id?: number | undefined;
                        name?: string | undefined;
                        bindings?: {
                            behaviorId?: number | undefined;
                            param1?: number | undefined;
                            param2?: number | undefined;
                        }[] | undefined;
                    } | undefined;
                    err?: import("./keymap").RestoreLayerErrorCode | undefined;
                } | undefined;
                setLayerProps?: import("./keymap").SetLayerPropsResponse | undefined;
            } & {
                getKeymap?: ({
                    layers?: {
                        id?: number | undefined;
                        name?: string | undefined;
                        bindings?: {
                            behaviorId?: number | undefined;
                            param1?: number | undefined;
                            param2?: number | undefined;
                        }[] | undefined;
                    }[] | undefined;
                    availableLayers?: number | undefined;
                    maxLayerNameLength?: number | undefined;
                } & {
                    layers?: ({
                        id?: number | undefined;
                        name?: string | undefined;
                        bindings?: {
                            behaviorId?: number | undefined;
                            param1?: number | undefined;
                            param2?: number | undefined;
                        }[] | undefined;
                    }[] & ({
                        id?: number | undefined;
                        name?: string | undefined;
                        bindings?: {
                            behaviorId?: number | undefined;
                            param1?: number | undefined;
                            param2?: number | undefined;
                        }[] | undefined;
                    } & {
                        id?: number | undefined;
                        name?: string | undefined;
                        bindings?: ({
                            behaviorId?: number | undefined;
                            param1?: number | undefined;
                            param2?: number | undefined;
                        }[] & ({
                            behaviorId?: number | undefined;
                            param1?: number | undefined;
                            param2?: number | undefined;
                        } & {
                            behaviorId?: number | undefined;
                            param1?: number | undefined;
                            param2?: number | undefined;
                        } & { [K_90 in Exclude<keyof I_1["requestResponse"]["keymap"]["getKeymap"]["layers"][number]["bindings"][number], keyof import("./keymap").BehaviorBinding>]: never; })[] & { [K_91 in Exclude<keyof I_1["requestResponse"]["keymap"]["getKeymap"]["layers"][number]["bindings"], keyof {
                            behaviorId?: number | undefined;
                            param1?: number | undefined;
                            param2?: number | undefined;
                        }[]>]: never; }) | undefined;
                    } & { [K_92 in Exclude<keyof I_1["requestResponse"]["keymap"]["getKeymap"]["layers"][number], keyof import("./keymap").Layer>]: never; })[] & { [K_93 in Exclude<keyof I_1["requestResponse"]["keymap"]["getKeymap"]["layers"], keyof {
                        id?: number | undefined;
                        name?: string | undefined;
                        bindings?: {
                            behaviorId?: number | undefined;
                            param1?: number | undefined;
                            param2?: number | undefined;
                        }[] | undefined;
                    }[]>]: never; }) | undefined;
                    availableLayers?: number | undefined;
                    maxLayerNameLength?: number | undefined;
                } & { [K_94 in Exclude<keyof I_1["requestResponse"]["keymap"]["getKeymap"], keyof import("./keymap").Keymap>]: never; }) | undefined;
                setLayerBinding?: import("./keymap").SetLayerBindingResponse | undefined;
                checkUnsavedChanges?: boolean | undefined;
                saveChanges?: ({
                    ok?: boolean | undefined;
                    err?: import("./keymap").SaveChangesErrorCode | undefined;
                } & {
                    ok?: boolean | undefined;
                    err?: import("./keymap").SaveChangesErrorCode | undefined;
                } & { [K_95 in Exclude<keyof I_1["requestResponse"]["keymap"]["saveChanges"], keyof import("./keymap").SaveChangesResponse>]: never; }) | undefined;
                discardChanges?: boolean | undefined;
                getPhysicalLayouts?: ({
                    activeLayoutIndex?: number | undefined;
                    layouts?: {
                        name?: string | undefined;
                        keys?: {
                            width?: number | undefined;
                            height?: number | undefined;
                            x?: number | undefined;
                            y?: number | undefined;
                            r?: number | undefined;
                            rx?: number | undefined;
                            ry?: number | undefined;
                        }[] | undefined;
                    }[] | undefined;
                } & {
                    activeLayoutIndex?: number | undefined;
                    layouts?: ({
                        name?: string | undefined;
                        keys?: {
                            width?: number | undefined;
                            height?: number | undefined;
                            x?: number | undefined;
                            y?: number | undefined;
                            r?: number | undefined;
                            rx?: number | undefined;
                            ry?: number | undefined;
                        }[] | undefined;
                    }[] & ({
                        name?: string | undefined;
                        keys?: {
                            width?: number | undefined;
                            height?: number | undefined;
                            x?: number | undefined;
                            y?: number | undefined;
                            r?: number | undefined;
                            rx?: number | undefined;
                            ry?: number | undefined;
                        }[] | undefined;
                    } & {
                        name?: string | undefined;
                        keys?: ({
                            width?: number | undefined;
                            height?: number | undefined;
                            x?: number | undefined;
                            y?: number | undefined;
                            r?: number | undefined;
                            rx?: number | undefined;
                            ry?: number | undefined;
                        }[] & ({
                            width?: number | undefined;
                            height?: number | undefined;
                            x?: number | undefined;
                            y?: number | undefined;
                            r?: number | undefined;
                            rx?: number | undefined;
                            ry?: number | undefined;
                        } & {
                            width?: number | undefined;
                            height?: number | undefined;
                            x?: number | undefined;
                            y?: number | undefined;
                            r?: number | undefined;
                            rx?: number | undefined;
                            ry?: number | undefined;
                        } & { [K_96 in Exclude<keyof I_1["requestResponse"]["keymap"]["getPhysicalLayouts"]["layouts"][number]["keys"][number], keyof import("./keymap").KeyPhysicalAttrs>]: never; })[] & { [K_97 in Exclude<keyof I_1["requestResponse"]["keymap"]["getPhysicalLayouts"]["layouts"][number]["keys"], keyof {
                            width?: number | undefined;
                            height?: number | undefined;
                            x?: number | undefined;
                            y?: number | undefined;
                            r?: number | undefined;
                            rx?: number | undefined;
                            ry?: number | undefined;
                        }[]>]: never; }) | undefined;
                    } & { [K_98 in Exclude<keyof I_1["requestResponse"]["keymap"]["getPhysicalLayouts"]["layouts"][number], keyof import("./keymap").PhysicalLayout>]: never; })[] & { [K_99 in Exclude<keyof I_1["requestResponse"]["keymap"]["getPhysicalLayouts"]["layouts"], keyof {
                        name?: string | undefined;
                        keys?: {
                            width?: number | undefined;
                            height?: number | undefined;
                            x?: number | undefined;
                            y?: number | undefined;
                            r?: number | undefined;
                            rx?: number | undefined;
                            ry?: number | undefined;
                        }[] | undefined;
                    }[]>]: never; }) | undefined;
                } & { [K_100 in Exclude<keyof I_1["requestResponse"]["keymap"]["getPhysicalLayouts"], keyof import("./keymap").PhysicalLayouts>]: never; }) | undefined;
                setActivePhysicalLayout?: ({
                    ok?: {
                        layers?: {
                            id?: number | undefined;
                            name?: string | undefined;
                            bindings?: {
                                behaviorId?: number | undefined;
                                param1?: number | undefined;
                                param2?: number | undefined;
                            }[] | undefined;
                        }[] | undefined;
                        availableLayers?: number | undefined;
                        maxLayerNameLength?: number | undefined;
                    } | undefined;
                    err?: import("./keymap").SetActivePhysicalLayoutErrorCode | undefined;
                } & {
                    ok?: ({
                        layers?: {
                            id?: number | undefined;
                            name?: string | undefined;
                            bindings?: {
                                behaviorId?: number | undefined;
                                param1?: number | undefined;
                                param2?: number | undefined;
                            }[] | undefined;
                        }[] | undefined;
                        availableLayers?: number | undefined;
                        maxLayerNameLength?: number | undefined;
                    } & {
                        layers?: ({
                            id?: number | undefined;
                            name?: string | undefined;
                            bindings?: {
                                behaviorId?: number | undefined;
                                param1?: number | undefined;
                                param2?: number | undefined;
                            }[] | undefined;
                        }[] & ({
                            id?: number | undefined;
                            name?: string | undefined;
                            bindings?: {
                                behaviorId?: number | undefined;
                                param1?: number | undefined;
                                param2?: number | undefined;
                            }[] | undefined;
                        } & {
                            id?: number | undefined;
                            name?: string | undefined;
                            bindings?: ({
                                behaviorId?: number | undefined;
                                param1?: number | undefined;
                                param2?: number | undefined;
                            }[] & ({
                                behaviorId?: number | undefined;
                                param1?: number | undefined;
                                param2?: number | undefined;
                            } & {
                                behaviorId?: number | undefined;
                                param1?: number | undefined;
                                param2?: number | undefined;
                            } & { [K_101 in Exclude<keyof I_1["requestResponse"]["keymap"]["setActivePhysicalLayout"]["ok"]["layers"][number]["bindings"][number], keyof import("./keymap").BehaviorBinding>]: never; })[] & { [K_102 in Exclude<keyof I_1["requestResponse"]["keymap"]["setActivePhysicalLayout"]["ok"]["layers"][number]["bindings"], keyof {
                                behaviorId?: number | undefined;
                                param1?: number | undefined;
                                param2?: number | undefined;
                            }[]>]: never; }) | undefined;
                        } & { [K_103 in Exclude<keyof I_1["requestResponse"]["keymap"]["setActivePhysicalLayout"]["ok"]["layers"][number], keyof import("./keymap").Layer>]: never; })[] & { [K_104 in Exclude<keyof I_1["requestResponse"]["keymap"]["setActivePhysicalLayout"]["ok"]["layers"], keyof {
                            id?: number | undefined;
                            name?: string | undefined;
                            bindings?: {
                                behaviorId?: number | undefined;
                                param1?: number | undefined;
                                param2?: number | undefined;
                            }[] | undefined;
                        }[]>]: never; }) | undefined;
                        availableLayers?: number | undefined;
                        maxLayerNameLength?: number | undefined;
                    } & { [K_105 in Exclude<keyof I_1["requestResponse"]["keymap"]["setActivePhysicalLayout"]["ok"], keyof import("./keymap").Keymap>]: never; }) | undefined;
                    err?: import("./keymap").SetActivePhysicalLayoutErrorCode | undefined;
                } & { [K_106 in Exclude<keyof I_1["requestResponse"]["keymap"]["setActivePhysicalLayout"], keyof import("./keymap").SetActivePhysicalLayoutResponse>]: never; }) | undefined;
                moveLayer?: ({
                    ok?: {
                        layers?: {
                            id?: number | undefined;
                            name?: string | undefined;
                            bindings?: {
                                behaviorId?: number | undefined;
                                param1?: number | undefined;
                                param2?: number | undefined;
                            }[] | undefined;
                        }[] | undefined;
                        availableLayers?: number | undefined;
                        maxLayerNameLength?: number | undefined;
                    } | undefined;
                    err?: import("./keymap").MoveLayerErrorCode | undefined;
                } & {
                    ok?: ({
                        layers?: {
                            id?: number | undefined;
                            name?: string | undefined;
                            bindings?: {
                                behaviorId?: number | undefined;
                                param1?: number | undefined;
                                param2?: number | undefined;
                            }[] | undefined;
                        }[] | undefined;
                        availableLayers?: number | undefined;
                        maxLayerNameLength?: number | undefined;
                    } & {
                        layers?: ({
                            id?: number | undefined;
                            name?: string | undefined;
                            bindings?: {
                                behaviorId?: number | undefined;
                                param1?: number | undefined;
                                param2?: number | undefined;
                            }[] | undefined;
                        }[] & ({
                            id?: number | undefined;
                            name?: string | undefined;
                            bindings?: {
                                behaviorId?: number | undefined;
                                param1?: number | undefined;
                                param2?: number | undefined;
                            }[] | undefined;
                        } & {
                            id?: number | undefined;
                            name?: string | undefined;
                            bindings?: ({
                                behaviorId?: number | undefined;
                                param1?: number | undefined;
                                param2?: number | undefined;
                            }[] & ({
                                behaviorId?: number | undefined;
                                param1?: number | undefined;
                                param2?: number | undefined;
                            } & {
                                behaviorId?: number | undefined;
                                param1?: number | undefined;
                                param2?: number | undefined;
                            } & { [K_107 in Exclude<keyof I_1["requestResponse"]["keymap"]["moveLayer"]["ok"]["layers"][number]["bindings"][number], keyof import("./keymap").BehaviorBinding>]: never; })[] & { [K_108 in Exclude<keyof I_1["requestResponse"]["keymap"]["moveLayer"]["ok"]["layers"][number]["bindings"], keyof {
                                behaviorId?: number | undefined;
                                param1?: number | undefined;
                                param2?: number | undefined;
                            }[]>]: never; }) | undefined;
                        } & { [K_109 in Exclude<keyof I_1["requestResponse"]["keymap"]["moveLayer"]["ok"]["layers"][number], keyof import("./keymap").Layer>]: never; })[] & { [K_110 in Exclude<keyof I_1["requestResponse"]["keymap"]["moveLayer"]["ok"]["layers"], keyof {
                            id?: number | undefined;
                            name?: string | undefined;
                            bindings?: {
                                behaviorId?: number | undefined;
                                param1?: number | undefined;
                                param2?: number | undefined;
                            }[] | undefined;
                        }[]>]: never; }) | undefined;
                        availableLayers?: number | undefined;
                        maxLayerNameLength?: number | undefined;
                    } & { [K_111 in Exclude<keyof I_1["requestResponse"]["keymap"]["moveLayer"]["ok"], keyof import("./keymap").Keymap>]: never; }) | undefined;
                    err?: import("./keymap").MoveLayerErrorCode | undefined;
                } & { [K_112 in Exclude<keyof I_1["requestResponse"]["keymap"]["moveLayer"], keyof import("./keymap").MoveLayerResponse>]: never; }) | undefined;
                addLayer?: ({
                    ok?: {
                        index?: number | undefined;
                        layer?: {
                            id?: number | undefined;
                            name?: string | undefined;
                            bindings?: {
                                behaviorId?: number | undefined;
                                param1?: number | undefined;
                                param2?: number | undefined;
                            }[] | undefined;
                        } | undefined;
                    } | undefined;
                    err?: import("./keymap").AddLayerErrorCode | undefined;
                } & {
                    ok?: ({
                        index?: number | undefined;
                        layer?: {
                            id?: number | undefined;
                            name?: string | undefined;
                            bindings?: {
                                behaviorId?: number | undefined;
                                param1?: number | undefined;
                                param2?: number | undefined;
                            }[] | undefined;
                        } | undefined;
                    } & {
                        index?: number | undefined;
                        layer?: ({
                            id?: number | undefined;
                            name?: string | undefined;
                            bindings?: {
                                behaviorId?: number | undefined;
                                param1?: number | undefined;
                                param2?: number | undefined;
                            }[] | undefined;
                        } & {
                            id?: number | undefined;
                            name?: string | undefined;
                            bindings?: ({
                                behaviorId?: number | undefined;
                                param1?: number | undefined;
                                param2?: number | undefined;
                            }[] & ({
                                behaviorId?: number | undefined;
                                param1?: number | undefined;
                                param2?: number | undefined;
                            } & {
                                behaviorId?: number | undefined;
                                param1?: number | undefined;
                                param2?: number | undefined;
                            } & { [K_113 in Exclude<keyof I_1["requestResponse"]["keymap"]["addLayer"]["ok"]["layer"]["bindings"][number], keyof import("./keymap").BehaviorBinding>]: never; })[] & { [K_114 in Exclude<keyof I_1["requestResponse"]["keymap"]["addLayer"]["ok"]["layer"]["bindings"], keyof {
                                behaviorId?: number | undefined;
                                param1?: number | undefined;
                                param2?: number | undefined;
                            }[]>]: never; }) | undefined;
                        } & { [K_115 in Exclude<keyof I_1["requestResponse"]["keymap"]["addLayer"]["ok"]["layer"], keyof import("./keymap").Layer>]: never; }) | undefined;
                    } & { [K_116 in Exclude<keyof I_1["requestResponse"]["keymap"]["addLayer"]["ok"], keyof import("./keymap").AddLayerResponseDetails>]: never; }) | undefined;
                    err?: import("./keymap").AddLayerErrorCode | undefined;
                } & { [K_117 in Exclude<keyof I_1["requestResponse"]["keymap"]["addLayer"], keyof import("./keymap").AddLayerResponse>]: never; }) | undefined;
                removeLayer?: ({
                    ok?: {} | undefined;
                    err?: import("./keymap").RemoveLayerErrorCode | undefined;
                } & {
                    ok?: ({} & {} & { [K_118 in Exclude<keyof I_1["requestResponse"]["keymap"]["removeLayer"]["ok"], never>]: never; }) | undefined;
                    err?: import("./keymap").RemoveLayerErrorCode | undefined;
                } & { [K_119 in Exclude<keyof I_1["requestResponse"]["keymap"]["removeLayer"], keyof import("./keymap").RemoveLayerResponse>]: never; }) | undefined;
                restoreLayer?: ({
                    ok?: {
                        id?: number | undefined;
                        name?: string | undefined;
                        bindings?: {
                            behaviorId?: number | undefined;
                            param1?: number | undefined;
                            param2?: number | undefined;
                        }[] | undefined;
                    } | undefined;
                    err?: import("./keymap").RestoreLayerErrorCode | undefined;
                } & {
                    ok?: ({
                        id?: number | undefined;
                        name?: string | undefined;
                        bindings?: {
                            behaviorId?: number | undefined;
                            param1?: number | undefined;
                            param2?: number | undefined;
                        }[] | undefined;
                    } & {
                        id?: number | undefined;
                        name?: string | undefined;
                        bindings?: ({
                            behaviorId?: number | undefined;
                            param1?: number | undefined;
                            param2?: number | undefined;
                        }[] & ({
                            behaviorId?: number | undefined;
                            param1?: number | undefined;
                            param2?: number | undefined;
                        } & {
                            behaviorId?: number | undefined;
                            param1?: number | undefined;
                            param2?: number | undefined;
                        } & { [K_120 in Exclude<keyof I_1["requestResponse"]["keymap"]["restoreLayer"]["ok"]["bindings"][number], keyof import("./keymap").BehaviorBinding>]: never; })[] & { [K_121 in Exclude<keyof I_1["requestResponse"]["keymap"]["restoreLayer"]["ok"]["bindings"], keyof {
                            behaviorId?: number | undefined;
                            param1?: number | undefined;
                            param2?: number | undefined;
                        }[]>]: never; }) | undefined;
                    } & { [K_122 in Exclude<keyof I_1["requestResponse"]["keymap"]["restoreLayer"]["ok"], keyof import("./keymap").Layer>]: never; }) | undefined;
                    err?: import("./keymap").RestoreLayerErrorCode | undefined;
                } & { [K_123 in Exclude<keyof I_1["requestResponse"]["keymap"]["restoreLayer"], keyof import("./keymap").RestoreLayerResponse>]: never; }) | undefined;
                setLayerProps?: import("./keymap").SetLayerPropsResponse | undefined;
            } & { [K_124 in Exclude<keyof I_1["requestResponse"]["keymap"], keyof Response8>]: never; }) | undefined;
            custom?: ({
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
                        uiUrl?: (string[] & string[] & { [K_125 in Exclude<keyof I_1["requestResponse"]["custom"]["listCustomSubsystems"]["subsystems"][number]["uiUrl"], keyof string[]>]: never; }) | undefined;
                    } & { [K_126 in Exclude<keyof I_1["requestResponse"]["custom"]["listCustomSubsystems"]["subsystems"][number], keyof import("./custom").CustomSubsystemInfo>]: never; })[] & { [K_127 in Exclude<keyof I_1["requestResponse"]["custom"]["listCustomSubsystems"]["subsystems"], keyof {
                        index?: number | undefined;
                        identifier?: string | undefined;
                        uiUrl?: string[] | undefined;
                    }[]>]: never; }) | undefined;
                } & { [K_128 in Exclude<keyof I_1["requestResponse"]["custom"]["listCustomSubsystems"], "subsystems">]: never; }) | undefined;
                call?: ({
                    subsystemIndex?: number | undefined;
                    payload?: Uint8Array | undefined;
                } & {
                    subsystemIndex?: number | undefined;
                    payload?: Uint8Array | undefined;
                } & { [K_129 in Exclude<keyof I_1["requestResponse"]["custom"]["call"], keyof import("./custom").CallResponse>]: never; }) | undefined;
            } & { [K_130 in Exclude<keyof I_1["requestResponse"]["custom"], keyof Response9>]: never; }) | undefined;
        } & { [K_131 in Exclude<keyof I_1["requestResponse"], keyof RequestResponse>]: never; }) | undefined;
        notification?: ({
            core?: {
                lockStateChanged?: import("./core").LockState | undefined;
            } | undefined;
            keymap?: {
                unsavedChangesStatusChanged?: boolean | undefined;
            } | undefined;
            custom?: {
                customNotification?: {
                    subsystemIndex?: number | undefined;
                    payload?: Uint8Array | undefined;
                } | undefined;
            } | undefined;
        } & {
            core?: ({
                lockStateChanged?: import("./core").LockState | undefined;
            } & {
                lockStateChanged?: import("./core").LockState | undefined;
            } & { [K_132 in Exclude<keyof I_1["notification"]["core"], "lockStateChanged">]: never; }) | undefined;
            keymap?: ({
                unsavedChangesStatusChanged?: boolean | undefined;
            } & {
                unsavedChangesStatusChanged?: boolean | undefined;
            } & { [K_133 in Exclude<keyof I_1["notification"]["keymap"], "unsavedChangesStatusChanged">]: never; }) | undefined;
            custom?: ({
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
                } & { [K_134 in Exclude<keyof I_1["notification"]["custom"]["customNotification"], keyof import("./custom").CustomNotification>]: never; }) | undefined;
            } & { [K_135 in Exclude<keyof I_1["notification"]["custom"], "customNotification">]: never; }) | undefined;
        } & { [K_136 in Exclude<keyof I_1["notification"], keyof Notification>]: never; }) | undefined;
    } & { [K_137 in Exclude<keyof I_1, keyof Response>]: never; }>(object: I_1): Response;
};
export declare const RequestResponse: {
    encode(message: RequestResponse, writer?: _m0.Writer): _m0.Writer;
    decode(input: _m0.Reader | Uint8Array, length?: number): RequestResponse;
    fromJSON(object: any): RequestResponse;
    toJSON(message: RequestResponse): unknown;
    create<I extends {
        requestId?: number | undefined;
        meta?: {
            noResponse?: boolean | undefined;
            simpleError?: import("./meta").ErrorConditions | undefined;
        } | undefined;
        core?: {
            getDeviceInfo?: {
                name?: string | undefined;
                serialNumber?: Uint8Array | undefined;
            } | undefined;
            getLockState?: import("./core").LockState | undefined;
            resetSettings?: boolean | undefined;
        } | undefined;
        behaviors?: {
            listAllBehaviors?: {
                behaviors?: number[] | undefined;
            } | undefined;
            getBehaviorDetails?: {
                id?: number | undefined;
                displayName?: string | undefined;
                metadata?: {
                    param1?: {
                        name?: string | undefined;
                        nil?: {} | undefined;
                        constant?: number | undefined;
                        range?: {
                            min?: number | undefined;
                            max?: number | undefined;
                        } | undefined;
                        hidUsage?: {
                            keyboardMax?: number | undefined;
                            consumerMax?: number | undefined;
                        } | undefined;
                        layerId?: {} | undefined;
                    }[] | undefined;
                    param2?: {
                        name?: string | undefined;
                        nil?: {} | undefined;
                        constant?: number | undefined;
                        range?: {
                            min?: number | undefined;
                            max?: number | undefined;
                        } | undefined;
                        hidUsage?: {
                            keyboardMax?: number | undefined;
                            consumerMax?: number | undefined;
                        } | undefined;
                        layerId?: {} | undefined;
                    }[] | undefined;
                }[] | undefined;
            } | undefined;
        } | undefined;
        keymap?: {
            getKeymap?: {
                layers?: {
                    id?: number | undefined;
                    name?: string | undefined;
                    bindings?: {
                        behaviorId?: number | undefined;
                        param1?: number | undefined;
                        param2?: number | undefined;
                    }[] | undefined;
                }[] | undefined;
                availableLayers?: number | undefined;
                maxLayerNameLength?: number | undefined;
            } | undefined;
            setLayerBinding?: import("./keymap").SetLayerBindingResponse | undefined;
            checkUnsavedChanges?: boolean | undefined;
            saveChanges?: {
                ok?: boolean | undefined;
                err?: import("./keymap").SaveChangesErrorCode | undefined;
            } | undefined;
            discardChanges?: boolean | undefined;
            getPhysicalLayouts?: {
                activeLayoutIndex?: number | undefined;
                layouts?: {
                    name?: string | undefined;
                    keys?: {
                        width?: number | undefined;
                        height?: number | undefined;
                        x?: number | undefined;
                        y?: number | undefined;
                        r?: number | undefined;
                        rx?: number | undefined;
                        ry?: number | undefined;
                    }[] | undefined;
                }[] | undefined;
            } | undefined;
            setActivePhysicalLayout?: {
                ok?: {
                    layers?: {
                        id?: number | undefined;
                        name?: string | undefined;
                        bindings?: {
                            behaviorId?: number | undefined;
                            param1?: number | undefined;
                            param2?: number | undefined;
                        }[] | undefined;
                    }[] | undefined;
                    availableLayers?: number | undefined;
                    maxLayerNameLength?: number | undefined;
                } | undefined;
                err?: import("./keymap").SetActivePhysicalLayoutErrorCode | undefined;
            } | undefined;
            moveLayer?: {
                ok?: {
                    layers?: {
                        id?: number | undefined;
                        name?: string | undefined;
                        bindings?: {
                            behaviorId?: number | undefined;
                            param1?: number | undefined;
                            param2?: number | undefined;
                        }[] | undefined;
                    }[] | undefined;
                    availableLayers?: number | undefined;
                    maxLayerNameLength?: number | undefined;
                } | undefined;
                err?: import("./keymap").MoveLayerErrorCode | undefined;
            } | undefined;
            addLayer?: {
                ok?: {
                    index?: number | undefined;
                    layer?: {
                        id?: number | undefined;
                        name?: string | undefined;
                        bindings?: {
                            behaviorId?: number | undefined;
                            param1?: number | undefined;
                            param2?: number | undefined;
                        }[] | undefined;
                    } | undefined;
                } | undefined;
                err?: import("./keymap").AddLayerErrorCode | undefined;
            } | undefined;
            removeLayer?: {
                ok?: {} | undefined;
                err?: import("./keymap").RemoveLayerErrorCode | undefined;
            } | undefined;
            restoreLayer?: {
                ok?: {
                    id?: number | undefined;
                    name?: string | undefined;
                    bindings?: {
                        behaviorId?: number | undefined;
                        param1?: number | undefined;
                        param2?: number | undefined;
                    }[] | undefined;
                } | undefined;
                err?: import("./keymap").RestoreLayerErrorCode | undefined;
            } | undefined;
            setLayerProps?: import("./keymap").SetLayerPropsResponse | undefined;
        } | undefined;
        custom?: {
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
        } | undefined;
    } & {
        requestId?: number | undefined;
        meta?: ({
            noResponse?: boolean | undefined;
            simpleError?: import("./meta").ErrorConditions | undefined;
        } & {
            noResponse?: boolean | undefined;
            simpleError?: import("./meta").ErrorConditions | undefined;
        } & { [K in Exclude<keyof I["meta"], keyof Response5>]: never; }) | undefined;
        core?: ({
            getDeviceInfo?: {
                name?: string | undefined;
                serialNumber?: Uint8Array | undefined;
            } | undefined;
            getLockState?: import("./core").LockState | undefined;
            resetSettings?: boolean | undefined;
        } & {
            getDeviceInfo?: ({
                name?: string | undefined;
                serialNumber?: Uint8Array | undefined;
            } & {
                name?: string | undefined;
                serialNumber?: Uint8Array | undefined;
            } & { [K_1 in Exclude<keyof I["core"]["getDeviceInfo"], keyof import("./core").GetDeviceInfoResponse>]: never; }) | undefined;
            getLockState?: import("./core").LockState | undefined;
            resetSettings?: boolean | undefined;
        } & { [K_2 in Exclude<keyof I["core"], keyof Response6>]: never; }) | undefined;
        behaviors?: ({
            listAllBehaviors?: {
                behaviors?: number[] | undefined;
            } | undefined;
            getBehaviorDetails?: {
                id?: number | undefined;
                displayName?: string | undefined;
                metadata?: {
                    param1?: {
                        name?: string | undefined;
                        nil?: {} | undefined;
                        constant?: number | undefined;
                        range?: {
                            min?: number | undefined;
                            max?: number | undefined;
                        } | undefined;
                        hidUsage?: {
                            keyboardMax?: number | undefined;
                            consumerMax?: number | undefined;
                        } | undefined;
                        layerId?: {} | undefined;
                    }[] | undefined;
                    param2?: {
                        name?: string | undefined;
                        nil?: {} | undefined;
                        constant?: number | undefined;
                        range?: {
                            min?: number | undefined;
                            max?: number | undefined;
                        } | undefined;
                        hidUsage?: {
                            keyboardMax?: number | undefined;
                            consumerMax?: number | undefined;
                        } | undefined;
                        layerId?: {} | undefined;
                    }[] | undefined;
                }[] | undefined;
            } | undefined;
        } & {
            listAllBehaviors?: ({
                behaviors?: number[] | undefined;
            } & {
                behaviors?: (number[] & number[] & { [K_3 in Exclude<keyof I["behaviors"]["listAllBehaviors"]["behaviors"], keyof number[]>]: never; }) | undefined;
            } & { [K_4 in Exclude<keyof I["behaviors"]["listAllBehaviors"], "behaviors">]: never; }) | undefined;
            getBehaviorDetails?: ({
                id?: number | undefined;
                displayName?: string | undefined;
                metadata?: {
                    param1?: {
                        name?: string | undefined;
                        nil?: {} | undefined;
                        constant?: number | undefined;
                        range?: {
                            min?: number | undefined;
                            max?: number | undefined;
                        } | undefined;
                        hidUsage?: {
                            keyboardMax?: number | undefined;
                            consumerMax?: number | undefined;
                        } | undefined;
                        layerId?: {} | undefined;
                    }[] | undefined;
                    param2?: {
                        name?: string | undefined;
                        nil?: {} | undefined;
                        constant?: number | undefined;
                        range?: {
                            min?: number | undefined;
                            max?: number | undefined;
                        } | undefined;
                        hidUsage?: {
                            keyboardMax?: number | undefined;
                            consumerMax?: number | undefined;
                        } | undefined;
                        layerId?: {} | undefined;
                    }[] | undefined;
                }[] | undefined;
            } & {
                id?: number | undefined;
                displayName?: string | undefined;
                metadata?: ({
                    param1?: {
                        name?: string | undefined;
                        nil?: {} | undefined;
                        constant?: number | undefined;
                        range?: {
                            min?: number | undefined;
                            max?: number | undefined;
                        } | undefined;
                        hidUsage?: {
                            keyboardMax?: number | undefined;
                            consumerMax?: number | undefined;
                        } | undefined;
                        layerId?: {} | undefined;
                    }[] | undefined;
                    param2?: {
                        name?: string | undefined;
                        nil?: {} | undefined;
                        constant?: number | undefined;
                        range?: {
                            min?: number | undefined;
                            max?: number | undefined;
                        } | undefined;
                        hidUsage?: {
                            keyboardMax?: number | undefined;
                            consumerMax?: number | undefined;
                        } | undefined;
                        layerId?: {} | undefined;
                    }[] | undefined;
                }[] & ({
                    param1?: {
                        name?: string | undefined;
                        nil?: {} | undefined;
                        constant?: number | undefined;
                        range?: {
                            min?: number | undefined;
                            max?: number | undefined;
                        } | undefined;
                        hidUsage?: {
                            keyboardMax?: number | undefined;
                            consumerMax?: number | undefined;
                        } | undefined;
                        layerId?: {} | undefined;
                    }[] | undefined;
                    param2?: {
                        name?: string | undefined;
                        nil?: {} | undefined;
                        constant?: number | undefined;
                        range?: {
                            min?: number | undefined;
                            max?: number | undefined;
                        } | undefined;
                        hidUsage?: {
                            keyboardMax?: number | undefined;
                            consumerMax?: number | undefined;
                        } | undefined;
                        layerId?: {} | undefined;
                    }[] | undefined;
                } & {
                    param1?: ({
                        name?: string | undefined;
                        nil?: {} | undefined;
                        constant?: number | undefined;
                        range?: {
                            min?: number | undefined;
                            max?: number | undefined;
                        } | undefined;
                        hidUsage?: {
                            keyboardMax?: number | undefined;
                            consumerMax?: number | undefined;
                        } | undefined;
                        layerId?: {} | undefined;
                    }[] & ({
                        name?: string | undefined;
                        nil?: {} | undefined;
                        constant?: number | undefined;
                        range?: {
                            min?: number | undefined;
                            max?: number | undefined;
                        } | undefined;
                        hidUsage?: {
                            keyboardMax?: number | undefined;
                            consumerMax?: number | undefined;
                        } | undefined;
                        layerId?: {} | undefined;
                    } & {
                        name?: string | undefined;
                        nil?: ({} & {} & { [K_5 in Exclude<keyof I["behaviors"]["getBehaviorDetails"]["metadata"][number]["param1"][number]["nil"], never>]: never; }) | undefined;
                        constant?: number | undefined;
                        range?: ({
                            min?: number | undefined;
                            max?: number | undefined;
                        } & {
                            min?: number | undefined;
                            max?: number | undefined;
                        } & { [K_6 in Exclude<keyof I["behaviors"]["getBehaviorDetails"]["metadata"][number]["param1"][number]["range"], keyof import("./behaviors").BehaviorParameterValueDescriptionRange>]: never; }) | undefined;
                        hidUsage?: ({
                            keyboardMax?: number | undefined;
                            consumerMax?: number | undefined;
                        } & {
                            keyboardMax?: number | undefined;
                            consumerMax?: number | undefined;
                        } & { [K_7 in Exclude<keyof I["behaviors"]["getBehaviorDetails"]["metadata"][number]["param1"][number]["hidUsage"], keyof import("./behaviors").BehaviorParameterHidUsage>]: never; }) | undefined;
                        layerId?: ({} & {} & { [K_8 in Exclude<keyof I["behaviors"]["getBehaviorDetails"]["metadata"][number]["param1"][number]["layerId"], never>]: never; }) | undefined;
                    } & { [K_9 in Exclude<keyof I["behaviors"]["getBehaviorDetails"]["metadata"][number]["param1"][number], keyof import("./behaviors").BehaviorParameterValueDescription>]: never; })[] & { [K_10 in Exclude<keyof I["behaviors"]["getBehaviorDetails"]["metadata"][number]["param1"], keyof {
                        name?: string | undefined;
                        nil?: {} | undefined;
                        constant?: number | undefined;
                        range?: {
                            min?: number | undefined;
                            max?: number | undefined;
                        } | undefined;
                        hidUsage?: {
                            keyboardMax?: number | undefined;
                            consumerMax?: number | undefined;
                        } | undefined;
                        layerId?: {} | undefined;
                    }[]>]: never; }) | undefined;
                    param2?: ({
                        name?: string | undefined;
                        nil?: {} | undefined;
                        constant?: number | undefined;
                        range?: {
                            min?: number | undefined;
                            max?: number | undefined;
                        } | undefined;
                        hidUsage?: {
                            keyboardMax?: number | undefined;
                            consumerMax?: number | undefined;
                        } | undefined;
                        layerId?: {} | undefined;
                    }[] & ({
                        name?: string | undefined;
                        nil?: {} | undefined;
                        constant?: number | undefined;
                        range?: {
                            min?: number | undefined;
                            max?: number | undefined;
                        } | undefined;
                        hidUsage?: {
                            keyboardMax?: number | undefined;
                            consumerMax?: number | undefined;
                        } | undefined;
                        layerId?: {} | undefined;
                    } & {
                        name?: string | undefined;
                        nil?: ({} & {} & { [K_11 in Exclude<keyof I["behaviors"]["getBehaviorDetails"]["metadata"][number]["param2"][number]["nil"], never>]: never; }) | undefined;
                        constant?: number | undefined;
                        range?: ({
                            min?: number | undefined;
                            max?: number | undefined;
                        } & {
                            min?: number | undefined;
                            max?: number | undefined;
                        } & { [K_12 in Exclude<keyof I["behaviors"]["getBehaviorDetails"]["metadata"][number]["param2"][number]["range"], keyof import("./behaviors").BehaviorParameterValueDescriptionRange>]: never; }) | undefined;
                        hidUsage?: ({
                            keyboardMax?: number | undefined;
                            consumerMax?: number | undefined;
                        } & {
                            keyboardMax?: number | undefined;
                            consumerMax?: number | undefined;
                        } & { [K_13 in Exclude<keyof I["behaviors"]["getBehaviorDetails"]["metadata"][number]["param2"][number]["hidUsage"], keyof import("./behaviors").BehaviorParameterHidUsage>]: never; }) | undefined;
                        layerId?: ({} & {} & { [K_14 in Exclude<keyof I["behaviors"]["getBehaviorDetails"]["metadata"][number]["param2"][number]["layerId"], never>]: never; }) | undefined;
                    } & { [K_15 in Exclude<keyof I["behaviors"]["getBehaviorDetails"]["metadata"][number]["param2"][number], keyof import("./behaviors").BehaviorParameterValueDescription>]: never; })[] & { [K_16 in Exclude<keyof I["behaviors"]["getBehaviorDetails"]["metadata"][number]["param2"], keyof {
                        name?: string | undefined;
                        nil?: {} | undefined;
                        constant?: number | undefined;
                        range?: {
                            min?: number | undefined;
                            max?: number | undefined;
                        } | undefined;
                        hidUsage?: {
                            keyboardMax?: number | undefined;
                            consumerMax?: number | undefined;
                        } | undefined;
                        layerId?: {} | undefined;
                    }[]>]: never; }) | undefined;
                } & { [K_17 in Exclude<keyof I["behaviors"]["getBehaviorDetails"]["metadata"][number], keyof import("./behaviors").BehaviorBindingParametersSet>]: never; })[] & { [K_18 in Exclude<keyof I["behaviors"]["getBehaviorDetails"]["metadata"], keyof {
                    param1?: {
                        name?: string | undefined;
                        nil?: {} | undefined;
                        constant?: number | undefined;
                        range?: {
                            min?: number | undefined;
                            max?: number | undefined;
                        } | undefined;
                        hidUsage?: {
                            keyboardMax?: number | undefined;
                            consumerMax?: number | undefined;
                        } | undefined;
                        layerId?: {} | undefined;
                    }[] | undefined;
                    param2?: {
                        name?: string | undefined;
                        nil?: {} | undefined;
                        constant?: number | undefined;
                        range?: {
                            min?: number | undefined;
                            max?: number | undefined;
                        } | undefined;
                        hidUsage?: {
                            keyboardMax?: number | undefined;
                            consumerMax?: number | undefined;
                        } | undefined;
                        layerId?: {} | undefined;
                    }[] | undefined;
                }[]>]: never; }) | undefined;
            } & { [K_19 in Exclude<keyof I["behaviors"]["getBehaviorDetails"], keyof import("./behaviors").GetBehaviorDetailsResponse>]: never; }) | undefined;
        } & { [K_20 in Exclude<keyof I["behaviors"], keyof Response7>]: never; }) | undefined;
        keymap?: ({
            getKeymap?: {
                layers?: {
                    id?: number | undefined;
                    name?: string | undefined;
                    bindings?: {
                        behaviorId?: number | undefined;
                        param1?: number | undefined;
                        param2?: number | undefined;
                    }[] | undefined;
                }[] | undefined;
                availableLayers?: number | undefined;
                maxLayerNameLength?: number | undefined;
            } | undefined;
            setLayerBinding?: import("./keymap").SetLayerBindingResponse | undefined;
            checkUnsavedChanges?: boolean | undefined;
            saveChanges?: {
                ok?: boolean | undefined;
                err?: import("./keymap").SaveChangesErrorCode | undefined;
            } | undefined;
            discardChanges?: boolean | undefined;
            getPhysicalLayouts?: {
                activeLayoutIndex?: number | undefined;
                layouts?: {
                    name?: string | undefined;
                    keys?: {
                        width?: number | undefined;
                        height?: number | undefined;
                        x?: number | undefined;
                        y?: number | undefined;
                        r?: number | undefined;
                        rx?: number | undefined;
                        ry?: number | undefined;
                    }[] | undefined;
                }[] | undefined;
            } | undefined;
            setActivePhysicalLayout?: {
                ok?: {
                    layers?: {
                        id?: number | undefined;
                        name?: string | undefined;
                        bindings?: {
                            behaviorId?: number | undefined;
                            param1?: number | undefined;
                            param2?: number | undefined;
                        }[] | undefined;
                    }[] | undefined;
                    availableLayers?: number | undefined;
                    maxLayerNameLength?: number | undefined;
                } | undefined;
                err?: import("./keymap").SetActivePhysicalLayoutErrorCode | undefined;
            } | undefined;
            moveLayer?: {
                ok?: {
                    layers?: {
                        id?: number | undefined;
                        name?: string | undefined;
                        bindings?: {
                            behaviorId?: number | undefined;
                            param1?: number | undefined;
                            param2?: number | undefined;
                        }[] | undefined;
                    }[] | undefined;
                    availableLayers?: number | undefined;
                    maxLayerNameLength?: number | undefined;
                } | undefined;
                err?: import("./keymap").MoveLayerErrorCode | undefined;
            } | undefined;
            addLayer?: {
                ok?: {
                    index?: number | undefined;
                    layer?: {
                        id?: number | undefined;
                        name?: string | undefined;
                        bindings?: {
                            behaviorId?: number | undefined;
                            param1?: number | undefined;
                            param2?: number | undefined;
                        }[] | undefined;
                    } | undefined;
                } | undefined;
                err?: import("./keymap").AddLayerErrorCode | undefined;
            } | undefined;
            removeLayer?: {
                ok?: {} | undefined;
                err?: import("./keymap").RemoveLayerErrorCode | undefined;
            } | undefined;
            restoreLayer?: {
                ok?: {
                    id?: number | undefined;
                    name?: string | undefined;
                    bindings?: {
                        behaviorId?: number | undefined;
                        param1?: number | undefined;
                        param2?: number | undefined;
                    }[] | undefined;
                } | undefined;
                err?: import("./keymap").RestoreLayerErrorCode | undefined;
            } | undefined;
            setLayerProps?: import("./keymap").SetLayerPropsResponse | undefined;
        } & {
            getKeymap?: ({
                layers?: {
                    id?: number | undefined;
                    name?: string | undefined;
                    bindings?: {
                        behaviorId?: number | undefined;
                        param1?: number | undefined;
                        param2?: number | undefined;
                    }[] | undefined;
                }[] | undefined;
                availableLayers?: number | undefined;
                maxLayerNameLength?: number | undefined;
            } & {
                layers?: ({
                    id?: number | undefined;
                    name?: string | undefined;
                    bindings?: {
                        behaviorId?: number | undefined;
                        param1?: number | undefined;
                        param2?: number | undefined;
                    }[] | undefined;
                }[] & ({
                    id?: number | undefined;
                    name?: string | undefined;
                    bindings?: {
                        behaviorId?: number | undefined;
                        param1?: number | undefined;
                        param2?: number | undefined;
                    }[] | undefined;
                } & {
                    id?: number | undefined;
                    name?: string | undefined;
                    bindings?: ({
                        behaviorId?: number | undefined;
                        param1?: number | undefined;
                        param2?: number | undefined;
                    }[] & ({
                        behaviorId?: number | undefined;
                        param1?: number | undefined;
                        param2?: number | undefined;
                    } & {
                        behaviorId?: number | undefined;
                        param1?: number | undefined;
                        param2?: number | undefined;
                    } & { [K_21 in Exclude<keyof I["keymap"]["getKeymap"]["layers"][number]["bindings"][number], keyof import("./keymap").BehaviorBinding>]: never; })[] & { [K_22 in Exclude<keyof I["keymap"]["getKeymap"]["layers"][number]["bindings"], keyof {
                        behaviorId?: number | undefined;
                        param1?: number | undefined;
                        param2?: number | undefined;
                    }[]>]: never; }) | undefined;
                } & { [K_23 in Exclude<keyof I["keymap"]["getKeymap"]["layers"][number], keyof import("./keymap").Layer>]: never; })[] & { [K_24 in Exclude<keyof I["keymap"]["getKeymap"]["layers"], keyof {
                    id?: number | undefined;
                    name?: string | undefined;
                    bindings?: {
                        behaviorId?: number | undefined;
                        param1?: number | undefined;
                        param2?: number | undefined;
                    }[] | undefined;
                }[]>]: never; }) | undefined;
                availableLayers?: number | undefined;
                maxLayerNameLength?: number | undefined;
            } & { [K_25 in Exclude<keyof I["keymap"]["getKeymap"], keyof import("./keymap").Keymap>]: never; }) | undefined;
            setLayerBinding?: import("./keymap").SetLayerBindingResponse | undefined;
            checkUnsavedChanges?: boolean | undefined;
            saveChanges?: ({
                ok?: boolean | undefined;
                err?: import("./keymap").SaveChangesErrorCode | undefined;
            } & {
                ok?: boolean | undefined;
                err?: import("./keymap").SaveChangesErrorCode | undefined;
            } & { [K_26 in Exclude<keyof I["keymap"]["saveChanges"], keyof import("./keymap").SaveChangesResponse>]: never; }) | undefined;
            discardChanges?: boolean | undefined;
            getPhysicalLayouts?: ({
                activeLayoutIndex?: number | undefined;
                layouts?: {
                    name?: string | undefined;
                    keys?: {
                        width?: number | undefined;
                        height?: number | undefined;
                        x?: number | undefined;
                        y?: number | undefined;
                        r?: number | undefined;
                        rx?: number | undefined;
                        ry?: number | undefined;
                    }[] | undefined;
                }[] | undefined;
            } & {
                activeLayoutIndex?: number | undefined;
                layouts?: ({
                    name?: string | undefined;
                    keys?: {
                        width?: number | undefined;
                        height?: number | undefined;
                        x?: number | undefined;
                        y?: number | undefined;
                        r?: number | undefined;
                        rx?: number | undefined;
                        ry?: number | undefined;
                    }[] | undefined;
                }[] & ({
                    name?: string | undefined;
                    keys?: {
                        width?: number | undefined;
                        height?: number | undefined;
                        x?: number | undefined;
                        y?: number | undefined;
                        r?: number | undefined;
                        rx?: number | undefined;
                        ry?: number | undefined;
                    }[] | undefined;
                } & {
                    name?: string | undefined;
                    keys?: ({
                        width?: number | undefined;
                        height?: number | undefined;
                        x?: number | undefined;
                        y?: number | undefined;
                        r?: number | undefined;
                        rx?: number | undefined;
                        ry?: number | undefined;
                    }[] & ({
                        width?: number | undefined;
                        height?: number | undefined;
                        x?: number | undefined;
                        y?: number | undefined;
                        r?: number | undefined;
                        rx?: number | undefined;
                        ry?: number | undefined;
                    } & {
                        width?: number | undefined;
                        height?: number | undefined;
                        x?: number | undefined;
                        y?: number | undefined;
                        r?: number | undefined;
                        rx?: number | undefined;
                        ry?: number | undefined;
                    } & { [K_27 in Exclude<keyof I["keymap"]["getPhysicalLayouts"]["layouts"][number]["keys"][number], keyof import("./keymap").KeyPhysicalAttrs>]: never; })[] & { [K_28 in Exclude<keyof I["keymap"]["getPhysicalLayouts"]["layouts"][number]["keys"], keyof {
                        width?: number | undefined;
                        height?: number | undefined;
                        x?: number | undefined;
                        y?: number | undefined;
                        r?: number | undefined;
                        rx?: number | undefined;
                        ry?: number | undefined;
                    }[]>]: never; }) | undefined;
                } & { [K_29 in Exclude<keyof I["keymap"]["getPhysicalLayouts"]["layouts"][number], keyof import("./keymap").PhysicalLayout>]: never; })[] & { [K_30 in Exclude<keyof I["keymap"]["getPhysicalLayouts"]["layouts"], keyof {
                    name?: string | undefined;
                    keys?: {
                        width?: number | undefined;
                        height?: number | undefined;
                        x?: number | undefined;
                        y?: number | undefined;
                        r?: number | undefined;
                        rx?: number | undefined;
                        ry?: number | undefined;
                    }[] | undefined;
                }[]>]: never; }) | undefined;
            } & { [K_31 in Exclude<keyof I["keymap"]["getPhysicalLayouts"], keyof import("./keymap").PhysicalLayouts>]: never; }) | undefined;
            setActivePhysicalLayout?: ({
                ok?: {
                    layers?: {
                        id?: number | undefined;
                        name?: string | undefined;
                        bindings?: {
                            behaviorId?: number | undefined;
                            param1?: number | undefined;
                            param2?: number | undefined;
                        }[] | undefined;
                    }[] | undefined;
                    availableLayers?: number | undefined;
                    maxLayerNameLength?: number | undefined;
                } | undefined;
                err?: import("./keymap").SetActivePhysicalLayoutErrorCode | undefined;
            } & {
                ok?: ({
                    layers?: {
                        id?: number | undefined;
                        name?: string | undefined;
                        bindings?: {
                            behaviorId?: number | undefined;
                            param1?: number | undefined;
                            param2?: number | undefined;
                        }[] | undefined;
                    }[] | undefined;
                    availableLayers?: number | undefined;
                    maxLayerNameLength?: number | undefined;
                } & {
                    layers?: ({
                        id?: number | undefined;
                        name?: string | undefined;
                        bindings?: {
                            behaviorId?: number | undefined;
                            param1?: number | undefined;
                            param2?: number | undefined;
                        }[] | undefined;
                    }[] & ({
                        id?: number | undefined;
                        name?: string | undefined;
                        bindings?: {
                            behaviorId?: number | undefined;
                            param1?: number | undefined;
                            param2?: number | undefined;
                        }[] | undefined;
                    } & {
                        id?: number | undefined;
                        name?: string | undefined;
                        bindings?: ({
                            behaviorId?: number | undefined;
                            param1?: number | undefined;
                            param2?: number | undefined;
                        }[] & ({
                            behaviorId?: number | undefined;
                            param1?: number | undefined;
                            param2?: number | undefined;
                        } & {
                            behaviorId?: number | undefined;
                            param1?: number | undefined;
                            param2?: number | undefined;
                        } & { [K_32 in Exclude<keyof I["keymap"]["setActivePhysicalLayout"]["ok"]["layers"][number]["bindings"][number], keyof import("./keymap").BehaviorBinding>]: never; })[] & { [K_33 in Exclude<keyof I["keymap"]["setActivePhysicalLayout"]["ok"]["layers"][number]["bindings"], keyof {
                            behaviorId?: number | undefined;
                            param1?: number | undefined;
                            param2?: number | undefined;
                        }[]>]: never; }) | undefined;
                    } & { [K_34 in Exclude<keyof I["keymap"]["setActivePhysicalLayout"]["ok"]["layers"][number], keyof import("./keymap").Layer>]: never; })[] & { [K_35 in Exclude<keyof I["keymap"]["setActivePhysicalLayout"]["ok"]["layers"], keyof {
                        id?: number | undefined;
                        name?: string | undefined;
                        bindings?: {
                            behaviorId?: number | undefined;
                            param1?: number | undefined;
                            param2?: number | undefined;
                        }[] | undefined;
                    }[]>]: never; }) | undefined;
                    availableLayers?: number | undefined;
                    maxLayerNameLength?: number | undefined;
                } & { [K_36 in Exclude<keyof I["keymap"]["setActivePhysicalLayout"]["ok"], keyof import("./keymap").Keymap>]: never; }) | undefined;
                err?: import("./keymap").SetActivePhysicalLayoutErrorCode | undefined;
            } & { [K_37 in Exclude<keyof I["keymap"]["setActivePhysicalLayout"], keyof import("./keymap").SetActivePhysicalLayoutResponse>]: never; }) | undefined;
            moveLayer?: ({
                ok?: {
                    layers?: {
                        id?: number | undefined;
                        name?: string | undefined;
                        bindings?: {
                            behaviorId?: number | undefined;
                            param1?: number | undefined;
                            param2?: number | undefined;
                        }[] | undefined;
                    }[] | undefined;
                    availableLayers?: number | undefined;
                    maxLayerNameLength?: number | undefined;
                } | undefined;
                err?: import("./keymap").MoveLayerErrorCode | undefined;
            } & {
                ok?: ({
                    layers?: {
                        id?: number | undefined;
                        name?: string | undefined;
                        bindings?: {
                            behaviorId?: number | undefined;
                            param1?: number | undefined;
                            param2?: number | undefined;
                        }[] | undefined;
                    }[] | undefined;
                    availableLayers?: number | undefined;
                    maxLayerNameLength?: number | undefined;
                } & {
                    layers?: ({
                        id?: number | undefined;
                        name?: string | undefined;
                        bindings?: {
                            behaviorId?: number | undefined;
                            param1?: number | undefined;
                            param2?: number | undefined;
                        }[] | undefined;
                    }[] & ({
                        id?: number | undefined;
                        name?: string | undefined;
                        bindings?: {
                            behaviorId?: number | undefined;
                            param1?: number | undefined;
                            param2?: number | undefined;
                        }[] | undefined;
                    } & {
                        id?: number | undefined;
                        name?: string | undefined;
                        bindings?: ({
                            behaviorId?: number | undefined;
                            param1?: number | undefined;
                            param2?: number | undefined;
                        }[] & ({
                            behaviorId?: number | undefined;
                            param1?: number | undefined;
                            param2?: number | undefined;
                        } & {
                            behaviorId?: number | undefined;
                            param1?: number | undefined;
                            param2?: number | undefined;
                        } & { [K_38 in Exclude<keyof I["keymap"]["moveLayer"]["ok"]["layers"][number]["bindings"][number], keyof import("./keymap").BehaviorBinding>]: never; })[] & { [K_39 in Exclude<keyof I["keymap"]["moveLayer"]["ok"]["layers"][number]["bindings"], keyof {
                            behaviorId?: number | undefined;
                            param1?: number | undefined;
                            param2?: number | undefined;
                        }[]>]: never; }) | undefined;
                    } & { [K_40 in Exclude<keyof I["keymap"]["moveLayer"]["ok"]["layers"][number], keyof import("./keymap").Layer>]: never; })[] & { [K_41 in Exclude<keyof I["keymap"]["moveLayer"]["ok"]["layers"], keyof {
                        id?: number | undefined;
                        name?: string | undefined;
                        bindings?: {
                            behaviorId?: number | undefined;
                            param1?: number | undefined;
                            param2?: number | undefined;
                        }[] | undefined;
                    }[]>]: never; }) | undefined;
                    availableLayers?: number | undefined;
                    maxLayerNameLength?: number | undefined;
                } & { [K_42 in Exclude<keyof I["keymap"]["moveLayer"]["ok"], keyof import("./keymap").Keymap>]: never; }) | undefined;
                err?: import("./keymap").MoveLayerErrorCode | undefined;
            } & { [K_43 in Exclude<keyof I["keymap"]["moveLayer"], keyof import("./keymap").MoveLayerResponse>]: never; }) | undefined;
            addLayer?: ({
                ok?: {
                    index?: number | undefined;
                    layer?: {
                        id?: number | undefined;
                        name?: string | undefined;
                        bindings?: {
                            behaviorId?: number | undefined;
                            param1?: number | undefined;
                            param2?: number | undefined;
                        }[] | undefined;
                    } | undefined;
                } | undefined;
                err?: import("./keymap").AddLayerErrorCode | undefined;
            } & {
                ok?: ({
                    index?: number | undefined;
                    layer?: {
                        id?: number | undefined;
                        name?: string | undefined;
                        bindings?: {
                            behaviorId?: number | undefined;
                            param1?: number | undefined;
                            param2?: number | undefined;
                        }[] | undefined;
                    } | undefined;
                } & {
                    index?: number | undefined;
                    layer?: ({
                        id?: number | undefined;
                        name?: string | undefined;
                        bindings?: {
                            behaviorId?: number | undefined;
                            param1?: number | undefined;
                            param2?: number | undefined;
                        }[] | undefined;
                    } & {
                        id?: number | undefined;
                        name?: string | undefined;
                        bindings?: ({
                            behaviorId?: number | undefined;
                            param1?: number | undefined;
                            param2?: number | undefined;
                        }[] & ({
                            behaviorId?: number | undefined;
                            param1?: number | undefined;
                            param2?: number | undefined;
                        } & {
                            behaviorId?: number | undefined;
                            param1?: number | undefined;
                            param2?: number | undefined;
                        } & { [K_44 in Exclude<keyof I["keymap"]["addLayer"]["ok"]["layer"]["bindings"][number], keyof import("./keymap").BehaviorBinding>]: never; })[] & { [K_45 in Exclude<keyof I["keymap"]["addLayer"]["ok"]["layer"]["bindings"], keyof {
                            behaviorId?: number | undefined;
                            param1?: number | undefined;
                            param2?: number | undefined;
                        }[]>]: never; }) | undefined;
                    } & { [K_46 in Exclude<keyof I["keymap"]["addLayer"]["ok"]["layer"], keyof import("./keymap").Layer>]: never; }) | undefined;
                } & { [K_47 in Exclude<keyof I["keymap"]["addLayer"]["ok"], keyof import("./keymap").AddLayerResponseDetails>]: never; }) | undefined;
                err?: import("./keymap").AddLayerErrorCode | undefined;
            } & { [K_48 in Exclude<keyof I["keymap"]["addLayer"], keyof import("./keymap").AddLayerResponse>]: never; }) | undefined;
            removeLayer?: ({
                ok?: {} | undefined;
                err?: import("./keymap").RemoveLayerErrorCode | undefined;
            } & {
                ok?: ({} & {} & { [K_49 in Exclude<keyof I["keymap"]["removeLayer"]["ok"], never>]: never; }) | undefined;
                err?: import("./keymap").RemoveLayerErrorCode | undefined;
            } & { [K_50 in Exclude<keyof I["keymap"]["removeLayer"], keyof import("./keymap").RemoveLayerResponse>]: never; }) | undefined;
            restoreLayer?: ({
                ok?: {
                    id?: number | undefined;
                    name?: string | undefined;
                    bindings?: {
                        behaviorId?: number | undefined;
                        param1?: number | undefined;
                        param2?: number | undefined;
                    }[] | undefined;
                } | undefined;
                err?: import("./keymap").RestoreLayerErrorCode | undefined;
            } & {
                ok?: ({
                    id?: number | undefined;
                    name?: string | undefined;
                    bindings?: {
                        behaviorId?: number | undefined;
                        param1?: number | undefined;
                        param2?: number | undefined;
                    }[] | undefined;
                } & {
                    id?: number | undefined;
                    name?: string | undefined;
                    bindings?: ({
                        behaviorId?: number | undefined;
                        param1?: number | undefined;
                        param2?: number | undefined;
                    }[] & ({
                        behaviorId?: number | undefined;
                        param1?: number | undefined;
                        param2?: number | undefined;
                    } & {
                        behaviorId?: number | undefined;
                        param1?: number | undefined;
                        param2?: number | undefined;
                    } & { [K_51 in Exclude<keyof I["keymap"]["restoreLayer"]["ok"]["bindings"][number], keyof import("./keymap").BehaviorBinding>]: never; })[] & { [K_52 in Exclude<keyof I["keymap"]["restoreLayer"]["ok"]["bindings"], keyof {
                        behaviorId?: number | undefined;
                        param1?: number | undefined;
                        param2?: number | undefined;
                    }[]>]: never; }) | undefined;
                } & { [K_53 in Exclude<keyof I["keymap"]["restoreLayer"]["ok"], keyof import("./keymap").Layer>]: never; }) | undefined;
                err?: import("./keymap").RestoreLayerErrorCode | undefined;
            } & { [K_54 in Exclude<keyof I["keymap"]["restoreLayer"], keyof import("./keymap").RestoreLayerResponse>]: never; }) | undefined;
            setLayerProps?: import("./keymap").SetLayerPropsResponse | undefined;
        } & { [K_55 in Exclude<keyof I["keymap"], keyof Response8>]: never; }) | undefined;
        custom?: ({
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
                    uiUrl?: (string[] & string[] & { [K_56 in Exclude<keyof I["custom"]["listCustomSubsystems"]["subsystems"][number]["uiUrl"], keyof string[]>]: never; }) | undefined;
                } & { [K_57 in Exclude<keyof I["custom"]["listCustomSubsystems"]["subsystems"][number], keyof import("./custom").CustomSubsystemInfo>]: never; })[] & { [K_58 in Exclude<keyof I["custom"]["listCustomSubsystems"]["subsystems"], keyof {
                    index?: number | undefined;
                    identifier?: string | undefined;
                    uiUrl?: string[] | undefined;
                }[]>]: never; }) | undefined;
            } & { [K_59 in Exclude<keyof I["custom"]["listCustomSubsystems"], "subsystems">]: never; }) | undefined;
            call?: ({
                subsystemIndex?: number | undefined;
                payload?: Uint8Array | undefined;
            } & {
                subsystemIndex?: number | undefined;
                payload?: Uint8Array | undefined;
            } & { [K_60 in Exclude<keyof I["custom"]["call"], keyof import("./custom").CallResponse>]: never; }) | undefined;
        } & { [K_61 in Exclude<keyof I["custom"], keyof Response9>]: never; }) | undefined;
    } & { [K_62 in Exclude<keyof I, keyof RequestResponse>]: never; }>(base?: I | undefined): RequestResponse;
    fromPartial<I_1 extends {
        requestId?: number | undefined;
        meta?: {
            noResponse?: boolean | undefined;
            simpleError?: import("./meta").ErrorConditions | undefined;
        } | undefined;
        core?: {
            getDeviceInfo?: {
                name?: string | undefined;
                serialNumber?: Uint8Array | undefined;
            } | undefined;
            getLockState?: import("./core").LockState | undefined;
            resetSettings?: boolean | undefined;
        } | undefined;
        behaviors?: {
            listAllBehaviors?: {
                behaviors?: number[] | undefined;
            } | undefined;
            getBehaviorDetails?: {
                id?: number | undefined;
                displayName?: string | undefined;
                metadata?: {
                    param1?: {
                        name?: string | undefined;
                        nil?: {} | undefined;
                        constant?: number | undefined;
                        range?: {
                            min?: number | undefined;
                            max?: number | undefined;
                        } | undefined;
                        hidUsage?: {
                            keyboardMax?: number | undefined;
                            consumerMax?: number | undefined;
                        } | undefined;
                        layerId?: {} | undefined;
                    }[] | undefined;
                    param2?: {
                        name?: string | undefined;
                        nil?: {} | undefined;
                        constant?: number | undefined;
                        range?: {
                            min?: number | undefined;
                            max?: number | undefined;
                        } | undefined;
                        hidUsage?: {
                            keyboardMax?: number | undefined;
                            consumerMax?: number | undefined;
                        } | undefined;
                        layerId?: {} | undefined;
                    }[] | undefined;
                }[] | undefined;
            } | undefined;
        } | undefined;
        keymap?: {
            getKeymap?: {
                layers?: {
                    id?: number | undefined;
                    name?: string | undefined;
                    bindings?: {
                        behaviorId?: number | undefined;
                        param1?: number | undefined;
                        param2?: number | undefined;
                    }[] | undefined;
                }[] | undefined;
                availableLayers?: number | undefined;
                maxLayerNameLength?: number | undefined;
            } | undefined;
            setLayerBinding?: import("./keymap").SetLayerBindingResponse | undefined;
            checkUnsavedChanges?: boolean | undefined;
            saveChanges?: {
                ok?: boolean | undefined;
                err?: import("./keymap").SaveChangesErrorCode | undefined;
            } | undefined;
            discardChanges?: boolean | undefined;
            getPhysicalLayouts?: {
                activeLayoutIndex?: number | undefined;
                layouts?: {
                    name?: string | undefined;
                    keys?: {
                        width?: number | undefined;
                        height?: number | undefined;
                        x?: number | undefined;
                        y?: number | undefined;
                        r?: number | undefined;
                        rx?: number | undefined;
                        ry?: number | undefined;
                    }[] | undefined;
                }[] | undefined;
            } | undefined;
            setActivePhysicalLayout?: {
                ok?: {
                    layers?: {
                        id?: number | undefined;
                        name?: string | undefined;
                        bindings?: {
                            behaviorId?: number | undefined;
                            param1?: number | undefined;
                            param2?: number | undefined;
                        }[] | undefined;
                    }[] | undefined;
                    availableLayers?: number | undefined;
                    maxLayerNameLength?: number | undefined;
                } | undefined;
                err?: import("./keymap").SetActivePhysicalLayoutErrorCode | undefined;
            } | undefined;
            moveLayer?: {
                ok?: {
                    layers?: {
                        id?: number | undefined;
                        name?: string | undefined;
                        bindings?: {
                            behaviorId?: number | undefined;
                            param1?: number | undefined;
                            param2?: number | undefined;
                        }[] | undefined;
                    }[] | undefined;
                    availableLayers?: number | undefined;
                    maxLayerNameLength?: number | undefined;
                } | undefined;
                err?: import("./keymap").MoveLayerErrorCode | undefined;
            } | undefined;
            addLayer?: {
                ok?: {
                    index?: number | undefined;
                    layer?: {
                        id?: number | undefined;
                        name?: string | undefined;
                        bindings?: {
                            behaviorId?: number | undefined;
                            param1?: number | undefined;
                            param2?: number | undefined;
                        }[] | undefined;
                    } | undefined;
                } | undefined;
                err?: import("./keymap").AddLayerErrorCode | undefined;
            } | undefined;
            removeLayer?: {
                ok?: {} | undefined;
                err?: import("./keymap").RemoveLayerErrorCode | undefined;
            } | undefined;
            restoreLayer?: {
                ok?: {
                    id?: number | undefined;
                    name?: string | undefined;
                    bindings?: {
                        behaviorId?: number | undefined;
                        param1?: number | undefined;
                        param2?: number | undefined;
                    }[] | undefined;
                } | undefined;
                err?: import("./keymap").RestoreLayerErrorCode | undefined;
            } | undefined;
            setLayerProps?: import("./keymap").SetLayerPropsResponse | undefined;
        } | undefined;
        custom?: {
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
        } | undefined;
    } & {
        requestId?: number | undefined;
        meta?: ({
            noResponse?: boolean | undefined;
            simpleError?: import("./meta").ErrorConditions | undefined;
        } & {
            noResponse?: boolean | undefined;
            simpleError?: import("./meta").ErrorConditions | undefined;
        } & { [K_63 in Exclude<keyof I_1["meta"], keyof Response5>]: never; }) | undefined;
        core?: ({
            getDeviceInfo?: {
                name?: string | undefined;
                serialNumber?: Uint8Array | undefined;
            } | undefined;
            getLockState?: import("./core").LockState | undefined;
            resetSettings?: boolean | undefined;
        } & {
            getDeviceInfo?: ({
                name?: string | undefined;
                serialNumber?: Uint8Array | undefined;
            } & {
                name?: string | undefined;
                serialNumber?: Uint8Array | undefined;
            } & { [K_64 in Exclude<keyof I_1["core"]["getDeviceInfo"], keyof import("./core").GetDeviceInfoResponse>]: never; }) | undefined;
            getLockState?: import("./core").LockState | undefined;
            resetSettings?: boolean | undefined;
        } & { [K_65 in Exclude<keyof I_1["core"], keyof Response6>]: never; }) | undefined;
        behaviors?: ({
            listAllBehaviors?: {
                behaviors?: number[] | undefined;
            } | undefined;
            getBehaviorDetails?: {
                id?: number | undefined;
                displayName?: string | undefined;
                metadata?: {
                    param1?: {
                        name?: string | undefined;
                        nil?: {} | undefined;
                        constant?: number | undefined;
                        range?: {
                            min?: number | undefined;
                            max?: number | undefined;
                        } | undefined;
                        hidUsage?: {
                            keyboardMax?: number | undefined;
                            consumerMax?: number | undefined;
                        } | undefined;
                        layerId?: {} | undefined;
                    }[] | undefined;
                    param2?: {
                        name?: string | undefined;
                        nil?: {} | undefined;
                        constant?: number | undefined;
                        range?: {
                            min?: number | undefined;
                            max?: number | undefined;
                        } | undefined;
                        hidUsage?: {
                            keyboardMax?: number | undefined;
                            consumerMax?: number | undefined;
                        } | undefined;
                        layerId?: {} | undefined;
                    }[] | undefined;
                }[] | undefined;
            } | undefined;
        } & {
            listAllBehaviors?: ({
                behaviors?: number[] | undefined;
            } & {
                behaviors?: (number[] & number[] & { [K_66 in Exclude<keyof I_1["behaviors"]["listAllBehaviors"]["behaviors"], keyof number[]>]: never; }) | undefined;
            } & { [K_67 in Exclude<keyof I_1["behaviors"]["listAllBehaviors"], "behaviors">]: never; }) | undefined;
            getBehaviorDetails?: ({
                id?: number | undefined;
                displayName?: string | undefined;
                metadata?: {
                    param1?: {
                        name?: string | undefined;
                        nil?: {} | undefined;
                        constant?: number | undefined;
                        range?: {
                            min?: number | undefined;
                            max?: number | undefined;
                        } | undefined;
                        hidUsage?: {
                            keyboardMax?: number | undefined;
                            consumerMax?: number | undefined;
                        } | undefined;
                        layerId?: {} | undefined;
                    }[] | undefined;
                    param2?: {
                        name?: string | undefined;
                        nil?: {} | undefined;
                        constant?: number | undefined;
                        range?: {
                            min?: number | undefined;
                            max?: number | undefined;
                        } | undefined;
                        hidUsage?: {
                            keyboardMax?: number | undefined;
                            consumerMax?: number | undefined;
                        } | undefined;
                        layerId?: {} | undefined;
                    }[] | undefined;
                }[] | undefined;
            } & {
                id?: number | undefined;
                displayName?: string | undefined;
                metadata?: ({
                    param1?: {
                        name?: string | undefined;
                        nil?: {} | undefined;
                        constant?: number | undefined;
                        range?: {
                            min?: number | undefined;
                            max?: number | undefined;
                        } | undefined;
                        hidUsage?: {
                            keyboardMax?: number | undefined;
                            consumerMax?: number | undefined;
                        } | undefined;
                        layerId?: {} | undefined;
                    }[] | undefined;
                    param2?: {
                        name?: string | undefined;
                        nil?: {} | undefined;
                        constant?: number | undefined;
                        range?: {
                            min?: number | undefined;
                            max?: number | undefined;
                        } | undefined;
                        hidUsage?: {
                            keyboardMax?: number | undefined;
                            consumerMax?: number | undefined;
                        } | undefined;
                        layerId?: {} | undefined;
                    }[] | undefined;
                }[] & ({
                    param1?: {
                        name?: string | undefined;
                        nil?: {} | undefined;
                        constant?: number | undefined;
                        range?: {
                            min?: number | undefined;
                            max?: number | undefined;
                        } | undefined;
                        hidUsage?: {
                            keyboardMax?: number | undefined;
                            consumerMax?: number | undefined;
                        } | undefined;
                        layerId?: {} | undefined;
                    }[] | undefined;
                    param2?: {
                        name?: string | undefined;
                        nil?: {} | undefined;
                        constant?: number | undefined;
                        range?: {
                            min?: number | undefined;
                            max?: number | undefined;
                        } | undefined;
                        hidUsage?: {
                            keyboardMax?: number | undefined;
                            consumerMax?: number | undefined;
                        } | undefined;
                        layerId?: {} | undefined;
                    }[] | undefined;
                } & {
                    param1?: ({
                        name?: string | undefined;
                        nil?: {} | undefined;
                        constant?: number | undefined;
                        range?: {
                            min?: number | undefined;
                            max?: number | undefined;
                        } | undefined;
                        hidUsage?: {
                            keyboardMax?: number | undefined;
                            consumerMax?: number | undefined;
                        } | undefined;
                        layerId?: {} | undefined;
                    }[] & ({
                        name?: string | undefined;
                        nil?: {} | undefined;
                        constant?: number | undefined;
                        range?: {
                            min?: number | undefined;
                            max?: number | undefined;
                        } | undefined;
                        hidUsage?: {
                            keyboardMax?: number | undefined;
                            consumerMax?: number | undefined;
                        } | undefined;
                        layerId?: {} | undefined;
                    } & {
                        name?: string | undefined;
                        nil?: ({} & {} & { [K_68 in Exclude<keyof I_1["behaviors"]["getBehaviorDetails"]["metadata"][number]["param1"][number]["nil"], never>]: never; }) | undefined;
                        constant?: number | undefined;
                        range?: ({
                            min?: number | undefined;
                            max?: number | undefined;
                        } & {
                            min?: number | undefined;
                            max?: number | undefined;
                        } & { [K_69 in Exclude<keyof I_1["behaviors"]["getBehaviorDetails"]["metadata"][number]["param1"][number]["range"], keyof import("./behaviors").BehaviorParameterValueDescriptionRange>]: never; }) | undefined;
                        hidUsage?: ({
                            keyboardMax?: number | undefined;
                            consumerMax?: number | undefined;
                        } & {
                            keyboardMax?: number | undefined;
                            consumerMax?: number | undefined;
                        } & { [K_70 in Exclude<keyof I_1["behaviors"]["getBehaviorDetails"]["metadata"][number]["param1"][number]["hidUsage"], keyof import("./behaviors").BehaviorParameterHidUsage>]: never; }) | undefined;
                        layerId?: ({} & {} & { [K_71 in Exclude<keyof I_1["behaviors"]["getBehaviorDetails"]["metadata"][number]["param1"][number]["layerId"], never>]: never; }) | undefined;
                    } & { [K_72 in Exclude<keyof I_1["behaviors"]["getBehaviorDetails"]["metadata"][number]["param1"][number], keyof import("./behaviors").BehaviorParameterValueDescription>]: never; })[] & { [K_73 in Exclude<keyof I_1["behaviors"]["getBehaviorDetails"]["metadata"][number]["param1"], keyof {
                        name?: string | undefined;
                        nil?: {} | undefined;
                        constant?: number | undefined;
                        range?: {
                            min?: number | undefined;
                            max?: number | undefined;
                        } | undefined;
                        hidUsage?: {
                            keyboardMax?: number | undefined;
                            consumerMax?: number | undefined;
                        } | undefined;
                        layerId?: {} | undefined;
                    }[]>]: never; }) | undefined;
                    param2?: ({
                        name?: string | undefined;
                        nil?: {} | undefined;
                        constant?: number | undefined;
                        range?: {
                            min?: number | undefined;
                            max?: number | undefined;
                        } | undefined;
                        hidUsage?: {
                            keyboardMax?: number | undefined;
                            consumerMax?: number | undefined;
                        } | undefined;
                        layerId?: {} | undefined;
                    }[] & ({
                        name?: string | undefined;
                        nil?: {} | undefined;
                        constant?: number | undefined;
                        range?: {
                            min?: number | undefined;
                            max?: number | undefined;
                        } | undefined;
                        hidUsage?: {
                            keyboardMax?: number | undefined;
                            consumerMax?: number | undefined;
                        } | undefined;
                        layerId?: {} | undefined;
                    } & {
                        name?: string | undefined;
                        nil?: ({} & {} & { [K_74 in Exclude<keyof I_1["behaviors"]["getBehaviorDetails"]["metadata"][number]["param2"][number]["nil"], never>]: never; }) | undefined;
                        constant?: number | undefined;
                        range?: ({
                            min?: number | undefined;
                            max?: number | undefined;
                        } & {
                            min?: number | undefined;
                            max?: number | undefined;
                        } & { [K_75 in Exclude<keyof I_1["behaviors"]["getBehaviorDetails"]["metadata"][number]["param2"][number]["range"], keyof import("./behaviors").BehaviorParameterValueDescriptionRange>]: never; }) | undefined;
                        hidUsage?: ({
                            keyboardMax?: number | undefined;
                            consumerMax?: number | undefined;
                        } & {
                            keyboardMax?: number | undefined;
                            consumerMax?: number | undefined;
                        } & { [K_76 in Exclude<keyof I_1["behaviors"]["getBehaviorDetails"]["metadata"][number]["param2"][number]["hidUsage"], keyof import("./behaviors").BehaviorParameterHidUsage>]: never; }) | undefined;
                        layerId?: ({} & {} & { [K_77 in Exclude<keyof I_1["behaviors"]["getBehaviorDetails"]["metadata"][number]["param2"][number]["layerId"], never>]: never; }) | undefined;
                    } & { [K_78 in Exclude<keyof I_1["behaviors"]["getBehaviorDetails"]["metadata"][number]["param2"][number], keyof import("./behaviors").BehaviorParameterValueDescription>]: never; })[] & { [K_79 in Exclude<keyof I_1["behaviors"]["getBehaviorDetails"]["metadata"][number]["param2"], keyof {
                        name?: string | undefined;
                        nil?: {} | undefined;
                        constant?: number | undefined;
                        range?: {
                            min?: number | undefined;
                            max?: number | undefined;
                        } | undefined;
                        hidUsage?: {
                            keyboardMax?: number | undefined;
                            consumerMax?: number | undefined;
                        } | undefined;
                        layerId?: {} | undefined;
                    }[]>]: never; }) | undefined;
                } & { [K_80 in Exclude<keyof I_1["behaviors"]["getBehaviorDetails"]["metadata"][number], keyof import("./behaviors").BehaviorBindingParametersSet>]: never; })[] & { [K_81 in Exclude<keyof I_1["behaviors"]["getBehaviorDetails"]["metadata"], keyof {
                    param1?: {
                        name?: string | undefined;
                        nil?: {} | undefined;
                        constant?: number | undefined;
                        range?: {
                            min?: number | undefined;
                            max?: number | undefined;
                        } | undefined;
                        hidUsage?: {
                            keyboardMax?: number | undefined;
                            consumerMax?: number | undefined;
                        } | undefined;
                        layerId?: {} | undefined;
                    }[] | undefined;
                    param2?: {
                        name?: string | undefined;
                        nil?: {} | undefined;
                        constant?: number | undefined;
                        range?: {
                            min?: number | undefined;
                            max?: number | undefined;
                        } | undefined;
                        hidUsage?: {
                            keyboardMax?: number | undefined;
                            consumerMax?: number | undefined;
                        } | undefined;
                        layerId?: {} | undefined;
                    }[] | undefined;
                }[]>]: never; }) | undefined;
            } & { [K_82 in Exclude<keyof I_1["behaviors"]["getBehaviorDetails"], keyof import("./behaviors").GetBehaviorDetailsResponse>]: never; }) | undefined;
        } & { [K_83 in Exclude<keyof I_1["behaviors"], keyof Response7>]: never; }) | undefined;
        keymap?: ({
            getKeymap?: {
                layers?: {
                    id?: number | undefined;
                    name?: string | undefined;
                    bindings?: {
                        behaviorId?: number | undefined;
                        param1?: number | undefined;
                        param2?: number | undefined;
                    }[] | undefined;
                }[] | undefined;
                availableLayers?: number | undefined;
                maxLayerNameLength?: number | undefined;
            } | undefined;
            setLayerBinding?: import("./keymap").SetLayerBindingResponse | undefined;
            checkUnsavedChanges?: boolean | undefined;
            saveChanges?: {
                ok?: boolean | undefined;
                err?: import("./keymap").SaveChangesErrorCode | undefined;
            } | undefined;
            discardChanges?: boolean | undefined;
            getPhysicalLayouts?: {
                activeLayoutIndex?: number | undefined;
                layouts?: {
                    name?: string | undefined;
                    keys?: {
                        width?: number | undefined;
                        height?: number | undefined;
                        x?: number | undefined;
                        y?: number | undefined;
                        r?: number | undefined;
                        rx?: number | undefined;
                        ry?: number | undefined;
                    }[] | undefined;
                }[] | undefined;
            } | undefined;
            setActivePhysicalLayout?: {
                ok?: {
                    layers?: {
                        id?: number | undefined;
                        name?: string | undefined;
                        bindings?: {
                            behaviorId?: number | undefined;
                            param1?: number | undefined;
                            param2?: number | undefined;
                        }[] | undefined;
                    }[] | undefined;
                    availableLayers?: number | undefined;
                    maxLayerNameLength?: number | undefined;
                } | undefined;
                err?: import("./keymap").SetActivePhysicalLayoutErrorCode | undefined;
            } | undefined;
            moveLayer?: {
                ok?: {
                    layers?: {
                        id?: number | undefined;
                        name?: string | undefined;
                        bindings?: {
                            behaviorId?: number | undefined;
                            param1?: number | undefined;
                            param2?: number | undefined;
                        }[] | undefined;
                    }[] | undefined;
                    availableLayers?: number | undefined;
                    maxLayerNameLength?: number | undefined;
                } | undefined;
                err?: import("./keymap").MoveLayerErrorCode | undefined;
            } | undefined;
            addLayer?: {
                ok?: {
                    index?: number | undefined;
                    layer?: {
                        id?: number | undefined;
                        name?: string | undefined;
                        bindings?: {
                            behaviorId?: number | undefined;
                            param1?: number | undefined;
                            param2?: number | undefined;
                        }[] | undefined;
                    } | undefined;
                } | undefined;
                err?: import("./keymap").AddLayerErrorCode | undefined;
            } | undefined;
            removeLayer?: {
                ok?: {} | undefined;
                err?: import("./keymap").RemoveLayerErrorCode | undefined;
            } | undefined;
            restoreLayer?: {
                ok?: {
                    id?: number | undefined;
                    name?: string | undefined;
                    bindings?: {
                        behaviorId?: number | undefined;
                        param1?: number | undefined;
                        param2?: number | undefined;
                    }[] | undefined;
                } | undefined;
                err?: import("./keymap").RestoreLayerErrorCode | undefined;
            } | undefined;
            setLayerProps?: import("./keymap").SetLayerPropsResponse | undefined;
        } & {
            getKeymap?: ({
                layers?: {
                    id?: number | undefined;
                    name?: string | undefined;
                    bindings?: {
                        behaviorId?: number | undefined;
                        param1?: number | undefined;
                        param2?: number | undefined;
                    }[] | undefined;
                }[] | undefined;
                availableLayers?: number | undefined;
                maxLayerNameLength?: number | undefined;
            } & {
                layers?: ({
                    id?: number | undefined;
                    name?: string | undefined;
                    bindings?: {
                        behaviorId?: number | undefined;
                        param1?: number | undefined;
                        param2?: number | undefined;
                    }[] | undefined;
                }[] & ({
                    id?: number | undefined;
                    name?: string | undefined;
                    bindings?: {
                        behaviorId?: number | undefined;
                        param1?: number | undefined;
                        param2?: number | undefined;
                    }[] | undefined;
                } & {
                    id?: number | undefined;
                    name?: string | undefined;
                    bindings?: ({
                        behaviorId?: number | undefined;
                        param1?: number | undefined;
                        param2?: number | undefined;
                    }[] & ({
                        behaviorId?: number | undefined;
                        param1?: number | undefined;
                        param2?: number | undefined;
                    } & {
                        behaviorId?: number | undefined;
                        param1?: number | undefined;
                        param2?: number | undefined;
                    } & { [K_84 in Exclude<keyof I_1["keymap"]["getKeymap"]["layers"][number]["bindings"][number], keyof import("./keymap").BehaviorBinding>]: never; })[] & { [K_85 in Exclude<keyof I_1["keymap"]["getKeymap"]["layers"][number]["bindings"], keyof {
                        behaviorId?: number | undefined;
                        param1?: number | undefined;
                        param2?: number | undefined;
                    }[]>]: never; }) | undefined;
                } & { [K_86 in Exclude<keyof I_1["keymap"]["getKeymap"]["layers"][number], keyof import("./keymap").Layer>]: never; })[] & { [K_87 in Exclude<keyof I_1["keymap"]["getKeymap"]["layers"], keyof {
                    id?: number | undefined;
                    name?: string | undefined;
                    bindings?: {
                        behaviorId?: number | undefined;
                        param1?: number | undefined;
                        param2?: number | undefined;
                    }[] | undefined;
                }[]>]: never; }) | undefined;
                availableLayers?: number | undefined;
                maxLayerNameLength?: number | undefined;
            } & { [K_88 in Exclude<keyof I_1["keymap"]["getKeymap"], keyof import("./keymap").Keymap>]: never; }) | undefined;
            setLayerBinding?: import("./keymap").SetLayerBindingResponse | undefined;
            checkUnsavedChanges?: boolean | undefined;
            saveChanges?: ({
                ok?: boolean | undefined;
                err?: import("./keymap").SaveChangesErrorCode | undefined;
            } & {
                ok?: boolean | undefined;
                err?: import("./keymap").SaveChangesErrorCode | undefined;
            } & { [K_89 in Exclude<keyof I_1["keymap"]["saveChanges"], keyof import("./keymap").SaveChangesResponse>]: never; }) | undefined;
            discardChanges?: boolean | undefined;
            getPhysicalLayouts?: ({
                activeLayoutIndex?: number | undefined;
                layouts?: {
                    name?: string | undefined;
                    keys?: {
                        width?: number | undefined;
                        height?: number | undefined;
                        x?: number | undefined;
                        y?: number | undefined;
                        r?: number | undefined;
                        rx?: number | undefined;
                        ry?: number | undefined;
                    }[] | undefined;
                }[] | undefined;
            } & {
                activeLayoutIndex?: number | undefined;
                layouts?: ({
                    name?: string | undefined;
                    keys?: {
                        width?: number | undefined;
                        height?: number | undefined;
                        x?: number | undefined;
                        y?: number | undefined;
                        r?: number | undefined;
                        rx?: number | undefined;
                        ry?: number | undefined;
                    }[] | undefined;
                }[] & ({
                    name?: string | undefined;
                    keys?: {
                        width?: number | undefined;
                        height?: number | undefined;
                        x?: number | undefined;
                        y?: number | undefined;
                        r?: number | undefined;
                        rx?: number | undefined;
                        ry?: number | undefined;
                    }[] | undefined;
                } & {
                    name?: string | undefined;
                    keys?: ({
                        width?: number | undefined;
                        height?: number | undefined;
                        x?: number | undefined;
                        y?: number | undefined;
                        r?: number | undefined;
                        rx?: number | undefined;
                        ry?: number | undefined;
                    }[] & ({
                        width?: number | undefined;
                        height?: number | undefined;
                        x?: number | undefined;
                        y?: number | undefined;
                        r?: number | undefined;
                        rx?: number | undefined;
                        ry?: number | undefined;
                    } & {
                        width?: number | undefined;
                        height?: number | undefined;
                        x?: number | undefined;
                        y?: number | undefined;
                        r?: number | undefined;
                        rx?: number | undefined;
                        ry?: number | undefined;
                    } & { [K_90 in Exclude<keyof I_1["keymap"]["getPhysicalLayouts"]["layouts"][number]["keys"][number], keyof import("./keymap").KeyPhysicalAttrs>]: never; })[] & { [K_91 in Exclude<keyof I_1["keymap"]["getPhysicalLayouts"]["layouts"][number]["keys"], keyof {
                        width?: number | undefined;
                        height?: number | undefined;
                        x?: number | undefined;
                        y?: number | undefined;
                        r?: number | undefined;
                        rx?: number | undefined;
                        ry?: number | undefined;
                    }[]>]: never; }) | undefined;
                } & { [K_92 in Exclude<keyof I_1["keymap"]["getPhysicalLayouts"]["layouts"][number], keyof import("./keymap").PhysicalLayout>]: never; })[] & { [K_93 in Exclude<keyof I_1["keymap"]["getPhysicalLayouts"]["layouts"], keyof {
                    name?: string | undefined;
                    keys?: {
                        width?: number | undefined;
                        height?: number | undefined;
                        x?: number | undefined;
                        y?: number | undefined;
                        r?: number | undefined;
                        rx?: number | undefined;
                        ry?: number | undefined;
                    }[] | undefined;
                }[]>]: never; }) | undefined;
            } & { [K_94 in Exclude<keyof I_1["keymap"]["getPhysicalLayouts"], keyof import("./keymap").PhysicalLayouts>]: never; }) | undefined;
            setActivePhysicalLayout?: ({
                ok?: {
                    layers?: {
                        id?: number | undefined;
                        name?: string | undefined;
                        bindings?: {
                            behaviorId?: number | undefined;
                            param1?: number | undefined;
                            param2?: number | undefined;
                        }[] | undefined;
                    }[] | undefined;
                    availableLayers?: number | undefined;
                    maxLayerNameLength?: number | undefined;
                } | undefined;
                err?: import("./keymap").SetActivePhysicalLayoutErrorCode | undefined;
            } & {
                ok?: ({
                    layers?: {
                        id?: number | undefined;
                        name?: string | undefined;
                        bindings?: {
                            behaviorId?: number | undefined;
                            param1?: number | undefined;
                            param2?: number | undefined;
                        }[] | undefined;
                    }[] | undefined;
                    availableLayers?: number | undefined;
                    maxLayerNameLength?: number | undefined;
                } & {
                    layers?: ({
                        id?: number | undefined;
                        name?: string | undefined;
                        bindings?: {
                            behaviorId?: number | undefined;
                            param1?: number | undefined;
                            param2?: number | undefined;
                        }[] | undefined;
                    }[] & ({
                        id?: number | undefined;
                        name?: string | undefined;
                        bindings?: {
                            behaviorId?: number | undefined;
                            param1?: number | undefined;
                            param2?: number | undefined;
                        }[] | undefined;
                    } & {
                        id?: number | undefined;
                        name?: string | undefined;
                        bindings?: ({
                            behaviorId?: number | undefined;
                            param1?: number | undefined;
                            param2?: number | undefined;
                        }[] & ({
                            behaviorId?: number | undefined;
                            param1?: number | undefined;
                            param2?: number | undefined;
                        } & {
                            behaviorId?: number | undefined;
                            param1?: number | undefined;
                            param2?: number | undefined;
                        } & { [K_95 in Exclude<keyof I_1["keymap"]["setActivePhysicalLayout"]["ok"]["layers"][number]["bindings"][number], keyof import("./keymap").BehaviorBinding>]: never; })[] & { [K_96 in Exclude<keyof I_1["keymap"]["setActivePhysicalLayout"]["ok"]["layers"][number]["bindings"], keyof {
                            behaviorId?: number | undefined;
                            param1?: number | undefined;
                            param2?: number | undefined;
                        }[]>]: never; }) | undefined;
                    } & { [K_97 in Exclude<keyof I_1["keymap"]["setActivePhysicalLayout"]["ok"]["layers"][number], keyof import("./keymap").Layer>]: never; })[] & { [K_98 in Exclude<keyof I_1["keymap"]["setActivePhysicalLayout"]["ok"]["layers"], keyof {
                        id?: number | undefined;
                        name?: string | undefined;
                        bindings?: {
                            behaviorId?: number | undefined;
                            param1?: number | undefined;
                            param2?: number | undefined;
                        }[] | undefined;
                    }[]>]: never; }) | undefined;
                    availableLayers?: number | undefined;
                    maxLayerNameLength?: number | undefined;
                } & { [K_99 in Exclude<keyof I_1["keymap"]["setActivePhysicalLayout"]["ok"], keyof import("./keymap").Keymap>]: never; }) | undefined;
                err?: import("./keymap").SetActivePhysicalLayoutErrorCode | undefined;
            } & { [K_100 in Exclude<keyof I_1["keymap"]["setActivePhysicalLayout"], keyof import("./keymap").SetActivePhysicalLayoutResponse>]: never; }) | undefined;
            moveLayer?: ({
                ok?: {
                    layers?: {
                        id?: number | undefined;
                        name?: string | undefined;
                        bindings?: {
                            behaviorId?: number | undefined;
                            param1?: number | undefined;
                            param2?: number | undefined;
                        }[] | undefined;
                    }[] | undefined;
                    availableLayers?: number | undefined;
                    maxLayerNameLength?: number | undefined;
                } | undefined;
                err?: import("./keymap").MoveLayerErrorCode | undefined;
            } & {
                ok?: ({
                    layers?: {
                        id?: number | undefined;
                        name?: string | undefined;
                        bindings?: {
                            behaviorId?: number | undefined;
                            param1?: number | undefined;
                            param2?: number | undefined;
                        }[] | undefined;
                    }[] | undefined;
                    availableLayers?: number | undefined;
                    maxLayerNameLength?: number | undefined;
                } & {
                    layers?: ({
                        id?: number | undefined;
                        name?: string | undefined;
                        bindings?: {
                            behaviorId?: number | undefined;
                            param1?: number | undefined;
                            param2?: number | undefined;
                        }[] | undefined;
                    }[] & ({
                        id?: number | undefined;
                        name?: string | undefined;
                        bindings?: {
                            behaviorId?: number | undefined;
                            param1?: number | undefined;
                            param2?: number | undefined;
                        }[] | undefined;
                    } & {
                        id?: number | undefined;
                        name?: string | undefined;
                        bindings?: ({
                            behaviorId?: number | undefined;
                            param1?: number | undefined;
                            param2?: number | undefined;
                        }[] & ({
                            behaviorId?: number | undefined;
                            param1?: number | undefined;
                            param2?: number | undefined;
                        } & {
                            behaviorId?: number | undefined;
                            param1?: number | undefined;
                            param2?: number | undefined;
                        } & { [K_101 in Exclude<keyof I_1["keymap"]["moveLayer"]["ok"]["layers"][number]["bindings"][number], keyof import("./keymap").BehaviorBinding>]: never; })[] & { [K_102 in Exclude<keyof I_1["keymap"]["moveLayer"]["ok"]["layers"][number]["bindings"], keyof {
                            behaviorId?: number | undefined;
                            param1?: number | undefined;
                            param2?: number | undefined;
                        }[]>]: never; }) | undefined;
                    } & { [K_103 in Exclude<keyof I_1["keymap"]["moveLayer"]["ok"]["layers"][number], keyof import("./keymap").Layer>]: never; })[] & { [K_104 in Exclude<keyof I_1["keymap"]["moveLayer"]["ok"]["layers"], keyof {
                        id?: number | undefined;
                        name?: string | undefined;
                        bindings?: {
                            behaviorId?: number | undefined;
                            param1?: number | undefined;
                            param2?: number | undefined;
                        }[] | undefined;
                    }[]>]: never; }) | undefined;
                    availableLayers?: number | undefined;
                    maxLayerNameLength?: number | undefined;
                } & { [K_105 in Exclude<keyof I_1["keymap"]["moveLayer"]["ok"], keyof import("./keymap").Keymap>]: never; }) | undefined;
                err?: import("./keymap").MoveLayerErrorCode | undefined;
            } & { [K_106 in Exclude<keyof I_1["keymap"]["moveLayer"], keyof import("./keymap").MoveLayerResponse>]: never; }) | undefined;
            addLayer?: ({
                ok?: {
                    index?: number | undefined;
                    layer?: {
                        id?: number | undefined;
                        name?: string | undefined;
                        bindings?: {
                            behaviorId?: number | undefined;
                            param1?: number | undefined;
                            param2?: number | undefined;
                        }[] | undefined;
                    } | undefined;
                } | undefined;
                err?: import("./keymap").AddLayerErrorCode | undefined;
            } & {
                ok?: ({
                    index?: number | undefined;
                    layer?: {
                        id?: number | undefined;
                        name?: string | undefined;
                        bindings?: {
                            behaviorId?: number | undefined;
                            param1?: number | undefined;
                            param2?: number | undefined;
                        }[] | undefined;
                    } | undefined;
                } & {
                    index?: number | undefined;
                    layer?: ({
                        id?: number | undefined;
                        name?: string | undefined;
                        bindings?: {
                            behaviorId?: number | undefined;
                            param1?: number | undefined;
                            param2?: number | undefined;
                        }[] | undefined;
                    } & {
                        id?: number | undefined;
                        name?: string | undefined;
                        bindings?: ({
                            behaviorId?: number | undefined;
                            param1?: number | undefined;
                            param2?: number | undefined;
                        }[] & ({
                            behaviorId?: number | undefined;
                            param1?: number | undefined;
                            param2?: number | undefined;
                        } & {
                            behaviorId?: number | undefined;
                            param1?: number | undefined;
                            param2?: number | undefined;
                        } & { [K_107 in Exclude<keyof I_1["keymap"]["addLayer"]["ok"]["layer"]["bindings"][number], keyof import("./keymap").BehaviorBinding>]: never; })[] & { [K_108 in Exclude<keyof I_1["keymap"]["addLayer"]["ok"]["layer"]["bindings"], keyof {
                            behaviorId?: number | undefined;
                            param1?: number | undefined;
                            param2?: number | undefined;
                        }[]>]: never; }) | undefined;
                    } & { [K_109 in Exclude<keyof I_1["keymap"]["addLayer"]["ok"]["layer"], keyof import("./keymap").Layer>]: never; }) | undefined;
                } & { [K_110 in Exclude<keyof I_1["keymap"]["addLayer"]["ok"], keyof import("./keymap").AddLayerResponseDetails>]: never; }) | undefined;
                err?: import("./keymap").AddLayerErrorCode | undefined;
            } & { [K_111 in Exclude<keyof I_1["keymap"]["addLayer"], keyof import("./keymap").AddLayerResponse>]: never; }) | undefined;
            removeLayer?: ({
                ok?: {} | undefined;
                err?: import("./keymap").RemoveLayerErrorCode | undefined;
            } & {
                ok?: ({} & {} & { [K_112 in Exclude<keyof I_1["keymap"]["removeLayer"]["ok"], never>]: never; }) | undefined;
                err?: import("./keymap").RemoveLayerErrorCode | undefined;
            } & { [K_113 in Exclude<keyof I_1["keymap"]["removeLayer"], keyof import("./keymap").RemoveLayerResponse>]: never; }) | undefined;
            restoreLayer?: ({
                ok?: {
                    id?: number | undefined;
                    name?: string | undefined;
                    bindings?: {
                        behaviorId?: number | undefined;
                        param1?: number | undefined;
                        param2?: number | undefined;
                    }[] | undefined;
                } | undefined;
                err?: import("./keymap").RestoreLayerErrorCode | undefined;
            } & {
                ok?: ({
                    id?: number | undefined;
                    name?: string | undefined;
                    bindings?: {
                        behaviorId?: number | undefined;
                        param1?: number | undefined;
                        param2?: number | undefined;
                    }[] | undefined;
                } & {
                    id?: number | undefined;
                    name?: string | undefined;
                    bindings?: ({
                        behaviorId?: number | undefined;
                        param1?: number | undefined;
                        param2?: number | undefined;
                    }[] & ({
                        behaviorId?: number | undefined;
                        param1?: number | undefined;
                        param2?: number | undefined;
                    } & {
                        behaviorId?: number | undefined;
                        param1?: number | undefined;
                        param2?: number | undefined;
                    } & { [K_114 in Exclude<keyof I_1["keymap"]["restoreLayer"]["ok"]["bindings"][number], keyof import("./keymap").BehaviorBinding>]: never; })[] & { [K_115 in Exclude<keyof I_1["keymap"]["restoreLayer"]["ok"]["bindings"], keyof {
                        behaviorId?: number | undefined;
                        param1?: number | undefined;
                        param2?: number | undefined;
                    }[]>]: never; }) | undefined;
                } & { [K_116 in Exclude<keyof I_1["keymap"]["restoreLayer"]["ok"], keyof import("./keymap").Layer>]: never; }) | undefined;
                err?: import("./keymap").RestoreLayerErrorCode | undefined;
            } & { [K_117 in Exclude<keyof I_1["keymap"]["restoreLayer"], keyof import("./keymap").RestoreLayerResponse>]: never; }) | undefined;
            setLayerProps?: import("./keymap").SetLayerPropsResponse | undefined;
        } & { [K_118 in Exclude<keyof I_1["keymap"], keyof Response8>]: never; }) | undefined;
        custom?: ({
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
                    uiUrl?: (string[] & string[] & { [K_119 in Exclude<keyof I_1["custom"]["listCustomSubsystems"]["subsystems"][number]["uiUrl"], keyof string[]>]: never; }) | undefined;
                } & { [K_120 in Exclude<keyof I_1["custom"]["listCustomSubsystems"]["subsystems"][number], keyof import("./custom").CustomSubsystemInfo>]: never; })[] & { [K_121 in Exclude<keyof I_1["custom"]["listCustomSubsystems"]["subsystems"], keyof {
                    index?: number | undefined;
                    identifier?: string | undefined;
                    uiUrl?: string[] | undefined;
                }[]>]: never; }) | undefined;
            } & { [K_122 in Exclude<keyof I_1["custom"]["listCustomSubsystems"], "subsystems">]: never; }) | undefined;
            call?: ({
                subsystemIndex?: number | undefined;
                payload?: Uint8Array | undefined;
            } & {
                subsystemIndex?: number | undefined;
                payload?: Uint8Array | undefined;
            } & { [K_123 in Exclude<keyof I_1["custom"]["call"], keyof import("./custom").CallResponse>]: never; }) | undefined;
        } & { [K_124 in Exclude<keyof I_1["custom"], keyof Response9>]: never; }) | undefined;
    } & { [K_125 in Exclude<keyof I_1, keyof RequestResponse>]: never; }>(object: I_1): RequestResponse;
};
export declare const Notification: {
    encode(message: Notification, writer?: _m0.Writer): _m0.Writer;
    decode(input: _m0.Reader | Uint8Array, length?: number): Notification;
    fromJSON(object: any): Notification;
    toJSON(message: Notification): unknown;
    create<I extends {
        core?: {
            lockStateChanged?: import("./core").LockState | undefined;
        } | undefined;
        keymap?: {
            unsavedChangesStatusChanged?: boolean | undefined;
        } | undefined;
        custom?: {
            customNotification?: {
                subsystemIndex?: number | undefined;
                payload?: Uint8Array | undefined;
            } | undefined;
        } | undefined;
    } & {
        core?: ({
            lockStateChanged?: import("./core").LockState | undefined;
        } & {
            lockStateChanged?: import("./core").LockState | undefined;
        } & { [K in Exclude<keyof I["core"], "lockStateChanged">]: never; }) | undefined;
        keymap?: ({
            unsavedChangesStatusChanged?: boolean | undefined;
        } & {
            unsavedChangesStatusChanged?: boolean | undefined;
        } & { [K_1 in Exclude<keyof I["keymap"], "unsavedChangesStatusChanged">]: never; }) | undefined;
        custom?: ({
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
            } & { [K_2 in Exclude<keyof I["custom"]["customNotification"], keyof import("./custom").CustomNotification>]: never; }) | undefined;
        } & { [K_3 in Exclude<keyof I["custom"], "customNotification">]: never; }) | undefined;
    } & { [K_4 in Exclude<keyof I, keyof Notification>]: never; }>(base?: I | undefined): Notification;
    fromPartial<I_1 extends {
        core?: {
            lockStateChanged?: import("./core").LockState | undefined;
        } | undefined;
        keymap?: {
            unsavedChangesStatusChanged?: boolean | undefined;
        } | undefined;
        custom?: {
            customNotification?: {
                subsystemIndex?: number | undefined;
                payload?: Uint8Array | undefined;
            } | undefined;
        } | undefined;
    } & {
        core?: ({
            lockStateChanged?: import("./core").LockState | undefined;
        } & {
            lockStateChanged?: import("./core").LockState | undefined;
        } & { [K_5 in Exclude<keyof I_1["core"], "lockStateChanged">]: never; }) | undefined;
        keymap?: ({
            unsavedChangesStatusChanged?: boolean | undefined;
        } & {
            unsavedChangesStatusChanged?: boolean | undefined;
        } & { [K_6 in Exclude<keyof I_1["keymap"], "unsavedChangesStatusChanged">]: never; }) | undefined;
        custom?: ({
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
            } & { [K_7 in Exclude<keyof I_1["custom"]["customNotification"], keyof import("./custom").CustomNotification>]: never; }) | undefined;
        } & { [K_8 in Exclude<keyof I_1["custom"], "customNotification">]: never; }) | undefined;
    } & { [K_9 in Exclude<keyof I_1, keyof Notification>]: never; }>(object: I_1): Notification;
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
