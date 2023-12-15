import { MakeWebSocket } from "./websock.js";
import { MakePeerConnection } from "./peer.js";
import { InitLocalStream, GetStoredUserName } from "./local.js";
import { AudioInit } from "./audio.js";
import { print, puts, user_name } from "./ui.js";

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
   
    print("<b>Host:</b> " + window.location.host);

    ShowSupportedConstraints();

    GetStoredUserName();
    print("<b>User Name:</b> " + user_name);
    puts("<hr>");
    puts(".");

    InitLocalStream();
    puts(".");

    AudioInit();
    puts(".");

    MakeWebSocket();
    puts(".");

    MakePeerConnection();
    puts(".");

    print("<br>Initialization succeeded!");
    print("\nClick/Tap a recipient above to start a call.\n");
}
