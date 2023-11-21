export const randomToken = Math.random().toString(36).slice(2);
export const id = "EvanOne";
export const key = "peerjs";
export var port = 4447;
export var websock;
export const pingInterval = 5000;


export function main() {
    if (window.location.host.split(".").slice(-1) == 'local')
        port += 1;

    var requestParams = new URLSearchParams({ token: randomToken, id: id, key: key,}).toString();

    //WebSockInit("wss:" + window.location.host + ":" + port);
}

function WebSockInit(url) {
    try {
        websock = new WebSocket(url);

        websock.onopen = (event) => {
            console.log("onopen: ", event);
            ScheduleHeartbeat();
        };
        websock.onmessage = (event) => {
            console.log("onmessage: ", event);
        };
        websock.onerror = (event) => {
            console.log("onerror: ", event);
        };
        websock.onclose = (event) => {
            console.log("onclose: ", event);
        };
    }
    catch (err) {
        console.log("failed: ", err);
    }
}

function ScheduleHeartbeat() {
    setTimeout(() => {
        SendHeartbeat();
    }, pingInterval);
}

function SendHeartbeat() {
    console.log("SendHeartbeat");

    websock.send(JSON.stringify({type: "HEARTBEAT"}));
    
    ScheduleHeartbeat();
}