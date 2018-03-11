#ifndef __JNIEnvExt__
#define __JNIEnvExt__

#include "Thread.h"

struct JNIEnvExt {
    const struct JNINativeInterface *funcTable;     /* must be first */

    const struct JNINativeInterface *baseFuncTable;

    u4 envThreadId;
    Thread *self;

    /* if nonzero, we are in a "critical" JNI call */
    int critical;

    struct JNIEnvExt *prev;
    struct JNIEnvExt *next;
};

#endif /* __JNIEnvExt__ */