export class UserCancelledError extends Error {
    constructor(m, opts) {
        super(m, opts);
        // Set the prototype explicitly.
        Object.setPrototypeOf(this, UserCancelledError.prototype);
    }
}
