let audioMgr = null;

class AudioManager {
    constructor() {
        this.audioContext = new AudioContext(window.AudioContext || window.webkitAudioContext);
        this.sounds = {};
        this.soundBuffers = [];
        this.soundCount = 0;

        this.currentMasterGain = 0.75;

        this.nodes = {
            destination: this.audioContext.destination,
            masterGain: this.audioContext.createGain(),
            backgroundMusicGain: this.audioContext.createGain(),
            coreEffectsGain: this.audioContext.createGain(),
        }

        this.nodes.masterGain.connect(this.nodes.destination);
        this.nodes.backgroundMusicGain.connect(this.nodes.masterGain);
        this.nodes.coreEffectsGain.connect(this.nodes.masterGain);

        this.nodes.masterGain.gain.value = this.currentMasterGain;
    }

    setArbitraryVolume() {
        var musicGainNode = this.nodes.backgroundMusicGain;

        // set music volume to 50%
        musicGainNode.gain.value = 0.5;
    }

    mute() {
        this.nodes.masterGain.gain.value = 0;
    }

    unmute() {
        this.nodes.masterGain.gain.value = this.currentMasterGain;
    }

    load(src) {
        let self = this;
        var idx = self.soundCount;
        self.soundCount++;

        const request = new XMLHttpRequest();

        request.open("GET", src);
        request.responseType = "arraybuffer";
    
        request.onload = function () {
            console.log("request.onload()");
            self.audioContext.decodeAudioData(
                request.response, 
                function(buffer) {
                    self.soundBuffers[idx] = buffer;
                }
            );
        };
    
        request.send();
    }

    play(idx) {
        let self = this;
        const src = self.audioContext.createBufferSource();

        src.buffer = self.soundBuffers[idx];
        src.connect(self.nodes.masterGain);
        src.onended = (event) => {
            event.target.disconnect();
        };
        src.start();
    }
}

function AudioInit() {
    audioMgr = new AudioManager();

    addEventListener("focus", (event) => {
        console.log("audio focus");
        audioMgr.unmute();
    });
    
    addEventListener("blur", (event) => {
        console.log("audio blur");
        audioMgr.mute();
    });

    // load order is preserved
    audioMgr.load("/audio/ShotSound.mp3");
    audioMgr.load("/audio/HitFiasco2.mp3");    
}
