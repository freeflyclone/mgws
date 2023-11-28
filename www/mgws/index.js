import { MakeWebSocket } from "./websock.js";
import { MakePeerConnection } from "./peer.js";
import { InitLocalStream, GetStoredUserName } from "./local.js";
import { AudioInit, audioMgr } from "./audio.js";
import { print } from "./ui.js";

export const appVersion = "0.1";

window.onload = main;
window.onunload = closing;

function ShowSupportedConstraints() {
    var supportedBrowserConstraints = navigator.mediaDevices.getSupportedConstraints();
    console.log(supportedBrowserConstraints);
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
    print("location: " + window.location);

    alert("Call signaling sounds will occur.");
    ShowSupportedConstraints();

    InitLocalStream();
    AudioInit();
    GetStoredUserName();
    MakeWebSocket();
    MakePeerConnection();
}
