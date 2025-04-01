package com.example.insidethescore.music;

import java.util.ArrayList;

// TODO use this to handle more detailed information about music works
public class MusicWork {

    public MusicWork() {
        this.title = "";
        this.type = Music.WorkType.UNDEFINED;
        this.composer = "";
        this.year = -1;
        this.description = "";
//        this.key = new MusicKeySignature(Music.Note.C, Music.Accidental.SHARP, Music.Scale.MINOR);
    }

    private String title;

    private int year;

    private String composer;

    private String description;

    private Music.WorkType type;

    private MusicKeySignature key;
    private ArrayList<MusicWork> movements;
}
