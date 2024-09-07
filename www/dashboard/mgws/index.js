import { MakeWebSocket } from "/mgws/websock.js";
import { registerButton, RegisterSession } from "/mgws/ui.js";

export const appVersion = "0.1";

function main() {
    console.log("location: " + window.location);

    MakeWebSocket();

    registerButton.addEventListener('click', RegisterSession);
}

Window.onload = main();