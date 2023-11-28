import { UpdateLocalId } from "./local.js";
import { PeerMessageHandler, PeerRegisterSession } from "./peer.js";

export var ws;

export function MakeWebSocket() {
    var wsUrl = "wss://" + window.location.host + "/websock";

    ws = new WebSocket(wsUrl);

    ws.onopen = OnOpen; 
    ws.onmessage = OnMessage;
    ws.onclose = OnClose;
    ws.onerror = OnError;

    ws.sessionID = 0;
}

function OnOpen(event) {
    console.log("OnOpen: ", ws.url);
};

function OnMessage(event) {
    var msg = JSON.parse(event.data);

    switch(msg.type) {
        case "SessionID":
            ws.sessionID = msg.id;
            PeerRegisterSession();
            UpdateLocalId();
            break;

        default:
            PeerMessageHandler(msg);
            break;
    }
}

function OnClose(event) {
    console.log("OnClose: ", event);
}

function OnError(event)  {
    console.log("OnError: ", event);
}
