import { print } from "./ui.js"

export const appVersion = "0.1";

window.onload = main;
async function closing() {
    print("closing");
    return null;
}

async function main() {
    print("location: " + window.location);
}
