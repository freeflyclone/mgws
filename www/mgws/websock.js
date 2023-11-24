import { appVersion, print } from "./index.js";
import { UpdateLocalId } from "./local.js";
import { PeerMessageHandler } from "./peer.js";

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

    var regSession = {
        type: "RegisterSession",
        appVersion: appVersion,
    };
  
    ws.send(JSON.stringify(regSession));
};

function OnMessage(event) {
    var msg = JSON.parse(event.data);

    switch(msg.type) {
        case "SessionID":
            ws.sessionID = msg.id;
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
