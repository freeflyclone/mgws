import { ws } from "./websock.js";
import { user_name } from "./ui.js";

export var localStream = null;

export function GetStoredUserName() {
    var userName = localStorage.getItem("userName");
    user_name.textContent = userName;

    return userName;
}

export function UpdateLocalIdPlaceholder(value) {
    console.log("UpdateLocalIdPlaceholder()");
    local_id_input.placeholder = value;
}

export function UpdateLocalId() {
    localStorage.setItem("userName", user_name.textContent);

    if (ws.sessionID != null) {
        SendLocalIdEvent();
    }
}

export function SendLocalIdEvent() {
    var localIdEvent = {
        type: "LocalIdEvent",
        sessionID: ws.sessionID,
        userName: user_name.textContent
    };

    ws.send(JSON.stringify(localIdEvent));
}

export function StopLocalStream() {
    localStream.getTracks().forEach(track => {
        track.stop();
    });
}

export function InitLocalStream() {
    const constraints = {
        'video': true,
        'audio': true
    }

    navigator.mediaDevices.getUserMedia(constraints)
        .then(stream => {
            localStream = stream;
            local_video.srcObject = localStream;
            local_video.play();
        })
        .catch(error => {
            console.error('Error accessing media devices.', error);
        });
}

