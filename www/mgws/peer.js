import { appVersion } from "./index.js";
import { ws } from "./websock.js";
import { audioMgr }from "./audio.js";
import { InitLocalStream, StopLocalStream, localStream } from "./local.js";
import {
    user_name_input,
    local_id_input,
    remote_id_input,
    remote_id, 
    remote_video, 
    callButton,
    answerButton,
    hangupButton,
    print,
    OnSessionsChangedMessage,
    UpdateRemoteId,
    ButtonDisable,
    UpdateCallStateUI
} from "./ui.js";

var configuration = {
	iceServers: [ { urls: "turn:hq.e-man.tv:19302", username: "turnuser", credential: "turn456" } ],
	sdpSemantics: "unified-plan",
};

export var pc;
export var peer_remote_id;

export const CallState = {
    Idle: Symbol("Idle"),
    Calling: Symbol("Calling"),
    Ringing: Symbol("Ringing"),
    Connected: Symbol("Connected"),
}
export var callState = CallState.Idle;

function SetCallState(state) {
    console.log("callState transition: ", callState, " to ", state);
    callState = state;
    UpdateCallStateUI(callState);
}

export function PeerRegisterSession() {
    var msg = {
        type: "RegisterSession",
        sessionId: ws.sessionID,
        appVersion: appVersion,
        userName: localStorage.getItem("userName"),
    };
  
    ws.send(JSON.stringify(msg));
}

function ResetCallState() {
    UpdateCallStateUI(CallState.Idle);
}

function SetPeerRemoteId(id) {
    peer_remote_id = id;
}

function AbortCall() {
    StopLocalStream();
    
    if (pc) {
        pc.close();
        pc = null;
        MakePeerConnection();
        InitLocalStream();
        ResetCallState();
    }

    SetCallState(CallState.Idle);
}

export function MakePeerConnection() {
    callButton.addEventListener('click', Call);
    answerButton.addEventListener('click', Answer);
    hangupButton.addEventListener('click', Hangup);

    ResetCallState();

    pc = window.peerConnection = new RTCPeerConnection(configuration);
    if (pc === null) {
        print("RTCPeerConnection() failed");
        return;
    }

    pc.addEventListener("connectionstatechange",    (event) => { OnConnectionStateChange(event); });
    pc.addEventListener("icecandidate",             (event) => { OnIceCandidateEvent(event.candidate); });
    pc.addEventListener("icecandidateerror",        (event) => { OnIceCandidateErrorEvent(event); });
    pc.addEventListener("track",                    (event) => { OnTrackEvent(event); });
}

export function PeerMessageHandler(msg) {
    switch(msg.type) {
        case "SessionsChanged": OnSessionsChangedMessage(msg); break;
        case "Call":            OnCallMessage(msg); break;
        case "Answer":          OnAnswerMessage(msg); break;
        case "Hangup":          OnHangupMessage(msg); break;
        case "LocalIdChanged":  break;
        case "ICECandidate":    OnIceCandidateMessage(msg); break;
        default:                print("msg.type: " + msg.type);
    }
}

export async function Call() {
    SetPeerRemoteId(remote_id);
    SetCallState(CallState.Calling);

    var callingString = 'calling ' + peer_remote_id;
    print(callingString);

    if (typeof localStream === null) {
        print("localStream is null.  Fix that.");
        return;
    }

    try {
        localStream.getTracks().forEach(track => {
            pc.addTrack(track, localStream);
        });
    }
    catch (err) {
        console.log("caught error: " + err);
    }
    
    const offerOptions = { offerToReceiveAudio: 1, offerToReceiveVideo: 1 };

    try {
        var offer = await pc.createOffer(offerOptions);
        await pc.setLocalDescription(offer);

        var msg = {
                      type: "Call",
                  userName: user_name_input.value,
                  targetId: remote_id_input.value,
                 callingId: local_id_input.value,
                 sessionId: ws.sessionID,
                   session: pc.localDescription
        };
    
        ws.send(JSON.stringify(msg));
    } catch (e) {
        print(`Failed to create offer: ${e}`);
    }
}

export async function Answer() {
    localStream.getTracks().forEach(track => {
        pc.addTrack(track, localStream);
    });
    
    const answer = await pc.createAnswer();
    await pc.setLocalDescription(answer);

    UpdateRemoteId(peer_remote_id);

    var msg = {
                     type: "Answer",
                 userName: user_name_input.value,
                 targetId: remote_id_input.value,
              answeringId: local_id_input.value,
                sessionId: ws.sessionID,
                  session: pc.localDescription
    };
    ws.send(JSON.stringify(msg));

    ButtonDisable(answerButton, true);
}

async function Hangup() {
    AbortCall();

    var msg = {
        type: "Hangup",
        userName: user_name,
        targetId: remote_id,
    };

    ws.send(JSON.stringify(msg));
};
  
function OnConnectionStateChange(cs) {
    var state = cs.target.connectionState;

    if (state === "connected") {
        print(state + ' to ' + peer_remote_id);
        SetCallState(CallState.Connected);
    }
}

function OnIceCandidateEvent(candidate) {
    if (candidate === null) {
        return;
    }

    var msg = {
             type: "ICECandidate",
         userName: user_name,
         targetId: peer_remote_id,
        candidate: candidate,
    };

    ws.send(JSON.stringify(msg));
}

function OnIceCandidateErrorEvent(event) {
    var string = "ICECandidateError - url: " + event.url;
    string += ", " + event.errorCode;
    string += ", " + event.errorText;
    string += ", " + event.hostCandidate;
    console.log(string);
    print(string);
}

function OnTrackEvent(event) {
    if (remote_video.srcObject !== event.streams[0]) {
        remote_video.srcObject = event.streams[0];
    }
}

function OnIceCandidateMessage(candidate) {
    try {
        pc.addIceCandidate(candidate.candidate);
    }
    catch (e) {
        console.error('Error adding ice candidate', e);
    }
}

function OnCallMessage(call) {
    SetPeerRemoteId(call.callingId);
    SetCallState(CallState.Ringing);

    var incomingString = 'call from ' + peer_remote_id + ', userName: ' + call.userName;

    console.log(incomingString);
    print(incomingString);

    pc.setRemoteDescription(new RTCSessionDescription(call.session));
}

function OnAnswerMessage(answer) {
    pc.setRemoteDescription(new RTCSessionDescription(answer.session));
}

function OnHangupMessage(answer) {
    AbortCall();
}
