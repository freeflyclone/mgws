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

function ShowSupportedConstraints() {
    var supportedBrowserConstraints = navigator.mediaDevices.getSupportedConstraints();
    //console.log(supportedBrowserConstraints);
}

async function main() {
    console.log("location: ", window.location);
    // Get the local webcam & mic and start them
    localStream = await navigator.mediaDevices.getUserMedia(userMediaConstraints);
    local_video.srcObject = localStream;
    local_video.play();

    ShowSupportedConstraints();

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

