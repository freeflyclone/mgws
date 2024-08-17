export var outputTextarea  = document.getElementById('output');
export var user_name_input = document.getElementById("user_name_input");

export function print(what) {
    if (outputTextarea) {
        outputTextarea.value += what + "\r\n";
    }
}

user_name_input.addEventListener('input', (event) => {
    // TODO: send every key via Websocket to server
    // have server send true/false if current name is unique.
});

user_name_input.addEventListener('keyup', ({key}) => {
    if (key === 'Enter') {
        // TODO: register new user in server database
        // if name longer than 0 characters and is unique.
        localStorage.setItem('userName', user_name_input.value);
        user_name_input.blur();
        window.location.assign("/mgws")
    }
});
