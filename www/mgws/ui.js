import { ws } from "./websock.js";
import { CallState, callState } from "./peer.js";
import { audioMgr }from "./audio.js";

export var remote_video    = document.getElementById("remote_video");
export var local_video     = document.getElementById("local_video");
export var remote_id_input = document.getElementById('remote_id_input');
export var remote_id_label = document.getElementById('remote_id_label');
export var user_name_input = document.getElementById("user_name_input");
export var user_name       = document.getElementById("user_name");
export var user_name_label = document.getElementById("user_name_label");
export var local_id_input  = document.getElementById("local_id_input");
export var outputTextarea  = document.getElementById('output');
export var callButton      = document.getElementById("call");
export var answerButton    = document.getElementById("answer");
export var hangupButton    = document.getElementById("hangup");
export var remotes_table   = document.getElementById("remotes_table");

//remote_id_input.disabled = true;

callButton.disabled = true;
answerButton.disabled = true;
hangupButton.disabled = true;

export var remote_id;
export var remotes = [];
export var controlsVisible = true;

export function print(what) {
    if (outputTextarea) {
        outputTextarea.innerText += what + "\n";
    }
}

function OnTableRowOnClickEvent(event) {
    UpdateRemoteId(event.target.id);
}

export function OnSessionsChangedMessage(sessionsList) {
    // Rebuild table from scratch every time it changes...
    remotes_table.innerHTML = null;
    remotes = [];

    sessionsList.sessions.forEach(function(session) {
        // Don't show ourselves in remotes table, that is non-sensical
        if (session.sessionId === ws.sessionID) {
            return;
        }

        remotes.push({"userName" : session.userName, "sessionId" : session.sessionId});

        var tr = document.createElement('tr');

        tr.className = "highlightable";
        tr.innerHTML = "<td id='" + session.sessionId + "'>" + session.userName + "</td>";
        tr.id = session.sessionId;

        tr.addEventListener("click", OnTableRowOnClickEvent);

        remotes_table.appendChild(tr);
    });

    return;
}

export function ButtonDisable(button, disable) {
    button.disabled = disable;
}

export function UpdateRemoteId(id) {
    remote_id = id;

    ButtonDisable(callButton, false);
}

export function UpdateCallStateUI(state) {
    switch(state) {
        case CallState.Idle:
            ControlsVisible(true);
            ButtonDisable(callButton, true);
            ButtonDisable(answerButton, true);
            ButtonDisable(hangupButton, true);
            audioMgr.stop(0);
            audioMgr.stop(1);
            break;

        case CallState.Calling:
            ButtonDisable(callButton, true);
            ButtonDisable(answerButton, true);
            ButtonDisable(hangupButton, false);
            audioMgr.play(0, 1, function() { 
                if (callState === CallState.Connected || callState === CallState.Idle) {
                    audioMgr.stop(0); 
                    return true;
                }
                return false;
            });
            break;

        case CallState.Ringing:
            ButtonDisable(callButton, true);
            ButtonDisable(answerButton, false);
            ButtonDisable(hangupButton, false);
            audioMgr.play(1, 1, function() { 
                if (callState === CallState.Idle || callState === CallState.Connected) {
                    audioMgr.stop(1); 
                    return true; 
                }
                return false;
            });
            break;

        case CallState.Connected:
            ControlsVisible(false);
            ButtonDisable(callButton, true);
            ButtonDisable(answerButton, true);
            ButtonDisable(hangupButton, false);
            audioMgr.stop(0);
            audioMgr.stop(1);
            true
    }
}

export function ControlsVisible(visible) {
    controlsVisible = visible;
    if (visible) {
        document.getElementById("controlsWrapper").className = "fade_start";
    }
    else {
        document.getElementById("controlsWrapper").className = "fade_out";
    }
}

export function ToggleControlVisibility() {
    ControlsVisible(!controlsVisible);
}

document.getElementById("controlsWrapper").addEventListener('click', (event) => {
    if (callState === CallState.Connected) {
        ToggleControlVisibility();
        console.log("controlsWrapper click event");
    }
});

