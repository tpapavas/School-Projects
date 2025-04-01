package com.example.insidethescore.music;

// TODO use this to handle more detailed information about music works
public class MusicKeySignature {
    private Music.Note note;
    private Music.Accidental accidental;
    private Music.Scale scale;

    public MusicKeySignature(Music.Note note, Music.Accidental accidental, Music.Scale scale) {
        this.note = note;
        this.accidental = accidental;
        this.scale = scale;
    }
}
