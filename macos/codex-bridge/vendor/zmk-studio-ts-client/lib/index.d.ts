import { Request, Response, RequestResponse, Notification } from './studio';
import { RpcTransport } from './transport';
import { ErrorConditions } from './meta';
export { Request, RequestResponse, Response, Notification };
export interface RpcConnection {
    label: string;
    request_response_readable: ReadableStream<RequestResponse>;
    request_writable: WritableStream<Request>;
    notification_readable: ReadableStream<Notification>;
    current_request: number;
}
export interface CreateRpcConnectionOpts {
    signal?: AbortSignal;
}
export declare function create_rpc_connection(transport: RpcTransport, opts?: CreateRpcConnectionOpts): RpcConnection;
export declare class NoResponseError extends Error {
    constructor();
}
export declare class MetaError extends Error {
    readonly condition: ErrorConditions;
    constructor(condition: ErrorConditions);
}
export declare function call_rpc(conn: RpcConnection, req: Omit<Request, 'requestId'>): Promise<RequestResponse>;
