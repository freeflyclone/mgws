import { ws } from "/mgws/websock.js";
import { appVersion } from "/mgws/index.js";

export var outputText = document.getElementById("output");

export var registerButton = document.getElementById("register");
export var getSessionsButton = document.getElementById("getSessions");

registerButton.addEventListener('click', SendRegisterSession);
getSessionsButton.addEventListener('click', SendGetActiveSessions);

export function SendRegisterSession() {
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

export function SendGetActiveSessions() {
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

export function ShowActiveSessions(msg) {
    outputText.textContent = "";

    var numUsers = msg.sessions.length;
    var singular = numUsers == 1 ? true : false;
    outputText.innerText += "There " + (singular ? "is " : "are ") + numUsers + (singular ? " user" : " users") + "...\n\n";

    for (var i=0; i<numUsers; i++ ) {
        var session = msg.sessions[i];
        outputText.innerText += session.userName + "(" + session.sessionId + ")\n";
    }
}