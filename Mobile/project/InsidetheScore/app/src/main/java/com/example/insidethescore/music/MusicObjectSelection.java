package com.example.insidethescore.music;

public class MusicObjectSelection {
    private String id;  // db id of music object
    private String value;  // value (title, surname, depends on the music object type) of music object

    private boolean objectSet;

    public MusicObjectSelection() {
        id = "";
        value = "";

        objectSet = false;
    }
    public boolean isSet() {
        return objectSet;
    }

    public void set(String id, String value) {
        this.id = id;
        this.value = value;

        objectSet = true;
    }

    public String getId() {
        return id;
    }

    public String getValue() {
        return value;
    }
}
