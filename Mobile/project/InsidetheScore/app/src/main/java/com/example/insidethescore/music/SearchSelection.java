package com.example.insidethescore.music;

public class SearchSelection {
    MusicObjectSelection era;
    MusicObjectSelection composer;
    MusicObjectSelection musicWork;

    public SearchSelection() {
        era = new MusicObjectSelection();

        composer = new MusicObjectSelection();

        musicWork = new MusicObjectSelection();
    }

    public void setEra(String id, String value) {
        era.set(id, value);
    }

    public MusicObjectSelection getEra() { return era; }

    public boolean isComposerSelected() {
        return composer.isSet();
    }

    public void setComposer(String id, String value) {
        composer.set(id, value);
    }

    public MusicObjectSelection getComposer() {
        return composer;
    }

    public void setMusicWork(String id, String value) {
        musicWork.set(id, value);
    }

    public MusicObjectSelection getMusicWork() {
        return musicWork;
    }
}
