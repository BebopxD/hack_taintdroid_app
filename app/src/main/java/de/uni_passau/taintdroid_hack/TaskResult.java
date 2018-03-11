package de.uni_passau.taintdroid_hack;

import de.uni_passau.taintdroid_hack.impl.ErrorResult;
import de.uni_passau.taintdroid_hack.impl.SuccessResult;

/**
 * Interface for giving feedback to the UI.
 * Basic implementations are: {@link SuccessResult} and {@link ErrorResult}.
 */
public interface TaskResult {

    String getMessage();

    boolean isError();
}
