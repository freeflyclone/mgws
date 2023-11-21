function IsMobile() {
    const isMobile = localStorage.mobile || window.navigator.maxTouchPoints > 1;

    return isMobile;
}

function InitIsMobile() {
    if (!IsMobile()) {
        return false;
    }
    
    var wasMobile = localStorage.getItem("isMobile");

    if (wasMobile != null) {
        return wasMobile;
    }

    if (confirm("Choose the mobile UI?") == true) {
        if (confirm("Woud you like to save your choice?") == false) {
            alert("Mobile UI only for this session.");
        } else {
            alert("You will not be prompted on subsequent sessions.");
            localStorage.setItem("isMobile", "true");
        }
        return true;
    }
    
    if (confirm("Woud you like to save your choice?") == false) {
        alert("Desktop UI only for this session.");
    } else {
        alert("You will not be prompted on subsequent sessions.");
        localStorage.setItem("isMobile", "false");
    }
    return false;
}

function ToggleTheme(value) { 
    var sheets = document.getElementsByTagName('link'); 
    sheets[0].href = value; 
} 

window.onload = main;
window.onunload = closing;

var local_video = document.getElementById("local_video");
var localStream;
var userMediaConstraints = {
    audio: true,
    video: true
};
var ws;

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
        { urls: "turn:hq.e-man.tv:19303", username: "turnuser", credential: "turn456"},
        //{ urls: "turn:hq.local:19303", username: "turnuser", credential: "turn456"},
	],
	sdpSemantics: "unified-plan",
};
export var pc;


function ShowSupportedConstraints() {
    var supportedBrowserConstraints = navigator.mediaDevices.getSupportedConstraints();
    //console.log(supportedBrowserConstraints);
}

export function print(what) {
    console.log(what);
}


async function closing() {
    if (localStream !== null) {
        var tracks = localStream.getVideoTracks();
        tracks.forEach(track => {
            console.log("closing: ", track);
            track.stop();
        });
    }
    ws.close();
    return null;
}

function MakeWebSocket() {
    var wsUrl = "wss://" + window.location.host + "/websock";

    ws = new WebSocket(wsUrl);

    ws.onopen = (event) => {
        console.log("ws.onopen: ", ws.url);

        ws.send("this is a test");
    };
    ws.onmessage = (event) => {
        var msg = event.data;
        console.log("ws.onmessage: ", msg);
    };
    ws.onclose = (event) => {
        console.log("ws.onclose: ", event);
    }
    ws.onerror = (event) => {
        console.log("ws.onerror: ", event);
    }
    ws.sessionID = 0;
}

function MakePeerConnection() {
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

document.getElementById("createOffer").addEventListener('click', createOffer);
document.getElementById("callRemote").addEventListener('click', callRemote);

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

    ws.Send(JSON.stringify(msg));
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

async function main() {
    console.log("location: ", window.location);
    // Get the local webcam & mic and start them
    localStream = await navigator.mediaDevices.getUserMedia(userMediaConstraints);
    local_video.srcObject = localStream;
    local_video.play();

    ShowSupportedConstraints();

    MakeWebSocket();
    MakePeerConnection();
}
