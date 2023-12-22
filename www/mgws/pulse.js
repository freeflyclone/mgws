import { ws } from "./websock.js";

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
        console.log("websocked closed, heartbeatTimer stopping");
        clearInterval(heartbeatTimer);
        return;
    }

    heartbeatsMissed++;
    if (heartbeatsMissed >= heartbeatMaxMissing) {
        console.log("Vanished server detected!");
        clearInterval(heartbeatTimer);
    }

    var msg = {
             type: "Heartbeat",
        sessionId: ws.sessionID,
    };

    ws.send(JSON.stringify(msg));
}

