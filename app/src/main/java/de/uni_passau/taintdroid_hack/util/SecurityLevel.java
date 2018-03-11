package de.uni_passau.taintdroid_hack.util;

/**
 * This enum represents the different security levels and implementations of Taintdroid-Hack.
 *
 * @see de.uni_passau.taintdroid_hack.impl.TaintdroidHack
 */
public enum SecurityLevel {
    REGULAR,
    OBFUSCATED_JAVA,
    OBFUSCATED_NATIVE_NETWORK,
    OBFUSCATED_NATIVE_UNTAINT,
    OBFUSCATED_NATIVE_FILESYSTEM
}
