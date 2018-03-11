package de.uni_passau.taintdroid_hack.impl;

public class TaintdroidHack {

    static {
        System.loadLibrary("native-lib");
    }

    public static native boolean sendDataTo(String content, String host, int port);

    public static native String untaint(String filePath);

    public static native void writeDataInFilesystem(String content, String filesDir);

    public static native String readDataFromFilesystem(String content);

    /**
     * Taintdroid untaint method taken from
     * <a href="https://github.com/gsbabil/AntiTaintDroid/blob/ab87e31de721509e08f0e0e3056753be4e96df3b/src/com/gsbabil/antitaintdroid/UntaintTricks.java/#L377">
     *     AntiTaintdroid on Github</a>
     *
     * @param in takes the possibly tainted String
     * @return a completely untainted String
     */
    public static String encodingTrick(String in) {
        String symbols = " 0123456789abcdefghijklmnopqrstuvwxyz" +
                "ABCDEFGHIJKLMNOPQRSTUVWXYZäöüÄÖÜ~`!@#$%^&*()-=_+[]{}" +
                "\\|;',./:\"<>?\n";

        StringBuilder out = new StringBuilder();

        if (in != null) {
            for (int i = 0; i < in.length(); i++) {
                for (int j = 0; j < symbols.length(); j++) {
                    if (in.charAt(i) == symbols.charAt(j)) {
                        out.append(symbols.charAt(j));
                        break;
                    }
                }
            }
        }
        return out.toString();
    }

}
