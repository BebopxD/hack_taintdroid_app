#ifndef TAINTDROIDHACK_TAINTDROID_HACK_H
#define TAINTDROIDHACK_TAINTDROID_HACK_H

#include <string.h>
#include <cstdio>
#include <sys/socket.h>
#include <netdb.h>
#include <jni.h>
#include <unistd.h> /* read, write, close */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <sstream>

#include "AppBindings.h"
#include "ScopedJniThreadState.h"

jboolean send_http_post(JNIEnv *env, jstring content_, jstring host_, int port);

jstring untaint(JNIEnv *env, jstring content_);

void write_to_file(JNIEnv *env, jstring content_, jstring dir_);

jstring read_from_file(JNIEnv *env, jstring path_);

#endif //TAINTDROIDHACK_TAINTDROID_HACK_H
