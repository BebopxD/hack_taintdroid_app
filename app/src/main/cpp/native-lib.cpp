#include "native-lib.h"

extern "C"
JNIEXPORT jboolean JNICALL
Java_de_uni_1passau_taintdroid_1hack_impl_TaintdroidHack_sendDataTo(JNIEnv *env, jobject instance,
                                                               jstring content_, jstring host_,
                                                               jint port) {
    return send_http_post(env, content_, host_, port);
}


extern "C"
JNIEXPORT jstring JNICALL
Java_de_uni_1passau_taintdroid_1hack_impl_TaintdroidHack_untaint(JNIEnv *env, jobject instance,
                                                            jstring content_) {
    return untaint(env, content_);
}


extern "C"
JNIEXPORT jstring JNICALL
Java_de_uni_1passau_taintdroid_1hack_impl_TaintdroidHack_readDataFromFilesystem(JNIEnv *env, jobject instance,
                                                               jstring path_) {
    return read_from_file(env, path_);
}


extern "C"
JNIEXPORT void JNICALL
Java_de_uni_1passau_taintdroid_1hack_impl_TaintdroidHack_writeDataInFilesystem(JNIEnv *env,
                                                                          jobject instance,
                                                                          jstring content_,
                                                                          jstring filesDir_) {
    write_to_file(env, content_, filesDir_);
}