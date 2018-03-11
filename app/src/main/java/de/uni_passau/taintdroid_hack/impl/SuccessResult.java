package de.uni_passau.taintdroid_hack.impl;

import de.uni_passau.taintdroid_hack.TaskResult;

public class SuccessResult implements TaskResult {

    @Override
    public String getMessage() {
        return null;
    }

    @Override
    public boolean isError() {
        return false;
    }
}
