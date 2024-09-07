import { ws } from "/mgws/websock.js";
import { appVersion } from "/mgws/index.js";

export var registerButton = document.getElementById("register");

export function RegisterSession() {
    var userName = localStorage.getItem("userName");
    var msg = {
        type: "RegisterSession",
        sessionId: ws.sessionID,
        appVersion: appVersion,
        userName: userName,
    };
  
    console.log(msg);
    ws.send(JSON.stringify(msg));
}