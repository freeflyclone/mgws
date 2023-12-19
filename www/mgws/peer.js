import { appVersion } from "./index.js";
import { ws } from "./websock.js";
import { InitLocalStream, StopLocalStream, localStream } from "./local.js";
import {
    user_name,
    remote_id, 
    remote_video, 
    callButton,
    answerButton,
    hangupButton,
    print,
    OnSessionsChangedMessage,
    UpdateRemoteId,
    ButtonDisable,
    UpdateCallStateUI,
    GetUserNameFromId
} from "./ui.js";

var configuration = {
	iceServers: [ { urls: "turn:hq.e-man.tv:19302", username: "turnuser", credential: "turn456" } ],
	sdpSemantics: "unified-plan",
};

export var pc;
export var peer_remote_id;
export var peer_remote_user_name;

export const CallState = {
    Idle: 0,
    Identified: 1,
    Calling: 2,
    Ringing: 3,
    Connected: 4,
}
export var callState = CallState.Idle;

export function SetCallState(state) {
    callState = state;
    UpdateCallStateUI(callState);
}

export function PeerRegisterSession() {
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

function ResetCallState() {
    UpdateCallStateUI(CallState.Idle);
}

function SetPeerRemoteId(id) {
    peer_remote_id = id;
}

export function EndCall() {
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

    UpdateRemoteId(remote_id);
    peer_remote_user_name = GetUserNameFromId(peer_remote_id)
    var callingString = 'calling ' + peer_remote_user_name + "...";
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
        print("caught error: " + err);
    }
    
    const offerOptions = { offerToReceiveAudio: 1, offerToReceiveVideo: 1 };

    try {
        var offer = await pc.createOffer(offerOptions);
        await pc.setLocalDescription(offer);

        var msg = {
                      type: "Call",
                  userName: user_name,
                  targetId: peer_remote_id.toString(),
                 sessionId: ws.sessionID,
                   session: pc.localDescription
        };
    
        console.log(msg);
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

    var answeringString = 'answering ' + GetUserNameFromId(peer_remote_id) + "...";
    print(answeringString);

    var msg = {
             type: "Answer",
         userName: user_name,
         targetId: peer_remote_id.toString(),
        sessionId: ws.sessionID,
          session: pc.localDescription
    };
    console.log(msg);
    ws.send(JSON.stringify(msg));

    ButtonDisable(answerButton, true);
}

export function Hangup() {
    switch(callState) {
        case CallState.Ringing:
            var ringingString = "declining call from " + GetUserNameFromId(peer_remote_id);
            print(ringingString);
            break;

        case CallState.Calling:
            var callingString = "cancelling call to " + GetUserNameFromId(peer_remote_id);
            print(callingString);
            break;

        case CallState.Connected:
            var connectedString = "ending call to " + GetUserNameFromId(peer_remote_id);
            print(connectedString);
            break;
    }

    EndCall();

    var msg = {
        type: "Hangup",
        userName: user_name,
        targetId: peer_remote_id.toString(),
    };

    console.log(msg);
    ws.send(JSON.stringify(msg));
};
  
function OnConnectionStateChange(cs) {
    var state = cs.target.connectionState;

    if (state === "connected") {
        print(state + ' to ' + GetUserNameFromId(peer_remote_id));
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
         targetId: peer_remote_id.toString(),
        candidate: candidate,
    };

    //console.log(msg);
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
    SetPeerRemoteId(call.sessionId);
    SetCallState(CallState.Ringing);

    peer_remote_user_name = call.userName;

    var incomingString = 'call from ' + peer_remote_user_name;
    print(incomingString);

    pc.setRemoteDescription(new RTCSessionDescription(call.session));
}

function OnAnswerMessage(answer) {
    pc.setRemoteDescription(new RTCSessionDescription(answer.session));
}

function OnHangupMessage(answer) {
    switch(callState) {
        case CallState.Ringing:
            var ringingString = "call cancelled by " + GetUserNameFromId(peer_remote_id);
            print(ringingString);
            break;

        case CallState.Calling:
            var callingString = "call declined by " + GetUserNameFromId(peer_remote_id);
            print(callingString);
            break;

        case CallState.Connected:
            var connectedString = "call ended by " + GetUserNameFromId(peer_remote_id);
            print(connectedString);
            break;
    }

    EndCall();
}
