package com.example.insidethescore.music;

// TODO use this to handle more detailed information about music works
public class Music {
    public enum WorkType {
        UNDEFINED,
        OTHER,
        SONATA,
        CONCERTO,
        TRIO,
        QUARTET,
        QUINTET
    }

    public enum Scale {
        MAJOR,
        MINOR
    }

    public enum Note {
        C,
        D,
        E,
        F,
        G,
        A,
        B
    }

    public enum Accidental {
        NATURAL,
        SHARP,
        FLAT
    }
}
