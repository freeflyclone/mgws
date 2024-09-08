import { MakeWebSocket } from "/mgws/websock.js";
import { RegisterSession, GetActiveSessions } from "/mgws/ui.js";

export const appVersion = "0.1";

function main() {
    console.log("location: " + window.location);

    MakeWebSocket( (msg) => {
        RegisterSession();
        GetActiveSessions();
    });
}

Window.onload = main();