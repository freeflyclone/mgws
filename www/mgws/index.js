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

window.onload=main;

var local_video = document.getElementById("local_video");
var localStream;
var userMediaConstraints = {
    audio: true,
    video: true
};

async function main() {
    // Get the local webcam & mic and start them
    localStream = await navigator.mediaDevices.getUserMedia(userMediaConstraints);
    local_video.srcObject = localStream;
    local_video.play();

    ShowSupportedConstraints();
}

function ShowSupportedConstraints() {
    var supportedBrowserConstraints = navigator.mediaDevices.getSupportedConstraints();
    console.log(supportedBrowserConstraints);
}