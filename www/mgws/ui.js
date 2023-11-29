import { ws } from "./websock.js";

export var remote_video    = document.getElementById("remote_video");
export var local_video     = document.getElementById("local_video");
export var remote_id_input = document.getElementById('remote_id_input');
export var remote_id_label = document.getElementById('remote_id_label');
export var user_name_input = document.getElementById("user_name_input");
export var user_name_label = document.getElementById("user_name_label");
export var local_id_input  = document.getElementById("local_id_input");
export var outputTextarea  = document.getElementById('output');
export var callButton      = document.getElementById("call");
export var answerButton    = document.getElementById("answer");
export var hangupButton    = document.getElementById("hangup");
export var remotes_table   = document.getElementById("remotes_table");

local_id_input.readOnly = true;
local_id_input.disabled = true;
remote_id_input.disabled = true;

callButton.disabled = true;
answerButton.disabled = true;
hangupButton.disabled = true;

export function print(what) {
    if (outputTextarea) {
        outputTextarea.value += what + "\r\n";
    }
}

function OnTableRowOnClickEvent(event) {
    UpdateRemoteId(event.target.innerHTML);
}

export function OnSessionsChangedMessage(sessionsList) {
    // Rebuild table from scratch every time it changes...
    remotes_table.innerHTML = null;

    sessionsList.sessions.forEach(function(session) {
        // Don't show ourselves in remotes table, that is non-sensical
        if (session.sessionId === ws.sessionID)
            return;

        var tr = document.createElement('tr');

        tr.className = "highlightable";
        tr.innerHTML = "<td>" + session.userName + '(' + session.sessionId + ")</td>";

        tr.addEventListener("click", OnTableRowOnClickEvent);

        remotes_table.appendChild(tr);
    });
}

export function ButtonDisable(button, disable) {
    button.disabled = disable;
}

export function UpdateRemoteId(id) {
    remote_id_input.value = id;
    StopRemoteIdBlinking();

    ButtonDisable(callButton, false);
}

export function StopUserNameBlinking() {
    if (user_name_input.value !== '') {
        user_name_label.style.animation = 'none';
        user_name_label.offsetHeight;
        user_name_label.style.animationPlayState = 'paused';
    }
}

export function StopRemoteIdBlinking() {
    if (remote_id_input.value !== '') {
        remote_id_label.style.animation = 'none';
        remote_id_label.offsetHeight;
        remote_id_label.style.animationPlayState = 'paused';
    }
}

