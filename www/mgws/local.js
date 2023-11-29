import { ws } from "./websock.js";
import { 
    user_name_input, 
    local_id_input, 
    StopUserNameBlinking } from "./ui.js";

export var localStream = null;

export function GetStoredUserName() {
    var userName = localStorage.getItem("userName");
    user_name_input.value = userName;

    StopUserNameBlinking();

    return userName;
}

export function UpdateLocalIdPlaceholder(value) {
    console.log("UpdateLocalIdPlaceholder()");
    local_id_input.placeholder = value;
}

export function UpdateLocalId() {
    localStorage.setItem("userName", user_name_input.value);

    local_id_input.value = user_name_input.value + '(' + ws.sessionID + ')';
    StopUserNameBlinking();

    if (ws.sessionID != null) {
        SendLocalIdEvent();
    }
}

export function SendLocalIdEvent() {
    var localIdEvent = {
        type: "LocalIdEvent",
        sessionID: ws.sessionID,
        localId: local_id_input.value,
        userName: user_name_input.value
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

    user_name_input.addEventListener('change', UpdateLocalId);

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

