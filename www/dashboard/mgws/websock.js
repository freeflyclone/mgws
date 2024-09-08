import { RegisterSession, GetActiveSessions } from "/mgws/ui.js";

export var ws;

var OnNewSession;

export function MakeWebSocket(ons) {
    // The MGWS WebSocket handler is invoked with "websock" added to the host URL 
    var wsUrl = "wss://localhost/websock";

    console.log("MakeWebSocket()\n");

    ws = new WebSocket(wsUrl);

    ws.onopen = OnOpen; 
    ws.onmessage = OnMessage;
    ws.onclose = OnClose;
    ws.onerror = OnError;

    ws.sessionID = 0;

    OnNewSession = ons;
}

function OnOpen(event) {
    console.log("OnOpen: ", ws.url);
};

function OnMessage(event) {
    var msg = JSON.parse(event.data);

    switch(msg.type) {
        case "SessionID":
            SessionID(msg);
            break;

        default:
            break;
    }
}

function OnClose(event) {
    console.log("OnClose: ", event);
    if (event.wasClean != true) {
        console.log("Unexpected loss of WebSocket server.");
    }
}

function OnError(event)  {
    console.log("OnError: ", event);
}

function SessionID(msg) {
    console.log(msg);
    ws.sessionID = msg.id;

    if (typeof OnNewSession != 'undefined') {
        OnNewSession(msg);
    }
}
