package de.uni_passau.taintdroid_hack.impl;

import android.support.annotation.NonNull;

import de.uni_passau.taintdroid_hack.TaskResult;

public class ErrorResult implements TaskResult {

    private String message;

    public ErrorResult(@NonNull String message) {
        this.message = message;
    }

    @Override
    public String getMessage() {
        return message;
    }

    @Override
    public boolean isError() {
        return true;
    }
}
