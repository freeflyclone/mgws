import { MakeWebSocket } from "/mgws/websock.js";
import { SendRegisterSession, SendGetActiveSessions } from "/mgws/ui.js";

export const appVersion = "0.1";

function main() {
    MakeWebSocket( (msg) => {
        SendRegisterSession();
        SendGetActiveSessions();
    });
}

Window.onload = main();