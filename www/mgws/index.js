import { MakeWebSocket } from "./websock.js";
import { MakePeerConnection, createOffer, call } from "./peer.js";
import { InitLocalStream, GetStoredUserName } from "./local.js";

export const appVersion = "0.1";

var outputTextarea = document.getElementById('output');
var createOfferButton = document.getElementById("createOffer");
var callButton = document.getElementById("call");

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

function ShowSupportedConstraints() {
    var supportedBrowserConstraints = navigator.mediaDevices.getSupportedConstraints();
    //console.log(supportedBrowserConstraints);
}

export function print(what) {
    outputTextarea.value += what + "\r\n";
}

async function closing() {
    print("closing");
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

async function main() {
    createOfferButton.addEventListener('click', createOffer);
    callButton.addEventListener('click', call);

    print("location: " + window.location);

    ShowSupportedConstraints();

    InitLocalStream();
    GetStoredUserName();
    MakeWebSocket();
    MakePeerConnection();
}
