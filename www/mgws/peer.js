import { print } from "./index.js";
import { ws } from "./websock.js";
import { localStream, user_name_input, local_id_input } from "./local.js";
import { OnSessionsChangedMessage } from "./ui.js";

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

// callee / caller depending on if WE are the caller or the answerer
export var peer_remote_id;
export var peer_remote_call_string;

export var remote_video = document.getElementById("remote_video");
export var remoteStream = null;

function SetPeerRemoteId(id) {
    peer_remote_id = id;
}

function SetPeerRemoteCallString(s) {
    peer_remote_call_string = s;
}

export function MakePeerConnection() {
    console.log("MakePeerConnection");
    pc = window.peerConnection = new RTCPeerConnection(configuration);
    if (pc === null) {
        print("RTCPeerConnection() failed");
        return;
    }

    pc.addEventListener("connectionstatechange",    (event) => { OnConnectionStateChange(event); });
    pc.addEventListener("datachannel",              (event) => { console.log(event); });
    pc.addEventListener("icecandidate",             (event) => { OnIceCandidateEvent(event.candidate); });
    pc.addEventListener("icecandidateerror",        (event) => { OnIceCandidateErrorEvent(event); });
    pc.addEventListener("removestream",             (event) => { console.log(event); });
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

export async function createOffer() {
    console.log("createOffer()");

    if (typeof localStream === null) {
        print("localStream is null.  Fix that.");
        return;
    }

    localStream.getTracks().forEach(track => {
        pc.addTrack(track, localStream);
    });
    
    const offerOptions = {
        // New spec states offerToReceiveAudio/Video are of type long (due to
        // having to tell how many "m" lines to generate).
        // http://w3c.github.io/webrtc-pc/#idl-def-RTCOfferAnswerOptions.
        offerToReceiveAudio: 1,
        offerToReceiveVideo: 1,
        iceRestart: 0,
        voiceActivityDetection: 0
    };

    SetPeerRemoteId(document.getElementById('remote_id_input').value);

    try {
        var offer = await pc.createOffer(offerOptions);
        await pc.setLocalDescription(offer);

        var msg = {
            type: "Call",
            sessionId: ws.sessionID,
            callerUserName: document.getElementById("user_name_input").value,
            targetId: document.getElementById("remote_id_input").value,
            callingId: document.getElementById("local_id_input").value,
            session: pc.localDescription
        };
    
        SetPeerRemoteCallString(msg.callerUserName + '@' + msg.targetId);

        var callingString = 'calling ' + peer_remote_call_string;
        print(callingString);

        ws.send(JSON.stringify(msg));
    } catch (e) {
        print(`Failed to create offer: ${e}`);
    }
}

export async function answer() {
    localStream.getTracks().forEach(track => {
        pc.addTrack(track, localStream);
    });
    
    const answer = await pc.createAnswer();
    await pc.setLocalDescription(answer);

    document.getElementById("remote_id_input").value = peer_remote_id;

    var msg = {
        type: "Answer",
        sessionId: ws.sessionID,
        answeringUserName: document.getElementById("user_name_input").value,
        targetId: document.getElementById("remote_id_input").value,
        answeringId: document.getElementById("local_id_input").value,
        session: pc.localDescription
    };

    console.log("answer(): ", answer);

    ws.send(JSON.stringify(msg));
}

function OnConnectionStateChange(cs) {
    var state = cs.target.connectionState;

    if (state === "connected") {
        print(state + ' to ' + peer_remote_call_string);
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
        originId: document.getElementById("local_id_input").value,
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
    SetPeerRemoteCallString(call.callerUserName + '@' + call.callingId);

    var incomingString = 'Incoming call from ' + peer_remote_call_string;

    console.log(incomingString);
    print(incomingString);

    // call.session is RTCSessionDescription AKA "offer" from the caller
    pc.setRemoteDescription(new RTCSessionDescription(call.session));
}

function OnAnswerMessage(answer) {
	// this was the final piece! 
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
