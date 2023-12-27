import { ws } from "./websock.js";
import { ClearRemotesTable, print } from "./ui.js";

const heartbeatIntervalMs = 500;
const heartbeatMaxMissing = 5;
var heartbeatTimer;
var heartbeatsMissed;

export function BeginHeartbeat() {
    heartbeatsMissed = 0;
    heartbeatTimer = setInterval(heartbeat, heartbeatIntervalMs);
}

export function PulseDetected() {
    heartbeatsMissed = 0;
}

function heartbeat() {
    if (ws.readyState === WebSocket.CLOSED)
    {
        var msgStr = "Signaling server connection lost, ID: " + ws.sessionID;
        console.log(msgStr);
        print(msgStr);
        clearInterval(heartbeatTimer);
        ClearRemotesTable();
        return;
    }

    heartbeatsMissed++;
    if (heartbeatsMissed >= heartbeatMaxMissing) {
        console.log("Signaling server vanished!");
        print("Signaling server vanished!");
        clearInterval(heartbeatTimer);
        ClearRemotesTable();
    }

    var msg = {
             type: "Heartbeat",
        sessionId: ws.sessionID,
    };

    ws.send(JSON.stringify(msg));
}

