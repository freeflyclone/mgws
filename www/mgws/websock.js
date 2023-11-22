import { appVersion } from "./index.js";

export var ws;

export function MakeWebSocket() {
    var wsUrl = "wss://" + window.location.host + "/websock";

    ws = new WebSocket(wsUrl);

    ws.onopen = (event) => {
        console.log("ws.onopen: ", ws.url);

        var regSession = {
            type: "RegisterSession",
            appVersion: appVersion,
        };
      
        ws.send(JSON.stringify(regSession));
    };
    ws.onmessage = (event) => {
        var msg = event.data;
        console.log("ws.onmessage: ", msg);
    };
    ws.onclose = (event) => {
        console.log("ws.onclose: ", event);
    }
    ws.onerror = (event) => {
        console.log("ws.onerror: ", event);
    }
    ws.sessionID = 0;
}

