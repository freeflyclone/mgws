import { print } from "./index.js";
import { ws } from "./websock.js";
import { localStream, user_name_input, local_id_input } from "./local.js";

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

export var remote_video = document.getElementById("remote_video");
export var remoteStream = null;

function SetPeerRemoteId(id) {
    peer_remote_id = id;
}

export function MakePeerConnection() {
    console.log("MakePeerConnection");
    pc = window.peerConnection = new RTCPeerConnection(configuration);
    if (pc === null) {
        print("RTCPeerConnection() failed");
    }

    print("RTCPeerConnection succeeded!");

    pc.addEventListener("connectionstatechange",    (event) => { OnConnectionStateChange(event); });
    pc.addEventListener("datachannel",              (event) => { console.log(event); });
    pc.addEventListener("icecandidate",             (event) => { OnIceCandidateEvent(event.candidate); });
    pc.addEventListener("icecandidateerror",        (event) => { OnIceCandidateErrorEvent(event); });
    pc.addEventListener("iceconnectionstatechange", (event) => { OnIceConnectionStateChangeEvent(event); });
    pc.addEventListener("icegatheringstatechange",  (event) => { OnIceGatheringStateChangeEvent(event.target); });
    pc.addEventListener("negotiationneeded",        (event) => { OnNegotiationNeededEvent(event); });
    pc.addEventListener("removestream",             (event) => { console.log(event); });
    pc.addEventListener("signalingstatechange",     (event) => { OnSignalingStateChangeEvent(event); });
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
        console.log("adding track: ", track);
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

    try {
        print('Creating "offer"...');
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
    
        print('Submitting "offer" via Call to ' + msg.targetId + '...');
    
        SetPeerRemoteId(msg.targetId);
        ws.send(JSON.stringify(msg));
    } catch (e) {
        print(`Failed to create offer: ${e}`);
    }
}

export async function answer() {
    print('Answering: ' + document.getElementById("remote_id_input").value);

    localStream.getTracks().forEach(track => {
        pc.addTrack(track, localStream);
        console.log("adding track: ", track);
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

    print("OnConnectionStateChange(): " + state);
    console.log("OnConnectionStateChange", cs);
}

function OnIceGatheringStateChangeEvent(connection) {
    console.log("OnIceGatheringStateChange(): ", connection.iceGatheringState);
    print("IceGatheringState: " + connection.iceGatheringState);
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

function OnIceConnectionStateChangeEvent(event) {
    var string = "IceConnectionState: " + event.target.iceConnectionState; 
    print(string);
    console.log(string);
}
function OnNegotiationNeededEvent(event) {
    console.log("OnNegotiationNeededEvent()", event);
}

function OnSignalingStateChangeEvent(event) {
    var string = "SignalingState: " + event.target.signalingState;
    print(string);
    console.log(string);
}

function OnTrackEvent(event) {
    var track = event.receiver.track;

    console.log("OnTrackEvent(): ", event);
    if (remote_video.srcObject !== event.streams[0]) {
        remote_video.srcObject = event.streams[0];
        print("Received remote stream");
        console.log("Received remote stream");
    }
}

function OnTableRowOnClickEvent(event) {
    var target = event.target;
    var remoteId = null;
    var remote_id_input = document.getElementById("remote_id_input");

    if (target.className === "right") {
        remoteId = target.previousSibling.innerHTML;
    }
    else {
        remoteId = target.innerHTML;
    }

    remote_id_input.value = remoteId; 
}

export function OnSessionsChangedMessage(sessionsList) {
    console.log("OnSessionsChanged: ", sessionsList);
    
    const  td_left = '<td class="left">';
    const td_right = '<td class="right">';
    const   td_end = '</td>';

    // Overwrite "remotes-table" with new header (start from scratch)
    var remotes_table = document.getElementById("remotes-table");
    remotes_table.innerHTML = 
        '<tr><th class="left">Remote ID:</th><th class="right">User Name</th></tr>' + 
        '<tr class="short">' + td_left + '---------- ' + td_end + td_right + ' ----------' + td_end + '</tr>';

    // Add 2 column HTML table row as child element of "remotes_table" for each session...
    sessionsList.sessions.forEach(function(session) {
        // Don't show ourselves in remotes table, that is non-sensical
        if (session.localId === local_id_input.value)
            return;

        var tr = document.createElement('tr');

        tr.className = "highlightable";
        tr.innerHTML = td_left + session.localId + td_end + td_right + session.userName + td_end;
        tr.addEventListener("click", OnTableRowOnClickEvent);
        remotes_table.appendChild(tr);
    });
}

function OnCallMessage(call) {
    console.log("OnCallMessage(): ", call);
    print("OnCallMessage(): from: " + call.callingId + " to: " + call.targetId + " by: " + call.callerUserName);

    SetPeerRemoteId(call.callingId);

    // call.session is RTCSessionDescription AKA "offer" from the caller
    pc.setRemoteDescription(new RTCSessionDescription(call.session));
}

function OnAnswerMessage(answer) {
    console.log("OnAnswerMessage(): ", answer);
    print("OnAnswerMessage(): received answer from: " + answer.answeringId + " by: " + answer.answeringUserName);

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
