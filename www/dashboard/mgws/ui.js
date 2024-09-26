import { ws } from "/mgws/websock.js";
import { appVersion } from "/mgws/index.js";

export var outputText = document.getElementById("output");

export var registerButton = document.getElementById("register");
export var getSessionsButton = document.getElementById("getSessions");

registerButton.addEventListener('click', RegisterSession);
getSessionsButton.addEventListener('click', GetActiveSessions);

export function RegisterSession() {
    var userName = localStorage.getItem("userName");
    var msg = {
        type: "RegisterSession",
        sessionId: ws.sessionID,
        appVersion: appVersion,
        userName: userName,
    };
  
    console.log("Sending " + "'" + msg.type + "'");
    ws.send(JSON.stringify(msg));
}

export function GetActiveSessions() {
    var userName = localStorage.getItem("userName");

    var msg = {
        type: "GetActiveSessions",
        sessionId: ws.sessionID,
        appVersion: appVersion,
        userName: userName,
    };

    console.log("Sending " + "'" + msg.type + "'");
    ws.send(JSON.stringify(msg));
}