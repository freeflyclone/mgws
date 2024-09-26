import { ShowActiveSessions } from "/mgws/ui.js";

export var ws;

var OnNewSession;

export function MakeWebSocket(ons) {
    // The MGWS WebSocket handler is invoked with "websock" added to the host URL 
    var protocol = window.location.protocol.replace("http", "ws");
    var wsUrl = protocol + "//" + window.location.hostname + ":8443/websock";

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
            OnSessionID(msg);
            break;

        case "ActiveSessions":
            OnActiveSessions(msg);
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

function OnSessionID(msg) {
    console.log("Recieved " + "'" + msg.type + "', ", msg);
    ws.sessionID = msg.id;

    if (typeof OnNewSession != 'undefined') {
        OnNewSession(msg);
    }
}

function OnActiveSessions(msg) {
    console.log("Recieved " + "'" + msg.type + "', ", msg);
    ShowActiveSessions(msg);
}
