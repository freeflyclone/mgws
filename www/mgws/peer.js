import { ws } from "./websock.js";
import { localStream } from "./local.js";
import {
    user_name_input,
    local_id_input,
    remote_id_input, 
    remote_video, 
    callButton,
    answerButton,
    print,
    OnSessionsChangedMessage 
} from "./ui.js";

callButton.addEventListener('click', Call);
answerButton.addEventListener('click', Answer);

var configuration = {
	iceServers: [
        /*
		{ urls: "stun:stun.l.google.com:19302" },
		{
			urls: [
				"turn:us-0.turn.peerjs.com:3478",
			],
			username: "peerjs",
			credential: "peerjsp",
		},
        */
        { urls: "turn:hq.e-man.tv:19302", username: "turnuser", credential: "turn456"},
	],
	sdpSemantics: "unified-plan",
};

export var pc;
export var peer_remote_id;

function SetPeerRemoteId(id) {
    peer_remote_id = id;
}

export function MakePeerConnection() {
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
        case "LocalIdChanged":  break;
        case "ICECandidate":    OnIceCandidateMessage(msg); break;
        default:                print("msg.type: " + msg.type);
    }
}

export async function Call() {
    SetPeerRemoteId(remote_id_input.value);

    var callingString = 'calling ' + peer_remote_id;
    print(callingString);


    if (typeof localStream === null) {
        print("localStream is null.  Fix that.");
        return;
    }

    localStream.getTracks().forEach(track => {
        pc.addTrack(track, localStream);
    });
    
    const offerOptions = { offerToReceiveAudio: 1, offerToReceiveVideo: 1 };

    try {
        var offer = await pc.createOffer(offerOptions);
        await pc.setLocalDescription(offer);

        var msg = {
                      type: "Call",
                 sessionId: ws.sessionID,
            callerUserName: user_name_input.value,
                  targetId: remote_id_input.value,
                 callingId: local_id_input.value,
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

    remote_id_input.value = peer_remote_id;

    var msg = {
                     type: "Answer",
                sessionId: ws.sessionID,
        answeringUserName: user_name_input.value,
                 targetId: remote_id_input.value,
              answeringId: local_id_input.value,
                  session: pc.localDescription
    };
    ws.send(JSON.stringify(msg));
}

function OnConnectionStateChange(cs) {
    var state = cs.target.connectionState;

    if (state === "connected") {
        print(state + ' to ' + peer_remote_id);
    }
}

function OnIceCandidateEvent(candidate) {
    if (candidate === null) {
        return;
    }

    var msg = {
             type: "ICECandidate",
        sessionId: ws.sessionID,
         targetId: peer_remote_id,
         originId: local_id_input.value,
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

function OnCallMessage(call) {
    SetPeerRemoteId(call.callingId);

    var incomingString = 'call from ' + peer_remote_id;

    console.log(incomingString);
    print(incomingString);

    pc.setRemoteDescription(new RTCSessionDescription(call.session));
}

function OnAnswerMessage(answer) {
    pc.setRemoteDescription(new RTCSessionDescription(answer.session));
}

function OnIceCandidateMessage(candidate) {
    try {
        pc.addIceCandidate(candidate.candidate);
    }
    catch (e) {
        console.error('Error adding ice candidate', e);
    }
}
