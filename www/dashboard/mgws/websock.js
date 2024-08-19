export var ws;

export function MakeWebSocket() {
    var wsUrl = "wss://ws.lumenicious.com:8443/websock";

    console.log("MakeWebSocket()\n");

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
            console.log(msg);
            ws.sessionID = msg.id;
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
