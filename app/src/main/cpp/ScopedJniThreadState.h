#ifndef __ScopedJniThreadState__
#define __ScopedJniThreadState__

#include "JNIEnvExt.h"

#ifdef WITH_TAINT_TRACKING
# define CHECK_STACK_SUM(_self)     ((void)0)
# define COMPUTE_STACK_SUM(_self)   ((void)0)
#endif

/*
 * Entry/exit processing for all JNI calls.
 *
 * We skip the (curiously expensive) thread-local storage lookup on our Thread*.
 * If the caller has passed the wrong JNIEnv in, we're going to be accessing unsynchronized
 * structures from more than one thread, and things are going to fail
 * in bizarre ways.  This is only sensible if the native code has been
 * fully exercised with CheckJNI enabled.
 */
class ScopedJniThreadState {
public:
    explicit ScopedJniThreadState(JNIEnv *env) {
        mSelf = ((JNIEnvExt *) env)->self;

        CHECK_STACK_SUM(mSelf);
        dvmChangeStatus(mSelf, THREAD_RUNNING);
    }

    ~ScopedJniThreadState() {
        dvmChangeStatus(mSelf, THREAD_NATIVE);
        COMPUTE_STACK_SUM(mSelf);
    }

    inline Thread *self() {
        return mSelf;
    }

private:
    Thread *mSelf;

    // Disallow copy and assignment.
    ScopedJniThreadState(const ScopedJniThreadState &);

    void operator=(const ScopedJniThreadState &);
};

#endif /* __ScopedJniThreadState__ */