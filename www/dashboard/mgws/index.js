import { MakeWebSocket } from "/mgws/websock.js";

function main() {
    console.log("location: " + window.location);

    MakeWebSocket();
}

Window.onload = main();