import { print } from "./index.js";
import { ws } from "./websock.js";
import { localStream } from "./local.js";

var iceCandidates = [];
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
        //{ urls: "turn:hq.local:19303", username: "turnuser", credential: "turn456"},
	],
	sdpSemantics: "unified-plan",
};
export var pc;

export function MakePeerConnection() {
    console.log("MakePeerConnection");

    pc = window.peerConnection = new RTCPeerConnection(configuration);
    if (pc === null) {
        print("RTCPeerConnection() failed");
    }

    print("RTCPeerConnection succeeded!");
    console.log(pc);
    pc.addEventListener("addstream", (event) => {
        print("addstream");
        console.log(event);
    });
    pc.addEventListener("connectionstatechange", (event) => {
        print('connectionstatechange: ' + pc.connectionState);
        console.log(event);
    });
    pc.addEventListener("datachannel", (event) => {
        console.log(event);
    });
    pc.addEventListener("icecandidate", (event) => {
        console.log(event.type);
        HandleIceCandidate(event.candidate);
    });
    pc.addEventListener("icecandidateerror", (event) => {
        HandleIceCandidateErrorEvent(event);
    });
    pc.addEventListener("iceconnectionstatechange", (event) => {
        print("iceconnectionstatechange");
        console.log(event);
    });
    pc.addEventListener("icegatheringstatechange", (event) => {
        HandleIceGatheringStateChange(event.target);
    });
    pc.addEventListener("negotiationneeded", (event) => {
        HandleNegotiationNeededEvent(event);
    });
    pc.addEventListener("removestream", (event) => {
        console.log(event);
    });
    pc.addEventListener("signalingstatechange", (event) => {
        HandleSignalingStateChangeEvent(event);
    });
    pc.addEventListener("track", (event) => {
        console.log(event);
    });
}

export async function createOffer() {
    console.log("createOffer()");

    if (typeof pc === 'undefined' || pc === null) {
        print('Need RTCPeerConnection, please click "New" to continue');
        return;
    }

    if (typeof localStream === null) {
        print("localStream is null.  Fix that.");
        return;
    }

    // Do this AFTER connection is established (maybe?)
    var tracks = localStream.getTracks();
    console.log("tracks: ", tracks);

    //pc.addTrack(tracks[0], localStream);
    pc.addTrack(tracks[1], localStream);

    const offerOptions = {
        // New spec states offerToReceiveAudio/Video are of type long (due to
        // having to tell how many "m" lines to generate).
        // http://w3c.github.io/webrtc-pc/#idl-def-RTCOfferAnswerOptions.
        offerToReceiveAudio: 0,
        offerToReceiveVideo: 0,
        iceRestart: 0,
        voiceActivityDetection: 0
    };

    try {
        const offer = await pc.createOffer(offerOptions);
        await pc.setLocalDescription(offer);
        print('Submitting "offer"...');
        ws.send(JSON.stringify(offer));
    } catch (e) {
        print(`Failed to create offer: ${e}`);
    }
}

export function callRemote() {
    var msg = {
        type: "CallRemote",
        sessionId: ws.sessionID,
        remoteId: document.getElementById("remote_id_input").value,
        userName: document.getElementById("user_name_input").value,
        session: pc.localDescription
    };

    print('Sending "offer" via CallRemote to ' + msg.remoteId + '...');

    ws.send(JSON.stringify(msg));
}

function HandleIceGatheringStateChange(connection) {
    console.log("HandleIceGatheringStateChange(): ", connection.iceGatheringState);
    print("IceGatheringState: " + connection.iceGatheringState);
}

function HandleIceCandidate(candidate) {
    if (candidate != null) {
        iceCandidates.push(candidate);

        var string = "type: " + candidate.type;
        string += ", proto: " + candidate.protocol;
        string += ", address: " + candidate.address;
        string += ":" + candidate.port;
        print(string);
        return;
    }
}

function HandleIceCandidateErrorEvent(event) {
    var string = "ICECandidateError - url: " + event.url;
    string += ", " + event.errorCode;
    string += ", " + event.errorText;
    string += ", " + event.hostCandidate;
    console.log("HandleIceCandidateErrorEvent(): " + string);
}

function HandleNegotiationNeededEvent(event) {
    console.log("HandleNegotiationNeededEvent()", event);
}

function HandleSignalingStateChangeEvent(event) {
    var string = "SignalingStateChange: " + event.target.signalingState;
    console.log("HandleSignalingStateChangeEvent(): " + string);
}

function OnTableRowOnClick (event) {
    var target = event.target;
    var remoteId = null;
    var remote_id_input = document.getElementById("remote_id_input");
    var local_id_input = document.getElementById("local_id_input");

    if (target.className === "right") {
        remoteId = target.previousSibling.innerHTML;
    }
    else {
        remoteId = target.innerHTML;
    }

    // disallow our localId for now (removing from list should be easy)
    if (remoteId === local_id_input.value) {
        return;
    }

    remote_id_input.value = remoteId; 
}

export function HandleSessionsChanged(sessionsList) {
    console.log(sessionsList);
    
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
        tr.addEventListener("click", OnTableRowOnClick);
        remotes_table.appendChild(tr);
    });
}

export function PeerMessageHandler(msg) {
    switch(msg.type) {
        case "SessionsChanged":
            HandleSessionsChanged(msg);
            break;

        default:
            print("msg.type: " + msg.type);
    }
}
