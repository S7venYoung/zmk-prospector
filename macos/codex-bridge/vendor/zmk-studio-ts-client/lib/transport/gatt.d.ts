/// <reference types="web-bluetooth" />
import type { RpcTransport } from './';
export declare function connect(options?: Partial<RequestDeviceOptions>): Promise<RpcTransport>;
