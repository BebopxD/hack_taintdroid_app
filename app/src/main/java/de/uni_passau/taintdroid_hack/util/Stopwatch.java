package de.uni_passau.taintdroid_hack.util;

/**
 * A utility class used to take execution times.
 */
public class Stopwatch {
    private long startTime;

    public Stopwatch() {
        startTime = System.currentTimeMillis();
    }

    public long splitTime() {
        return System.currentTimeMillis() - startTime;
    }
}
