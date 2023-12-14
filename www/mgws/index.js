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
    // TODO: this doesn't work on iOS Safari, fix that.
    if (localStorage.getItem('userName') === null) {
        window.location.assign("/enroll");
    }
   
    print(window.location);

    ShowSupportedConstraints();

    print("Getting local media devices...");
    InitLocalStream();

    print("Initializing sounds...");
    AudioInit();

    print("Looking for stored user name...");
    GetStoredUserName();

    print("Connecting to signaling server...");
    MakeWebSocket();

    print("Establishing a peer connection.");
    MakePeerConnection();

    print("\nSuccess!");
    print("\nClick/Tap a recipient in the panel above to initiate a call.");
}
