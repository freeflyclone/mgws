export var outputTextarea  = document.getElementById('output');

export function print(what) {
    if (outputTextarea) {
        outputTextarea.value += what + "\r\n";
    }
}

