export var remote_video    = document.getElementById("remote_video");
export var local_video = document.getElementById("local_video");

export var remote_id_input = document.getElementById('remote_id_input');
export var user_name_input = document.getElementById("user_name_input");
export var local_id_input  = document.getElementById("local_id_input");

export var outputTextarea = document.getElementById('output');
export var callButton = document.getElementById("call");
export var answerButton = document.getElementById("answer");

export function print(what) {
    outputTextarea.value += what + "\r\n";
}

function OnTableRowOnClickEvent(event) {
    var target = event.target;
    var remoteId = null;
    var remote_id_input = document.getElementById("remote_id_input");

    if (target.className === "right") {
        remoteId = target.previousSibling.innerHTML;
    }
    else {
        remoteId = target.innerHTML;
    }

    remote_id_input.value = remoteId; 
}

export function OnSessionsChangedMessage(sessionsList) {
    const  td_left = '<td class="left">';
    const td_right = '<td class="right">';
    const   td_end = '</td>';

    // Overwrite "remotes-table" with new header (start from scratch)
    var remotes_table = document.getElementById("remotes-table");
    remotes_table.innerHTML = 
        '<tr><th class="left">Remote ID:</th><th class="right">User Name</th></tr>' + 
        '<tr class="short">' + td_left + '---------- ' + td_end + td_right + ' ----------' + td_end + '</tr>';

    // Add 2 column HTML table row as child element of "remotes_table" for each session...
    sessionsList.sessions.forEach(function(session) {
        // Don't show ourselves in remotes table, that is non-sensical
        if (session.localId === local_id_input.value)
            return;

        var tr = document.createElement('tr');

        tr.className = "highlightable";
        tr.innerHTML = td_left + session.localId + td_end + td_right + session.userName + td_end;
        tr.addEventListener("click", OnTableRowOnClickEvent);
        remotes_table.appendChild(tr);
    });
}

