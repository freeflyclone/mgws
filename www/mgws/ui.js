export var remote_video    = document.getElementById("remote_video");
export var local_video     = document.getElementById("local_video");
export var remote_id_input = document.getElementById('remote_id_input');
export var user_name_input = document.getElementById("user_name_input");
export var local_id_input  = document.getElementById("local_id_input");
export var outputTextarea  = document.getElementById('output');
export var callButton      = document.getElementById("call");
export var answerButton    = document.getElementById("answer");
export var remotes_table   = document.getElementById("remotes-table");

export function print(what) {
    outputTextarea.value += what + "\r\n";
}

function OnTableRowOnClickEvent(event) {
    remote_id_input.value = event.target.innerHTML; 
}

export function OnSessionsChangedMessage(sessionsList) {
    // Rebuild table from scratch every time it changes...
    remotes_table.innerHTML = '<tr><th>Remote ID:</th></tr><tr><td>----------</td></tr>'

    sessionsList.sessions.forEach(function(session) {
        // Don't show ourselves in remotes table, that is non-sensical
        if (session.localId === local_id_input.value)
            return;

        var tr = document.createElement('tr');

        tr.className = "highlightable";
        tr.innerHTML = "<td>" + session.localId + "</td>";

        tr.addEventListener("click", OnTableRowOnClickEvent);

        remotes_table.appendChild(tr);
    });
}

