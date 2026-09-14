import * as _m0 from "protobufjs/minimal";
export declare const protobufPackage = "zmk.keymap";
export declare enum SaveChangesErrorCode {
    SAVE_CHANGES_ERR_OK = 0,
    SAVE_CHANGES_ERR_GENERIC = 1,
    SAVE_CHANGES_ERR_NOT_SUPPORTED = 2,
    SAVE_CHANGES_ERR_NO_SPACE = 3,
    UNRECOGNIZED = -1
}
export declare function saveChangesErrorCodeFromJSON(object: any): SaveChangesErrorCode;
export declare function saveChangesErrorCodeToJSON(object: SaveChangesErrorCode): string;
export declare enum SetLayerBindingResponse {
    SET_LAYER_BINDING_RESP_OK = 0,
    SET_LAYER_BINDING_RESP_INVALID_LOCATION = 1,
    SET_LAYER_BINDING_RESP_INVALID_BEHAVIOR = 2,
    SET_LAYER_BINDING_RESP_INVALID_PARAMETERS = 3,
    UNRECOGNIZED = -1
}
export declare function setLayerBindingResponseFromJSON(object: any): SetLayerBindingResponse;
export declare function setLayerBindingResponseToJSON(object: SetLayerBindingResponse): string;
export declare enum MoveLayerErrorCode {
    MOVE_LAYER_ERR_OK = 0,
    MOVE_LAYER_ERR_GENERIC = 1,
    MOVE_LAYER_ERR_INVALID_LAYER = 2,
    MOVE_LAYER_ERR_INVALID_DESTINATION = 3,
    UNRECOGNIZED = -1
}
export declare function moveLayerErrorCodeFromJSON(object: any): MoveLayerErrorCode;
export declare function moveLayerErrorCodeToJSON(object: MoveLayerErrorCode): string;
export declare enum AddLayerErrorCode {
    ADD_LAYER_ERR_OK = 0,
    ADD_LAYER_ERR_GENERIC = 1,
    ADD_LAYER_ERR_NO_SPACE = 2,
    UNRECOGNIZED = -1
}
export declare function addLayerErrorCodeFromJSON(object: any): AddLayerErrorCode;
export declare function addLayerErrorCodeToJSON(object: AddLayerErrorCode): string;
export declare enum RemoveLayerErrorCode {
    REMOVE_LAYER_ERR_OK = 0,
    REMOVE_LAYER_ERR_GENERIC = 1,
    REMOVE_LAYER_ERR_INVALID_INDEX = 2,
    UNRECOGNIZED = -1
}
export declare function removeLayerErrorCodeFromJSON(object: any): RemoveLayerErrorCode;
export declare function removeLayerErrorCodeToJSON(object: RemoveLayerErrorCode): string;
export declare enum RestoreLayerErrorCode {
    RESTORE_LAYER_ERR_OK = 0,
    RESTORE_LAYER_ERR_GENERIC = 1,
    RESTORE_LAYER_ERR_INVALID_ID = 2,
    RESTORE_LAYER_ERR_INVALID_INDEX = 3,
    UNRECOGNIZED = -1
}
export declare function restoreLayerErrorCodeFromJSON(object: any): RestoreLayerErrorCode;
export declare function restoreLayerErrorCodeToJSON(object: RestoreLayerErrorCode): string;
export declare enum SetLayerPropsResponse {
    SET_LAYER_PROPS_RESP_OK = 0,
    SET_LAYER_PROPS_RESP_ERR_GENERIC = 1,
    SET_LAYER_PROPS_RESP_ERR_INVALID_ID = 2,
    UNRECOGNIZED = -1
}
export declare function setLayerPropsResponseFromJSON(object: any): SetLayerPropsResponse;
export declare function setLayerPropsResponseToJSON(object: SetLayerPropsResponse): string;
export declare enum SetActivePhysicalLayoutErrorCode {
    SET_ACTIVE_PHYSICAL_LAYOUT_ERR_OK = 0,
    SET_ACTIVE_PHYSICAL_LAYOUT_ERR_GENERIC = 1,
    SET_ACTIVE_PHYSICAL_LAYOUT_ERR_INVALID_LAYOUT_INDEX = 2,
    UNRECOGNIZED = -1
}
export declare function setActivePhysicalLayoutErrorCodeFromJSON(object: any): SetActivePhysicalLayoutErrorCode;
export declare function setActivePhysicalLayoutErrorCodeToJSON(object: SetActivePhysicalLayoutErrorCode): string;
export interface Request {
    getKeymap?: boolean | undefined;
    setLayerBinding?: SetLayerBindingRequest | undefined;
    checkUnsavedChanges?: boolean | undefined;
    saveChanges?: boolean | undefined;
    discardChanges?: boolean | undefined;
    getPhysicalLayouts?: boolean | undefined;
    setActivePhysicalLayout?: number | undefined;
    moveLayer?: MoveLayerRequest | undefined;
    addLayer?: AddLayerRequest | undefined;
    removeLayer?: RemoveLayerRequest | undefined;
    restoreLayer?: RestoreLayerRequest | undefined;
    setLayerProps?: SetLayerPropsRequest | undefined;
}
export interface Response {
    getKeymap?: Keymap | undefined;
    setLayerBinding?: SetLayerBindingResponse | undefined;
    checkUnsavedChanges?: boolean | undefined;
    saveChanges?: SaveChangesResponse | undefined;
    discardChanges?: boolean | undefined;
    getPhysicalLayouts?: PhysicalLayouts | undefined;
    setActivePhysicalLayout?: SetActivePhysicalLayoutResponse | undefined;
    moveLayer?: MoveLayerResponse | undefined;
    addLayer?: AddLayerResponse | undefined;
    removeLayer?: RemoveLayerResponse | undefined;
    restoreLayer?: RestoreLayerResponse | undefined;
    setLayerProps?: SetLayerPropsResponse | undefined;
}
export interface Notification {
    unsavedChangesStatusChanged?: boolean | undefined;
}
export interface SaveChangesResponse {
    ok?: boolean | undefined;
    err?: SaveChangesErrorCode | undefined;
}
export interface SetActivePhysicalLayoutResponse {
    ok?: Keymap | undefined;
    err?: SetActivePhysicalLayoutErrorCode | undefined;
}
export interface MoveLayerResponse {
    ok?: Keymap | undefined;
    err?: MoveLayerErrorCode | undefined;
}
export interface AddLayerResponse {
    ok?: AddLayerResponseDetails | undefined;
    err?: AddLayerErrorCode | undefined;
}
export interface AddLayerResponseDetails {
    index: number;
    layer: Layer | undefined;
}
export interface RemoveLayerResponse {
    ok?: RemoveLayerOk | undefined;
    err?: RemoveLayerErrorCode | undefined;
}
export interface RemoveLayerOk {
}
export interface RestoreLayerResponse {
    ok?: Layer | undefined;
    err?: RestoreLayerErrorCode | undefined;
}
export interface SetLayerBindingRequest {
    layerId: number;
    keyPosition: number;
    binding: BehaviorBinding | undefined;
}
export interface MoveLayerRequest {
    startIndex: number;
    destIndex: number;
}
export interface AddLayerRequest {
}
export interface RemoveLayerRequest {
    layerIndex: number;
}
export interface RestoreLayerRequest {
    layerId: number;
    atIndex: number;
}
export interface SetLayerPropsRequest {
    layerId: number;
    name: string;
}
export interface Keymap {
    layers: Layer[];
    availableLayers: number;
    maxLayerNameLength: number;
}
export interface Layer {
    id: number;
    name: string;
    bindings: BehaviorBinding[];
}
export interface BehaviorBinding {
    behaviorId: number;
    param1: number;
    param2: number;
}
export interface PhysicalLayouts {
    activeLayoutIndex: number;
    layouts: PhysicalLayout[];
}
export interface PhysicalLayout {
    name: string;
    keys: KeyPhysicalAttrs[];
}
export interface KeyPhysicalAttrs {
    width: number;
    height: number;
    x: number;
    y: number;
    r: number;
    rx: number;
    ry: number;
}
export declare const Request: {
    encode(message: Request, writer?: _m0.Writer): _m0.Writer;
    decode(input: _m0.Reader | Uint8Array, length?: number): Request;
    fromJSON(object: any): Request;
    toJSON(message: Request): unknown;
    create<I extends {
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
            } & { [K in Exclude<keyof I["setLayerBinding"]["binding"], keyof BehaviorBinding>]: never; }) | undefined;
        } & { [K_1 in Exclude<keyof I["setLayerBinding"], keyof SetLayerBindingRequest>]: never; }) | undefined;
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
        } & { [K_2 in Exclude<keyof I["moveLayer"], keyof MoveLayerRequest>]: never; }) | undefined;
        addLayer?: ({} & {} & { [K_3 in Exclude<keyof I["addLayer"], never>]: never; }) | undefined;
        removeLayer?: ({
            layerIndex?: number | undefined;
        } & {
            layerIndex?: number | undefined;
        } & { [K_4 in Exclude<keyof I["removeLayer"], "layerIndex">]: never; }) | undefined;
        restoreLayer?: ({
            layerId?: number | undefined;
            atIndex?: number | undefined;
        } & {
            layerId?: number | undefined;
            atIndex?: number | undefined;
        } & { [K_5 in Exclude<keyof I["restoreLayer"], keyof RestoreLayerRequest>]: never; }) | undefined;
        setLayerProps?: ({
            layerId?: number | undefined;
            name?: string | undefined;
        } & {
            layerId?: number | undefined;
            name?: string | undefined;
        } & { [K_6 in Exclude<keyof I["setLayerProps"], keyof SetLayerPropsRequest>]: never; }) | undefined;
    } & { [K_7 in Exclude<keyof I, keyof Request>]: never; }>(base?: I | undefined): Request;
    fromPartial<I_1 extends {
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
            } & { [K_8 in Exclude<keyof I_1["setLayerBinding"]["binding"], keyof BehaviorBinding>]: never; }) | undefined;
        } & { [K_9 in Exclude<keyof I_1["setLayerBinding"], keyof SetLayerBindingRequest>]: never; }) | undefined;
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
        } & { [K_10 in Exclude<keyof I_1["moveLayer"], keyof MoveLayerRequest>]: never; }) | undefined;
        addLayer?: ({} & {} & { [K_11 in Exclude<keyof I_1["addLayer"], never>]: never; }) | undefined;
        removeLayer?: ({
            layerIndex?: number | undefined;
        } & {
            layerIndex?: number | undefined;
        } & { [K_12 in Exclude<keyof I_1["removeLayer"], "layerIndex">]: never; }) | undefined;
        restoreLayer?: ({
            layerId?: number | undefined;
            atIndex?: number | undefined;
        } & {
            layerId?: number | undefined;
            atIndex?: number | undefined;
        } & { [K_13 in Exclude<keyof I_1["restoreLayer"], keyof RestoreLayerRequest>]: never; }) | undefined;
        setLayerProps?: ({
            layerId?: number | undefined;
            name?: string | undefined;
        } & {
            layerId?: number | undefined;
            name?: string | undefined;
        } & { [K_14 in Exclude<keyof I_1["setLayerProps"], keyof SetLayerPropsRequest>]: never; }) | undefined;
    } & { [K_15 in Exclude<keyof I_1, keyof Request>]: never; }>(object: I_1): Request;
};
export declare const Response: {
    encode(message: Response, writer?: _m0.Writer): _m0.Writer;
    decode(input: _m0.Reader | Uint8Array, length?: number): Response;
    fromJSON(object: any): Response;
    toJSON(message: Response): unknown;
    create<I extends {
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
        setLayerBinding?: SetLayerBindingResponse | undefined;
        checkUnsavedChanges?: boolean | undefined;
        saveChanges?: {
            ok?: boolean | undefined;
            err?: SaveChangesErrorCode | undefined;
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
            err?: SetActivePhysicalLayoutErrorCode | undefined;
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
            err?: MoveLayerErrorCode | undefined;
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
            err?: AddLayerErrorCode | undefined;
        } | undefined;
        removeLayer?: {
            ok?: {} | undefined;
            err?: RemoveLayerErrorCode | undefined;
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
            err?: RestoreLayerErrorCode | undefined;
        } | undefined;
        setLayerProps?: SetLayerPropsResponse | undefined;
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
                } & { [K in Exclude<keyof I["getKeymap"]["layers"][number]["bindings"][number], keyof BehaviorBinding>]: never; })[] & { [K_1 in Exclude<keyof I["getKeymap"]["layers"][number]["bindings"], keyof {
                    behaviorId?: number | undefined;
                    param1?: number | undefined;
                    param2?: number | undefined;
                }[]>]: never; }) | undefined;
            } & { [K_2 in Exclude<keyof I["getKeymap"]["layers"][number], keyof Layer>]: never; })[] & { [K_3 in Exclude<keyof I["getKeymap"]["layers"], keyof {
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
        } & { [K_4 in Exclude<keyof I["getKeymap"], keyof Keymap>]: never; }) | undefined;
        setLayerBinding?: SetLayerBindingResponse | undefined;
        checkUnsavedChanges?: boolean | undefined;
        saveChanges?: ({
            ok?: boolean | undefined;
            err?: SaveChangesErrorCode | undefined;
        } & {
            ok?: boolean | undefined;
            err?: SaveChangesErrorCode | undefined;
        } & { [K_5 in Exclude<keyof I["saveChanges"], keyof SaveChangesResponse>]: never; }) | undefined;
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
                } & { [K_6 in Exclude<keyof I["getPhysicalLayouts"]["layouts"][number]["keys"][number], keyof KeyPhysicalAttrs>]: never; })[] & { [K_7 in Exclude<keyof I["getPhysicalLayouts"]["layouts"][number]["keys"], keyof {
                    width?: number | undefined;
                    height?: number | undefined;
                    x?: number | undefined;
                    y?: number | undefined;
                    r?: number | undefined;
                    rx?: number | undefined;
                    ry?: number | undefined;
                }[]>]: never; }) | undefined;
            } & { [K_8 in Exclude<keyof I["getPhysicalLayouts"]["layouts"][number], keyof PhysicalLayout>]: never; })[] & { [K_9 in Exclude<keyof I["getPhysicalLayouts"]["layouts"], keyof {
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
        } & { [K_10 in Exclude<keyof I["getPhysicalLayouts"], keyof PhysicalLayouts>]: never; }) | undefined;
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
            err?: SetActivePhysicalLayoutErrorCode | undefined;
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
                    } & { [K_11 in Exclude<keyof I["setActivePhysicalLayout"]["ok"]["layers"][number]["bindings"][number], keyof BehaviorBinding>]: never; })[] & { [K_12 in Exclude<keyof I["setActivePhysicalLayout"]["ok"]["layers"][number]["bindings"], keyof {
                        behaviorId?: number | undefined;
                        param1?: number | undefined;
                        param2?: number | undefined;
                    }[]>]: never; }) | undefined;
                } & { [K_13 in Exclude<keyof I["setActivePhysicalLayout"]["ok"]["layers"][number], keyof Layer>]: never; })[] & { [K_14 in Exclude<keyof I["setActivePhysicalLayout"]["ok"]["layers"], keyof {
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
            } & { [K_15 in Exclude<keyof I["setActivePhysicalLayout"]["ok"], keyof Keymap>]: never; }) | undefined;
            err?: SetActivePhysicalLayoutErrorCode | undefined;
        } & { [K_16 in Exclude<keyof I["setActivePhysicalLayout"], keyof SetActivePhysicalLayoutResponse>]: never; }) | undefined;
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
            err?: MoveLayerErrorCode | undefined;
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
                    } & { [K_17 in Exclude<keyof I["moveLayer"]["ok"]["layers"][number]["bindings"][number], keyof BehaviorBinding>]: never; })[] & { [K_18 in Exclude<keyof I["moveLayer"]["ok"]["layers"][number]["bindings"], keyof {
                        behaviorId?: number | undefined;
                        param1?: number | undefined;
                        param2?: number | undefined;
                    }[]>]: never; }) | undefined;
                } & { [K_19 in Exclude<keyof I["moveLayer"]["ok"]["layers"][number], keyof Layer>]: never; })[] & { [K_20 in Exclude<keyof I["moveLayer"]["ok"]["layers"], keyof {
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
            } & { [K_21 in Exclude<keyof I["moveLayer"]["ok"], keyof Keymap>]: never; }) | undefined;
            err?: MoveLayerErrorCode | undefined;
        } & { [K_22 in Exclude<keyof I["moveLayer"], keyof MoveLayerResponse>]: never; }) | undefined;
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
            err?: AddLayerErrorCode | undefined;
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
                    } & { [K_23 in Exclude<keyof I["addLayer"]["ok"]["layer"]["bindings"][number], keyof BehaviorBinding>]: never; })[] & { [K_24 in Exclude<keyof I["addLayer"]["ok"]["layer"]["bindings"], keyof {
                        behaviorId?: number | undefined;
                        param1?: number | undefined;
                        param2?: number | undefined;
                    }[]>]: never; }) | undefined;
                } & { [K_25 in Exclude<keyof I["addLayer"]["ok"]["layer"], keyof Layer>]: never; }) | undefined;
            } & { [K_26 in Exclude<keyof I["addLayer"]["ok"], keyof AddLayerResponseDetails>]: never; }) | undefined;
            err?: AddLayerErrorCode | undefined;
        } & { [K_27 in Exclude<keyof I["addLayer"], keyof AddLayerResponse>]: never; }) | undefined;
        removeLayer?: ({
            ok?: {} | undefined;
            err?: RemoveLayerErrorCode | undefined;
        } & {
            ok?: ({} & {} & { [K_28 in Exclude<keyof I["removeLayer"]["ok"], never>]: never; }) | undefined;
            err?: RemoveLayerErrorCode | undefined;
        } & { [K_29 in Exclude<keyof I["removeLayer"], keyof RemoveLayerResponse>]: never; }) | undefined;
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
            err?: RestoreLayerErrorCode | undefined;
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
                } & { [K_30 in Exclude<keyof I["restoreLayer"]["ok"]["bindings"][number], keyof BehaviorBinding>]: never; })[] & { [K_31 in Exclude<keyof I["restoreLayer"]["ok"]["bindings"], keyof {
                    behaviorId?: number | undefined;
                    param1?: number | undefined;
                    param2?: number | undefined;
                }[]>]: never; }) | undefined;
            } & { [K_32 in Exclude<keyof I["restoreLayer"]["ok"], keyof Layer>]: never; }) | undefined;
            err?: RestoreLayerErrorCode | undefined;
        } & { [K_33 in Exclude<keyof I["restoreLayer"], keyof RestoreLayerResponse>]: never; }) | undefined;
        setLayerProps?: SetLayerPropsResponse | undefined;
    } & { [K_34 in Exclude<keyof I, keyof Response>]: never; }>(base?: I | undefined): Response;
    fromPartial<I_1 extends {
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
        setLayerBinding?: SetLayerBindingResponse | undefined;
        checkUnsavedChanges?: boolean | undefined;
        saveChanges?: {
            ok?: boolean | undefined;
            err?: SaveChangesErrorCode | undefined;
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
            err?: SetActivePhysicalLayoutErrorCode | undefined;
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
            err?: MoveLayerErrorCode | undefined;
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
            err?: AddLayerErrorCode | undefined;
        } | undefined;
        removeLayer?: {
            ok?: {} | undefined;
            err?: RemoveLayerErrorCode | undefined;
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
            err?: RestoreLayerErrorCode | undefined;
        } | undefined;
        setLayerProps?: SetLayerPropsResponse | undefined;
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
                } & { [K_35 in Exclude<keyof I_1["getKeymap"]["layers"][number]["bindings"][number], keyof BehaviorBinding>]: never; })[] & { [K_36 in Exclude<keyof I_1["getKeymap"]["layers"][number]["bindings"], keyof {
                    behaviorId?: number | undefined;
                    param1?: number | undefined;
                    param2?: number | undefined;
                }[]>]: never; }) | undefined;
            } & { [K_37 in Exclude<keyof I_1["getKeymap"]["layers"][number], keyof Layer>]: never; })[] & { [K_38 in Exclude<keyof I_1["getKeymap"]["layers"], keyof {
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
        } & { [K_39 in Exclude<keyof I_1["getKeymap"], keyof Keymap>]: never; }) | undefined;
        setLayerBinding?: SetLayerBindingResponse | undefined;
        checkUnsavedChanges?: boolean | undefined;
        saveChanges?: ({
            ok?: boolean | undefined;
            err?: SaveChangesErrorCode | undefined;
        } & {
            ok?: boolean | undefined;
            err?: SaveChangesErrorCode | undefined;
        } & { [K_40 in Exclude<keyof I_1["saveChanges"], keyof SaveChangesResponse>]: never; }) | undefined;
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
                } & { [K_41 in Exclude<keyof I_1["getPhysicalLayouts"]["layouts"][number]["keys"][number], keyof KeyPhysicalAttrs>]: never; })[] & { [K_42 in Exclude<keyof I_1["getPhysicalLayouts"]["layouts"][number]["keys"], keyof {
                    width?: number | undefined;
                    height?: number | undefined;
                    x?: number | undefined;
                    y?: number | undefined;
                    r?: number | undefined;
                    rx?: number | undefined;
                    ry?: number | undefined;
                }[]>]: never; }) | undefined;
            } & { [K_43 in Exclude<keyof I_1["getPhysicalLayouts"]["layouts"][number], keyof PhysicalLayout>]: never; })[] & { [K_44 in Exclude<keyof I_1["getPhysicalLayouts"]["layouts"], keyof {
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
        } & { [K_45 in Exclude<keyof I_1["getPhysicalLayouts"], keyof PhysicalLayouts>]: never; }) | undefined;
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
            err?: SetActivePhysicalLayoutErrorCode | undefined;
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
                    } & { [K_46 in Exclude<keyof I_1["setActivePhysicalLayout"]["ok"]["layers"][number]["bindings"][number], keyof BehaviorBinding>]: never; })[] & { [K_47 in Exclude<keyof I_1["setActivePhysicalLayout"]["ok"]["layers"][number]["bindings"], keyof {
                        behaviorId?: number | undefined;
                        param1?: number | undefined;
                        param2?: number | undefined;
                    }[]>]: never; }) | undefined;
                } & { [K_48 in Exclude<keyof I_1["setActivePhysicalLayout"]["ok"]["layers"][number], keyof Layer>]: never; })[] & { [K_49 in Exclude<keyof I_1["setActivePhysicalLayout"]["ok"]["layers"], keyof {
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
            } & { [K_50 in Exclude<keyof I_1["setActivePhysicalLayout"]["ok"], keyof Keymap>]: never; }) | undefined;
            err?: SetActivePhysicalLayoutErrorCode | undefined;
        } & { [K_51 in Exclude<keyof I_1["setActivePhysicalLayout"], keyof SetActivePhysicalLayoutResponse>]: never; }) | undefined;
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
            err?: MoveLayerErrorCode | undefined;
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
                    } & { [K_52 in Exclude<keyof I_1["moveLayer"]["ok"]["layers"][number]["bindings"][number], keyof BehaviorBinding>]: never; })[] & { [K_53 in Exclude<keyof I_1["moveLayer"]["ok"]["layers"][number]["bindings"], keyof {
                        behaviorId?: number | undefined;
                        param1?: number | undefined;
                        param2?: number | undefined;
                    }[]>]: never; }) | undefined;
                } & { [K_54 in Exclude<keyof I_1["moveLayer"]["ok"]["layers"][number], keyof Layer>]: never; })[] & { [K_55 in Exclude<keyof I_1["moveLayer"]["ok"]["layers"], keyof {
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
            } & { [K_56 in Exclude<keyof I_1["moveLayer"]["ok"], keyof Keymap>]: never; }) | undefined;
            err?: MoveLayerErrorCode | undefined;
        } & { [K_57 in Exclude<keyof I_1["moveLayer"], keyof MoveLayerResponse>]: never; }) | undefined;
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
            err?: AddLayerErrorCode | undefined;
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
                    } & { [K_58 in Exclude<keyof I_1["addLayer"]["ok"]["layer"]["bindings"][number], keyof BehaviorBinding>]: never; })[] & { [K_59 in Exclude<keyof I_1["addLayer"]["ok"]["layer"]["bindings"], keyof {
                        behaviorId?: number | undefined;
                        param1?: number | undefined;
                        param2?: number | undefined;
                    }[]>]: never; }) | undefined;
                } & { [K_60 in Exclude<keyof I_1["addLayer"]["ok"]["layer"], keyof Layer>]: never; }) | undefined;
            } & { [K_61 in Exclude<keyof I_1["addLayer"]["ok"], keyof AddLayerResponseDetails>]: never; }) | undefined;
            err?: AddLayerErrorCode | undefined;
        } & { [K_62 in Exclude<keyof I_1["addLayer"], keyof AddLayerResponse>]: never; }) | undefined;
        removeLayer?: ({
            ok?: {} | undefined;
            err?: RemoveLayerErrorCode | undefined;
        } & {
            ok?: ({} & {} & { [K_63 in Exclude<keyof I_1["removeLayer"]["ok"], never>]: never; }) | undefined;
            err?: RemoveLayerErrorCode | undefined;
        } & { [K_64 in Exclude<keyof I_1["removeLayer"], keyof RemoveLayerResponse>]: never; }) | undefined;
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
            err?: RestoreLayerErrorCode | undefined;
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
                } & { [K_65 in Exclude<keyof I_1["restoreLayer"]["ok"]["bindings"][number], keyof BehaviorBinding>]: never; })[] & { [K_66 in Exclude<keyof I_1["restoreLayer"]["ok"]["bindings"], keyof {
                    behaviorId?: number | undefined;
                    param1?: number | undefined;
                    param2?: number | undefined;
                }[]>]: never; }) | undefined;
            } & { [K_67 in Exclude<keyof I_1["restoreLayer"]["ok"], keyof Layer>]: never; }) | undefined;
            err?: RestoreLayerErrorCode | undefined;
        } & { [K_68 in Exclude<keyof I_1["restoreLayer"], keyof RestoreLayerResponse>]: never; }) | undefined;
        setLayerProps?: SetLayerPropsResponse | undefined;
    } & { [K_69 in Exclude<keyof I_1, keyof Response>]: never; }>(object: I_1): Response;
};
export declare const Notification: {
    encode(message: Notification, writer?: _m0.Writer): _m0.Writer;
    decode(input: _m0.Reader | Uint8Array, length?: number): Notification;
    fromJSON(object: any): Notification;
    toJSON(message: Notification): unknown;
    create<I extends {
        unsavedChangesStatusChanged?: boolean | undefined;
    } & {
        unsavedChangesStatusChanged?: boolean | undefined;
    } & { [K in Exclude<keyof I, "unsavedChangesStatusChanged">]: never; }>(base?: I | undefined): Notification;
    fromPartial<I_1 extends {
        unsavedChangesStatusChanged?: boolean | undefined;
    } & {
        unsavedChangesStatusChanged?: boolean | undefined;
    } & { [K_1 in Exclude<keyof I_1, "unsavedChangesStatusChanged">]: never; }>(object: I_1): Notification;
};
export declare const SaveChangesResponse: {
    encode(message: SaveChangesResponse, writer?: _m0.Writer): _m0.Writer;
    decode(input: _m0.Reader | Uint8Array, length?: number): SaveChangesResponse;
    fromJSON(object: any): SaveChangesResponse;
    toJSON(message: SaveChangesResponse): unknown;
    create<I extends {
        ok?: boolean | undefined;
        err?: SaveChangesErrorCode | undefined;
    } & {
        ok?: boolean | undefined;
        err?: SaveChangesErrorCode | undefined;
    } & { [K in Exclude<keyof I, keyof SaveChangesResponse>]: never; }>(base?: I | undefined): SaveChangesResponse;
    fromPartial<I_1 extends {
        ok?: boolean | undefined;
        err?: SaveChangesErrorCode | undefined;
    } & {
        ok?: boolean | undefined;
        err?: SaveChangesErrorCode | undefined;
    } & { [K_1 in Exclude<keyof I_1, keyof SaveChangesResponse>]: never; }>(object: I_1): SaveChangesResponse;
};
export declare const SetActivePhysicalLayoutResponse: {
    encode(message: SetActivePhysicalLayoutResponse, writer?: _m0.Writer): _m0.Writer;
    decode(input: _m0.Reader | Uint8Array, length?: number): SetActivePhysicalLayoutResponse;
    fromJSON(object: any): SetActivePhysicalLayoutResponse;
    toJSON(message: SetActivePhysicalLayoutResponse): unknown;
    create<I extends {
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
        err?: SetActivePhysicalLayoutErrorCode | undefined;
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
                } & { [K in Exclude<keyof I["ok"]["layers"][number]["bindings"][number], keyof BehaviorBinding>]: never; })[] & { [K_1 in Exclude<keyof I["ok"]["layers"][number]["bindings"], keyof {
                    behaviorId?: number | undefined;
                    param1?: number | undefined;
                    param2?: number | undefined;
                }[]>]: never; }) | undefined;
            } & { [K_2 in Exclude<keyof I["ok"]["layers"][number], keyof Layer>]: never; })[] & { [K_3 in Exclude<keyof I["ok"]["layers"], keyof {
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
        } & { [K_4 in Exclude<keyof I["ok"], keyof Keymap>]: never; }) | undefined;
        err?: SetActivePhysicalLayoutErrorCode | undefined;
    } & { [K_5 in Exclude<keyof I, keyof SetActivePhysicalLayoutResponse>]: never; }>(base?: I | undefined): SetActivePhysicalLayoutResponse;
    fromPartial<I_1 extends {
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
        err?: SetActivePhysicalLayoutErrorCode | undefined;
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
                } & { [K_6 in Exclude<keyof I_1["ok"]["layers"][number]["bindings"][number], keyof BehaviorBinding>]: never; })[] & { [K_7 in Exclude<keyof I_1["ok"]["layers"][number]["bindings"], keyof {
                    behaviorId?: number | undefined;
                    param1?: number | undefined;
                    param2?: number | undefined;
                }[]>]: never; }) | undefined;
            } & { [K_8 in Exclude<keyof I_1["ok"]["layers"][number], keyof Layer>]: never; })[] & { [K_9 in Exclude<keyof I_1["ok"]["layers"], keyof {
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
        } & { [K_10 in Exclude<keyof I_1["ok"], keyof Keymap>]: never; }) | undefined;
        err?: SetActivePhysicalLayoutErrorCode | undefined;
    } & { [K_11 in Exclude<keyof I_1, keyof SetActivePhysicalLayoutResponse>]: never; }>(object: I_1): SetActivePhysicalLayoutResponse;
};
export declare const MoveLayerResponse: {
    encode(message: MoveLayerResponse, writer?: _m0.Writer): _m0.Writer;
    decode(input: _m0.Reader | Uint8Array, length?: number): MoveLayerResponse;
    fromJSON(object: any): MoveLayerResponse;
    toJSON(message: MoveLayerResponse): unknown;
    create<I extends {
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
        err?: MoveLayerErrorCode | undefined;
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
                } & { [K in Exclude<keyof I["ok"]["layers"][number]["bindings"][number], keyof BehaviorBinding>]: never; })[] & { [K_1 in Exclude<keyof I["ok"]["layers"][number]["bindings"], keyof {
                    behaviorId?: number | undefined;
                    param1?: number | undefined;
                    param2?: number | undefined;
                }[]>]: never; }) | undefined;
            } & { [K_2 in Exclude<keyof I["ok"]["layers"][number], keyof Layer>]: never; })[] & { [K_3 in Exclude<keyof I["ok"]["layers"], keyof {
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
        } & { [K_4 in Exclude<keyof I["ok"], keyof Keymap>]: never; }) | undefined;
        err?: MoveLayerErrorCode | undefined;
    } & { [K_5 in Exclude<keyof I, keyof MoveLayerResponse>]: never; }>(base?: I | undefined): MoveLayerResponse;
    fromPartial<I_1 extends {
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
        err?: MoveLayerErrorCode | undefined;
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
                } & { [K_6 in Exclude<keyof I_1["ok"]["layers"][number]["bindings"][number], keyof BehaviorBinding>]: never; })[] & { [K_7 in Exclude<keyof I_1["ok"]["layers"][number]["bindings"], keyof {
                    behaviorId?: number | undefined;
                    param1?: number | undefined;
                    param2?: number | undefined;
                }[]>]: never; }) | undefined;
            } & { [K_8 in Exclude<keyof I_1["ok"]["layers"][number], keyof Layer>]: never; })[] & { [K_9 in Exclude<keyof I_1["ok"]["layers"], keyof {
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
        } & { [K_10 in Exclude<keyof I_1["ok"], keyof Keymap>]: never; }) | undefined;
        err?: MoveLayerErrorCode | undefined;
    } & { [K_11 in Exclude<keyof I_1, keyof MoveLayerResponse>]: never; }>(object: I_1): MoveLayerResponse;
};
export declare const AddLayerResponse: {
    encode(message: AddLayerResponse, writer?: _m0.Writer): _m0.Writer;
    decode(input: _m0.Reader | Uint8Array, length?: number): AddLayerResponse;
    fromJSON(object: any): AddLayerResponse;
    toJSON(message: AddLayerResponse): unknown;
    create<I extends {
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
        err?: AddLayerErrorCode | undefined;
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
                } & { [K in Exclude<keyof I["ok"]["layer"]["bindings"][number], keyof BehaviorBinding>]: never; })[] & { [K_1 in Exclude<keyof I["ok"]["layer"]["bindings"], keyof {
                    behaviorId?: number | undefined;
                    param1?: number | undefined;
                    param2?: number | undefined;
                }[]>]: never; }) | undefined;
            } & { [K_2 in Exclude<keyof I["ok"]["layer"], keyof Layer>]: never; }) | undefined;
        } & { [K_3 in Exclude<keyof I["ok"], keyof AddLayerResponseDetails>]: never; }) | undefined;
        err?: AddLayerErrorCode | undefined;
    } & { [K_4 in Exclude<keyof I, keyof AddLayerResponse>]: never; }>(base?: I | undefined): AddLayerResponse;
    fromPartial<I_1 extends {
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
        err?: AddLayerErrorCode | undefined;
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
                } & { [K_5 in Exclude<keyof I_1["ok"]["layer"]["bindings"][number], keyof BehaviorBinding>]: never; })[] & { [K_6 in Exclude<keyof I_1["ok"]["layer"]["bindings"], keyof {
                    behaviorId?: number | undefined;
                    param1?: number | undefined;
                    param2?: number | undefined;
                }[]>]: never; }) | undefined;
            } & { [K_7 in Exclude<keyof I_1["ok"]["layer"], keyof Layer>]: never; }) | undefined;
        } & { [K_8 in Exclude<keyof I_1["ok"], keyof AddLayerResponseDetails>]: never; }) | undefined;
        err?: AddLayerErrorCode | undefined;
    } & { [K_9 in Exclude<keyof I_1, keyof AddLayerResponse>]: never; }>(object: I_1): AddLayerResponse;
};
export declare const AddLayerResponseDetails: {
    encode(message: AddLayerResponseDetails, writer?: _m0.Writer): _m0.Writer;
    decode(input: _m0.Reader | Uint8Array, length?: number): AddLayerResponseDetails;
    fromJSON(object: any): AddLayerResponseDetails;
    toJSON(message: AddLayerResponseDetails): unknown;
    create<I extends {
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
            } & { [K in Exclude<keyof I["layer"]["bindings"][number], keyof BehaviorBinding>]: never; })[] & { [K_1 in Exclude<keyof I["layer"]["bindings"], keyof {
                behaviorId?: number | undefined;
                param1?: number | undefined;
                param2?: number | undefined;
            }[]>]: never; }) | undefined;
        } & { [K_2 in Exclude<keyof I["layer"], keyof Layer>]: never; }) | undefined;
    } & { [K_3 in Exclude<keyof I, keyof AddLayerResponseDetails>]: never; }>(base?: I | undefined): AddLayerResponseDetails;
    fromPartial<I_1 extends {
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
            } & { [K_4 in Exclude<keyof I_1["layer"]["bindings"][number], keyof BehaviorBinding>]: never; })[] & { [K_5 in Exclude<keyof I_1["layer"]["bindings"], keyof {
                behaviorId?: number | undefined;
                param1?: number | undefined;
                param2?: number | undefined;
            }[]>]: never; }) | undefined;
        } & { [K_6 in Exclude<keyof I_1["layer"], keyof Layer>]: never; }) | undefined;
    } & { [K_7 in Exclude<keyof I_1, keyof AddLayerResponseDetails>]: never; }>(object: I_1): AddLayerResponseDetails;
};
export declare const RemoveLayerResponse: {
    encode(message: RemoveLayerResponse, writer?: _m0.Writer): _m0.Writer;
    decode(input: _m0.Reader | Uint8Array, length?: number): RemoveLayerResponse;
    fromJSON(object: any): RemoveLayerResponse;
    toJSON(message: RemoveLayerResponse): unknown;
    create<I extends {
        ok?: {} | undefined;
        err?: RemoveLayerErrorCode | undefined;
    } & {
        ok?: ({} & {} & { [K in Exclude<keyof I["ok"], never>]: never; }) | undefined;
        err?: RemoveLayerErrorCode | undefined;
    } & { [K_1 in Exclude<keyof I, keyof RemoveLayerResponse>]: never; }>(base?: I | undefined): RemoveLayerResponse;
    fromPartial<I_1 extends {
        ok?: {} | undefined;
        err?: RemoveLayerErrorCode | undefined;
    } & {
        ok?: ({} & {} & { [K_2 in Exclude<keyof I_1["ok"], never>]: never; }) | undefined;
        err?: RemoveLayerErrorCode | undefined;
    } & { [K_3 in Exclude<keyof I_1, keyof RemoveLayerResponse>]: never; }>(object: I_1): RemoveLayerResponse;
};
export declare const RemoveLayerOk: {
    encode(_: RemoveLayerOk, writer?: _m0.Writer): _m0.Writer;
    decode(input: _m0.Reader | Uint8Array, length?: number): RemoveLayerOk;
    fromJSON(_: any): RemoveLayerOk;
    toJSON(_: RemoveLayerOk): unknown;
    create<I extends {} & {} & { [K in Exclude<keyof I, never>]: never; }>(base?: I | undefined): RemoveLayerOk;
    fromPartial<I_1 extends {} & {} & { [K_1 in Exclude<keyof I_1, never>]: never; }>(_: I_1): RemoveLayerOk;
};
export declare const RestoreLayerResponse: {
    encode(message: RestoreLayerResponse, writer?: _m0.Writer): _m0.Writer;
    decode(input: _m0.Reader | Uint8Array, length?: number): RestoreLayerResponse;
    fromJSON(object: any): RestoreLayerResponse;
    toJSON(message: RestoreLayerResponse): unknown;
    create<I extends {
        ok?: {
            id?: number | undefined;
            name?: string | undefined;
            bindings?: {
                behaviorId?: number | undefined;
                param1?: number | undefined;
                param2?: number | undefined;
            }[] | undefined;
        } | undefined;
        err?: RestoreLayerErrorCode | undefined;
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
            } & { [K in Exclude<keyof I["ok"]["bindings"][number], keyof BehaviorBinding>]: never; })[] & { [K_1 in Exclude<keyof I["ok"]["bindings"], keyof {
                behaviorId?: number | undefined;
                param1?: number | undefined;
                param2?: number | undefined;
            }[]>]: never; }) | undefined;
        } & { [K_2 in Exclude<keyof I["ok"], keyof Layer>]: never; }) | undefined;
        err?: RestoreLayerErrorCode | undefined;
    } & { [K_3 in Exclude<keyof I, keyof RestoreLayerResponse>]: never; }>(base?: I | undefined): RestoreLayerResponse;
    fromPartial<I_1 extends {
        ok?: {
            id?: number | undefined;
            name?: string | undefined;
            bindings?: {
                behaviorId?: number | undefined;
                param1?: number | undefined;
                param2?: number | undefined;
            }[] | undefined;
        } | undefined;
        err?: RestoreLayerErrorCode | undefined;
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
            } & { [K_4 in Exclude<keyof I_1["ok"]["bindings"][number], keyof BehaviorBinding>]: never; })[] & { [K_5 in Exclude<keyof I_1["ok"]["bindings"], keyof {
                behaviorId?: number | undefined;
                param1?: number | undefined;
                param2?: number | undefined;
            }[]>]: never; }) | undefined;
        } & { [K_6 in Exclude<keyof I_1["ok"], keyof Layer>]: never; }) | undefined;
        err?: RestoreLayerErrorCode | undefined;
    } & { [K_7 in Exclude<keyof I_1, keyof RestoreLayerResponse>]: never; }>(object: I_1): RestoreLayerResponse;
};
export declare const SetLayerBindingRequest: {
    encode(message: SetLayerBindingRequest, writer?: _m0.Writer): _m0.Writer;
    decode(input: _m0.Reader | Uint8Array, length?: number): SetLayerBindingRequest;
    fromJSON(object: any): SetLayerBindingRequest;
    toJSON(message: SetLayerBindingRequest): unknown;
    create<I extends {
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
        } & { [K in Exclude<keyof I["binding"], keyof BehaviorBinding>]: never; }) | undefined;
    } & { [K_1 in Exclude<keyof I, keyof SetLayerBindingRequest>]: never; }>(base?: I | undefined): SetLayerBindingRequest;
    fromPartial<I_1 extends {
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
        } & { [K_2 in Exclude<keyof I_1["binding"], keyof BehaviorBinding>]: never; }) | undefined;
    } & { [K_3 in Exclude<keyof I_1, keyof SetLayerBindingRequest>]: never; }>(object: I_1): SetLayerBindingRequest;
};
export declare const MoveLayerRequest: {
    encode(message: MoveLayerRequest, writer?: _m0.Writer): _m0.Writer;
    decode(input: _m0.Reader | Uint8Array, length?: number): MoveLayerRequest;
    fromJSON(object: any): MoveLayerRequest;
    toJSON(message: MoveLayerRequest): unknown;
    create<I extends {
        startIndex?: number | undefined;
        destIndex?: number | undefined;
    } & {
        startIndex?: number | undefined;
        destIndex?: number | undefined;
    } & { [K in Exclude<keyof I, keyof MoveLayerRequest>]: never; }>(base?: I | undefined): MoveLayerRequest;
    fromPartial<I_1 extends {
        startIndex?: number | undefined;
        destIndex?: number | undefined;
    } & {
        startIndex?: number | undefined;
        destIndex?: number | undefined;
    } & { [K_1 in Exclude<keyof I_1, keyof MoveLayerRequest>]: never; }>(object: I_1): MoveLayerRequest;
};
export declare const AddLayerRequest: {
    encode(_: AddLayerRequest, writer?: _m0.Writer): _m0.Writer;
    decode(input: _m0.Reader | Uint8Array, length?: number): AddLayerRequest;
    fromJSON(_: any): AddLayerRequest;
    toJSON(_: AddLayerRequest): unknown;
    create<I extends {} & {} & { [K in Exclude<keyof I, never>]: never; }>(base?: I | undefined): AddLayerRequest;
    fromPartial<I_1 extends {} & {} & { [K_1 in Exclude<keyof I_1, never>]: never; }>(_: I_1): AddLayerRequest;
};
export declare const RemoveLayerRequest: {
    encode(message: RemoveLayerRequest, writer?: _m0.Writer): _m0.Writer;
    decode(input: _m0.Reader | Uint8Array, length?: number): RemoveLayerRequest;
    fromJSON(object: any): RemoveLayerRequest;
    toJSON(message: RemoveLayerRequest): unknown;
    create<I extends {
        layerIndex?: number | undefined;
    } & {
        layerIndex?: number | undefined;
    } & { [K in Exclude<keyof I, "layerIndex">]: never; }>(base?: I | undefined): RemoveLayerRequest;
    fromPartial<I_1 extends {
        layerIndex?: number | undefined;
    } & {
        layerIndex?: number | undefined;
    } & { [K_1 in Exclude<keyof I_1, "layerIndex">]: never; }>(object: I_1): RemoveLayerRequest;
};
export declare const RestoreLayerRequest: {
    encode(message: RestoreLayerRequest, writer?: _m0.Writer): _m0.Writer;
    decode(input: _m0.Reader | Uint8Array, length?: number): RestoreLayerRequest;
    fromJSON(object: any): RestoreLayerRequest;
    toJSON(message: RestoreLayerRequest): unknown;
    create<I extends {
        layerId?: number | undefined;
        atIndex?: number | undefined;
    } & {
        layerId?: number | undefined;
        atIndex?: number | undefined;
    } & { [K in Exclude<keyof I, keyof RestoreLayerRequest>]: never; }>(base?: I | undefined): RestoreLayerRequest;
    fromPartial<I_1 extends {
        layerId?: number | undefined;
        atIndex?: number | undefined;
    } & {
        layerId?: number | undefined;
        atIndex?: number | undefined;
    } & { [K_1 in Exclude<keyof I_1, keyof RestoreLayerRequest>]: never; }>(object: I_1): RestoreLayerRequest;
};
export declare const SetLayerPropsRequest: {
    encode(message: SetLayerPropsRequest, writer?: _m0.Writer): _m0.Writer;
    decode(input: _m0.Reader | Uint8Array, length?: number): SetLayerPropsRequest;
    fromJSON(object: any): SetLayerPropsRequest;
    toJSON(message: SetLayerPropsRequest): unknown;
    create<I extends {
        layerId?: number | undefined;
        name?: string | undefined;
    } & {
        layerId?: number | undefined;
        name?: string | undefined;
    } & { [K in Exclude<keyof I, keyof SetLayerPropsRequest>]: never; }>(base?: I | undefined): SetLayerPropsRequest;
    fromPartial<I_1 extends {
        layerId?: number | undefined;
        name?: string | undefined;
    } & {
        layerId?: number | undefined;
        name?: string | undefined;
    } & { [K_1 in Exclude<keyof I_1, keyof SetLayerPropsRequest>]: never; }>(object: I_1): SetLayerPropsRequest;
};
export declare const Keymap: {
    encode(message: Keymap, writer?: _m0.Writer): _m0.Writer;
    decode(input: _m0.Reader | Uint8Array, length?: number): Keymap;
    fromJSON(object: any): Keymap;
    toJSON(message: Keymap): unknown;
    create<I extends {
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
            } & { [K in Exclude<keyof I["layers"][number]["bindings"][number], keyof BehaviorBinding>]: never; })[] & { [K_1 in Exclude<keyof I["layers"][number]["bindings"], keyof {
                behaviorId?: number | undefined;
                param1?: number | undefined;
                param2?: number | undefined;
            }[]>]: never; }) | undefined;
        } & { [K_2 in Exclude<keyof I["layers"][number], keyof Layer>]: never; })[] & { [K_3 in Exclude<keyof I["layers"], keyof {
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
    } & { [K_4 in Exclude<keyof I, keyof Keymap>]: never; }>(base?: I | undefined): Keymap;
    fromPartial<I_1 extends {
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
            } & { [K_5 in Exclude<keyof I_1["layers"][number]["bindings"][number], keyof BehaviorBinding>]: never; })[] & { [K_6 in Exclude<keyof I_1["layers"][number]["bindings"], keyof {
                behaviorId?: number | undefined;
                param1?: number | undefined;
                param2?: number | undefined;
            }[]>]: never; }) | undefined;
        } & { [K_7 in Exclude<keyof I_1["layers"][number], keyof Layer>]: never; })[] & { [K_8 in Exclude<keyof I_1["layers"], keyof {
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
    } & { [K_9 in Exclude<keyof I_1, keyof Keymap>]: never; }>(object: I_1): Keymap;
};
export declare const Layer: {
    encode(message: Layer, writer?: _m0.Writer): _m0.Writer;
    decode(input: _m0.Reader | Uint8Array, length?: number): Layer;
    fromJSON(object: any): Layer;
    toJSON(message: Layer): unknown;
    create<I extends {
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
        } & { [K in Exclude<keyof I["bindings"][number], keyof BehaviorBinding>]: never; })[] & { [K_1 in Exclude<keyof I["bindings"], keyof {
            behaviorId?: number | undefined;
            param1?: number | undefined;
            param2?: number | undefined;
        }[]>]: never; }) | undefined;
    } & { [K_2 in Exclude<keyof I, keyof Layer>]: never; }>(base?: I | undefined): Layer;
    fromPartial<I_1 extends {
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
        } & { [K_3 in Exclude<keyof I_1["bindings"][number], keyof BehaviorBinding>]: never; })[] & { [K_4 in Exclude<keyof I_1["bindings"], keyof {
            behaviorId?: number | undefined;
            param1?: number | undefined;
            param2?: number | undefined;
        }[]>]: never; }) | undefined;
    } & { [K_5 in Exclude<keyof I_1, keyof Layer>]: never; }>(object: I_1): Layer;
};
export declare const BehaviorBinding: {
    encode(message: BehaviorBinding, writer?: _m0.Writer): _m0.Writer;
    decode(input: _m0.Reader | Uint8Array, length?: number): BehaviorBinding;
    fromJSON(object: any): BehaviorBinding;
    toJSON(message: BehaviorBinding): unknown;
    create<I extends {
        behaviorId?: number | undefined;
        param1?: number | undefined;
        param2?: number | undefined;
    } & {
        behaviorId?: number | undefined;
        param1?: number | undefined;
        param2?: number | undefined;
    } & { [K in Exclude<keyof I, keyof BehaviorBinding>]: never; }>(base?: I | undefined): BehaviorBinding;
    fromPartial<I_1 extends {
        behaviorId?: number | undefined;
        param1?: number | undefined;
        param2?: number | undefined;
    } & {
        behaviorId?: number | undefined;
        param1?: number | undefined;
        param2?: number | undefined;
    } & { [K_1 in Exclude<keyof I_1, keyof BehaviorBinding>]: never; }>(object: I_1): BehaviorBinding;
};
export declare const PhysicalLayouts: {
    encode(message: PhysicalLayouts, writer?: _m0.Writer): _m0.Writer;
    decode(input: _m0.Reader | Uint8Array, length?: number): PhysicalLayouts;
    fromJSON(object: any): PhysicalLayouts;
    toJSON(message: PhysicalLayouts): unknown;
    create<I extends {
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
            } & { [K in Exclude<keyof I["layouts"][number]["keys"][number], keyof KeyPhysicalAttrs>]: never; })[] & { [K_1 in Exclude<keyof I["layouts"][number]["keys"], keyof {
                width?: number | undefined;
                height?: number | undefined;
                x?: number | undefined;
                y?: number | undefined;
                r?: number | undefined;
                rx?: number | undefined;
                ry?: number | undefined;
            }[]>]: never; }) | undefined;
        } & { [K_2 in Exclude<keyof I["layouts"][number], keyof PhysicalLayout>]: never; })[] & { [K_3 in Exclude<keyof I["layouts"], keyof {
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
    } & { [K_4 in Exclude<keyof I, keyof PhysicalLayouts>]: never; }>(base?: I | undefined): PhysicalLayouts;
    fromPartial<I_1 extends {
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
            } & { [K_5 in Exclude<keyof I_1["layouts"][number]["keys"][number], keyof KeyPhysicalAttrs>]: never; })[] & { [K_6 in Exclude<keyof I_1["layouts"][number]["keys"], keyof {
                width?: number | undefined;
                height?: number | undefined;
                x?: number | undefined;
                y?: number | undefined;
                r?: number | undefined;
                rx?: number | undefined;
                ry?: number | undefined;
            }[]>]: never; }) | undefined;
        } & { [K_7 in Exclude<keyof I_1["layouts"][number], keyof PhysicalLayout>]: never; })[] & { [K_8 in Exclude<keyof I_1["layouts"], keyof {
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
    } & { [K_9 in Exclude<keyof I_1, keyof PhysicalLayouts>]: never; }>(object: I_1): PhysicalLayouts;
};
export declare const PhysicalLayout: {
    encode(message: PhysicalLayout, writer?: _m0.Writer): _m0.Writer;
    decode(input: _m0.Reader | Uint8Array, length?: number): PhysicalLayout;
    fromJSON(object: any): PhysicalLayout;
    toJSON(message: PhysicalLayout): unknown;
    create<I extends {
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
        } & { [K in Exclude<keyof I["keys"][number], keyof KeyPhysicalAttrs>]: never; })[] & { [K_1 in Exclude<keyof I["keys"], keyof {
            width?: number | undefined;
            height?: number | undefined;
            x?: number | undefined;
            y?: number | undefined;
            r?: number | undefined;
            rx?: number | undefined;
            ry?: number | undefined;
        }[]>]: never; }) | undefined;
    } & { [K_2 in Exclude<keyof I, keyof PhysicalLayout>]: never; }>(base?: I | undefined): PhysicalLayout;
    fromPartial<I_1 extends {
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
        } & { [K_3 in Exclude<keyof I_1["keys"][number], keyof KeyPhysicalAttrs>]: never; })[] & { [K_4 in Exclude<keyof I_1["keys"], keyof {
            width?: number | undefined;
            height?: number | undefined;
            x?: number | undefined;
            y?: number | undefined;
            r?: number | undefined;
            rx?: number | undefined;
            ry?: number | undefined;
        }[]>]: never; }) | undefined;
    } & { [K_5 in Exclude<keyof I_1, keyof PhysicalLayout>]: never; }>(object: I_1): PhysicalLayout;
};
export declare const KeyPhysicalAttrs: {
    encode(message: KeyPhysicalAttrs, writer?: _m0.Writer): _m0.Writer;
    decode(input: _m0.Reader | Uint8Array, length?: number): KeyPhysicalAttrs;
    fromJSON(object: any): KeyPhysicalAttrs;
    toJSON(message: KeyPhysicalAttrs): unknown;
    create<I extends {
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
    } & { [K in Exclude<keyof I, keyof KeyPhysicalAttrs>]: never; }>(base?: I | undefined): KeyPhysicalAttrs;
    fromPartial<I_1 extends {
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
    } & { [K_1 in Exclude<keyof I_1, keyof KeyPhysicalAttrs>]: never; }>(object: I_1): KeyPhysicalAttrs;
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
