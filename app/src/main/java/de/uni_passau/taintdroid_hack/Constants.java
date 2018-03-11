package de.uni_passau.taintdroid_hack;

/**
 * This class collects some various constants used in the code of Taintdroid-Untaint.
 */
public class Constants {

    /**
     * Logging tag of our application.
     */
    public static final String TAG = "hack-taintdroid";

    /**
     * The ip of the host device in an emulator environment.
     */
    public static final String HOST_IP = "10.0.2.2";

    /**
     * This constant represents StandardCharsets.UTF8.name(), since this is not supported in API
     * level 18.
     */
    public static final String UTF8 = "UTF-8";

    /**
     * This constant represents System.lineSeparator(), since this was not yet included in API level
     * 18. How did people write code back then?
     */
    public static final String LINE_SEP = "\n";
}
