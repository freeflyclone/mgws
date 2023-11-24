import { ws } from "./websock.js";

export var localStream = null;

export var user_name_input = document.getElementById("user_name_input");
export var local_id_input = document.getElementById("local_id_input");
var local_video = document.getElementById("local_video");

user_name_input.addEventListener('change', UpdateLocalId);

export function GetStoredUserName() {
    console.log("GetStoredUserName()");

    var userName = localStorage.getItem("userName");
    user_name_input.value = userName;

    console.log("userName: ", userName);

    return userName;
}

export function UpdateLocalIdPlaceholder(value) {
    console.log("UpdateLocalIdPlaceholder()");
    local_id_input.placeholder = value;
}

export function UpdateLocalId() {
    localStorage.setItem("userName", user_name_input.value);

    local_id_input.value = user_name_input.value + "_" + ws.sessionID;

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

