import { ws } from "./websock.js";
import { CallState, callState } from "./peer.js";
import { audioMgr }from "./audio.js";

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

callButton.disabled = true;
answerButton.disabled = true;
hangupButton.disabled = true;

export var user_name;
export var remote_id;
export var remotes = [];
export var controlsVisible = true;

export function print(what) {
    if (outputTextarea) {
        outputTextarea.innerHTML += what + "<br>";
    }
}

export function puts(what) {
    if (outputTextarea) {
        outputTextarea.innerHTML += what;
    }
}

function ToggleClass(el, className) {
    if (el.className === ""){
        el.className += className;
    }
    else {
        el.classList.remove(className);
    }
}

function OnTableRowClickEvent(event) {
    var tr = event.target.parentNode;
    console.log("OnTableRowClickEvent(): ", tr);

    ToggleClass(tr, 'selected');

    UpdateRemoteId(tr.id);
    console.log("OnTableRowClickEvent(): ", tr);
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

        tr.innerHTML = '<td id="' + session.sessionId + '" class="">' + session.userName + "</td>";
        tr.id = session.sessionId;

        tr.addEventListener("click", OnTableRowClickEvent);

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

            if (remote_id) {
                UpdateCallStateUI(CallState.Identified);
            }

            break;

        case CallState.Identified:
                ControlsVisible(true);
                ButtonDisable(callButton, false);
                ButtonDisable(answerButton, true);
                ButtonDisable(hangupButton, true);
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

export function SetLocalUserName(name) {
    user_name = name;
}

export function GetUserNameFromId(id) {
    var obj = remotes.find(o => o.sessionId === id);

    if (typeof obj === 'undefined') {
        return null;
    }

    return obj.userName;
}

document.getElementById("controlsWrapper").addEventListener('click', (event) => {
    if (callState === CallState.Connected) {
        ToggleControlVisibility();
        console.log("controlsWrapper click event");
    }
});

