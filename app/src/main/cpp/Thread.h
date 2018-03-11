/*
 * Copyright (C) 2008 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * VM thread support.
 */
#ifndef DALVIK_THREAD_H_
#define DALVIK_THREAD_H_

#include "jni.h"
#include "InterpState.h"
#include "Object.h"
#include "IndirectRefTable.h"
#include "ReferenceTable.h"

#include <errno.h>
#include <pthread.h>
#include <string>

#if defined(CHECK_MUTEX) && !defined(__USE_UNIX98)
/* glibc lacks this unless you #define __USE_UNIX98 */
int pthread_mutexattr_settype(pthread_mutexattr_t *attr, int type);
enum { PTHREAD_MUTEX_ERRORCHECK = PTHREAD_MUTEX_ERRORCHECK_NP };
#endif

/*
 * Current status; these map to JDWP constants, so don't rearrange them.
 * (If you do alter this, update the strings in dvmDumpThread and the
 * conversion table in VMThread.java.)
 *
 * Note that "suspended" is orthogonal to these values (so says JDWP).
 */
enum ThreadStatus {
    THREAD_UNDEFINED    = -1,       /* makes enum compatible with int32_t */

    /* these match up with JDWP values */
            THREAD_ZOMBIE       = 0,        /* TERMINATED */
    THREAD_RUNNING      = 1,        /* RUNNABLE or running now */
    THREAD_TIMED_WAIT   = 2,        /* TIMED_WAITING in Object.wait() */
    THREAD_MONITOR      = 3,        /* BLOCKED on a monitor */
    THREAD_WAIT         = 4,        /* WAITING in Object.wait() */
    /* non-JDWP states */
            THREAD_INITIALIZING = 5,        /* allocated, not yet running */
    THREAD_STARTING     = 6,        /* started, not yet on thread list */
    THREAD_NATIVE       = 7,        /* off in a JNI native method */
    THREAD_VMWAIT       = 8,        /* waiting on a VM resource */
    THREAD_SUSPENDED    = 9,        /* suspended, usually by GC or debugger */
};

/* thread priorities, from java.lang.Thread */
enum {
    THREAD_MIN_PRIORITY     = 1,
    THREAD_NORM_PRIORITY    = 5,
    THREAD_MAX_PRIORITY     = 10,
};

#ifdef WITH_TAINT_TRACKING
# define kMinStackSize       (1024 + STACK_OVERFLOW_RESERVE)
# define kDefaultStackSize   (32*1024)    /* eight 4K pages */
# define kMaxStackSize       (512*1024 + STACK_OVERFLOW_RESERVE)
#else
#define kMinStackSize       (512 + STACK_OVERFLOW_RESERVE)
#define kDefaultStackSize   (16*1024)   /* four 4K pages */
#define kMaxStackSize       (256*1024 + STACK_OVERFLOW_RESERVE)
#endif

/*
 * Interpreter control struction.  Packed into a long long to enable
 * atomic updates.
 */
union InterpBreak {
    volatile int64_t   all;
    struct {
        uint16_t   subMode;
        uint8_t    breakFlags;
        int8_t     unused;   /* for future expansion */
#ifndef DVM_NO_ASM_INTERP
        void* curHandlerTable;
#else
        int32_t    unused1;
#endif
    } ctl;
};

class Monitor;
typedef u8 ObjectId;    /* any object (threadID, stringID, arrayID, etc) */

/*
 * Error constants.
 */
enum JdwpError {
    ERR_NONE                                        = 0,
    ERR_INVALID_THREAD                              = 10,
    ERR_INVALID_THREAD_GROUP                        = 11,
    ERR_INVALID_PRIORITY                            = 12,
    ERR_THREAD_NOT_SUSPENDED                        = 13,
    ERR_THREAD_SUSPENDED                            = 14,
    ERR_INVALID_OBJECT                              = 20,
    ERR_INVALID_CLASS                               = 21,
    ERR_CLASS_NOT_PREPARED                          = 22,
    ERR_INVALID_METHODID                            = 23,
    ERR_INVALID_LOCATION                            = 24,
    ERR_INVALID_FIELDID                             = 25,
    ERR_INVALID_FRAMEID                             = 30,
    ERR_NO_MORE_FRAMES                              = 31,
    ERR_OPAQUE_FRAME                                = 32,
    ERR_NOT_CURRENT_FRAME                           = 33,
    ERR_TYPE_MISMATCH                               = 34,
    ERR_INVALID_SLOT                                = 35,
    ERR_DUPLICATE                                   = 40,
    ERR_NOT_FOUND                                   = 41,
    ERR_INVALID_MONITOR                             = 50,
    ERR_NOT_MONITOR_OWNER                           = 51,
    ERR_INTERRUPT                                   = 52,
    ERR_INVALID_CLASS_FORMAT                        = 60,
    ERR_CIRCULAR_CLASS_DEFINITION                   = 61,
    ERR_FAILS_VERIFICATION                          = 62,
    ERR_ADD_METHOD_NOT_IMPLEMENTED                  = 63,
    ERR_SCHEMA_CHANGE_NOT_IMPLEMENTED               = 64,
    ERR_INVALID_TYPESTATE                           = 65,
    ERR_HIERARCHY_CHANGE_NOT_IMPLEMENTED            = 66,
    ERR_DELETE_METHOD_NOT_IMPLEMENTED               = 67,
    ERR_UNSUPPORTED_VERSION                         = 68,
    ERR_NAMES_DONT_MATCH                            = 69,
    ERR_CLASS_MODIFIERS_CHANGE_NOT_IMPLEMENTED      = 70,
    ERR_METHOD_MODIFIERS_CHANGE_NOT_IMPLEMENTED     = 71,
    ERR_NOT_IMPLEMENTED                             = 99,
    ERR_NULL_POINTER                                = 100,
    ERR_ABSENT_INFORMATION                          = 101,
    ERR_INVALID_EVENT_TYPE                          = 102,
    ERR_ILLEGAL_ARGUMENT                            = 103,
    ERR_OUT_OF_MEMORY                               = 110,
    ERR_ACCESS_DENIED                               = 111,
    ERR_VM_DEAD                                     = 112,
    ERR_INTERNAL                                    = 113,
    ERR_UNATTACHED_THREAD                           = 115,
    ERR_INVALID_TAG                                 = 500,
    ERR_ALREADY_INVOKING                            = 502,
    ERR_INVALID_INDEX                               = 503,
    ERR_INVALID_LENGTH                              = 504,
    ERR_INVALID_STRING                              = 506,
    ERR_INVALID_CLASS_LOADER                        = 507,
    ERR_INVALID_ARRAY                               = 508,
    ERR_TRANSPORT_LOAD                              = 509,
    ERR_TRANSPORT_INIT                              = 510,
    ERR_NATIVE_METHOD                               = 511,
    ERR_INVALID_COUNT                               = 512,
};

/*
 * Invoke-during-breakpoint support.
 */
struct DebugInvokeReq {
    /* boolean; only set when we're in the tail end of an event handler */
    bool ready;

    /* boolean; set if the JDWP thread wants this thread to do work */
    bool invokeNeeded;

    /* request */
    Object* obj;        /* not used for ClassType.InvokeMethod */
    Object* thread;
    ClassObject* clazz;
    Method* method;
    u4 numArgs;
    u8* argArray;   /* will be NULL if numArgs==0 */
    u4 options;

    /* result */
    JdwpError err;
    u1 resultTag;
    JValue resultValue;
    ObjectId exceptObj;

    /* condition variable to wait on while the method executes */
    pthread_mutex_t lock;
    pthread_cond_t cv;
};

/*
 * Memory allocation profiler state.  This is used both globally and
 * per-thread.
 *
 * If you add a field here, zero it out in dvmStartAllocCounting().
 */
struct AllocProfState {
    bool    enabled;            // is allocation tracking enabled?

    int     allocCount;         // #of objects allocated
    int     allocSize;          // cumulative size of objects

    int     failedAllocCount;   // #of times an allocation failed
    int     failedAllocSize;    // cumulative size of failed allocations

    int     freeCount;          // #of objects freed
    int     freeSize;           // cumulative size of freed objects

    int     gcCount;            // #of times an allocation triggered a GC

    int     classInitCount;     // #of initialized classes
    u8      classInitTime;      // cumulative time spent in class init (nsec)
};

/*
 * Our per-thread data.
 *
 * These are allocated on the system heap.
 */
struct Thread {
    /*
     * Interpreter state which must be preserved across nested
     * interpreter invocations (via JNI callbacks).  Must be the first
     * element in Thread.
     */
    InterpSaveState interpSave;

    /* small unique integer; useful for "thin" locks and debug messages */
    u4          threadId;

    /*
     * Begin interpreter state which does not need to be preserved, but should
     * be located towards the beginning of the Thread structure for
     * efficiency.
     */

    /*
     * interpBreak contains info about the interpreter mode, as well as
     * a count of the number of times the thread has been suspended.  When
     * the count drops to zero, the thread resumes.
     */
    InterpBreak interpBreak;

    /*
     * "dbgSuspendCount" is the portion of the suspend count that the
     * debugger is responsible for.  This has to be tracked separately so
     * that we can recover correctly if the debugger abruptly disconnects
     * (suspendCount -= dbgSuspendCount).  The debugger should not be able
     * to resume GC-suspended threads, because we ignore the debugger while
     * a GC is in progress.
     *
     * Both of these are guarded by gDvm.threadSuspendCountLock.
     *
     * Note the non-debug component will rarely be other than 1 or 0 -- (not
     * sure it's even possible with the way mutexes are currently used.)
     */

    int suspendCount;
    int dbgSuspendCount;

    u1*         cardTable;

    /* current limit of stack; flexes for StackOverflowError */
    const u1*   interpStackEnd;

    /* FP of bottom-most (currently executing) stack frame on interp stack */
    void*       XcurFrame;
    /* current exception, or NULL if nothing pending */
    Object*     exception;

    bool        debugIsMethodEntry;
    /* interpreter stack size; our stacks are fixed-length */
    int         interpStackSize;
    bool        stackOverflowed;

    /* thread handle, as reported by pthread_self() */
    pthread_t   handle;

    /* Assembly interpreter handler tables */
#ifndef DVM_NO_ASM_INTERP
    void*       mainHandlerTable;   // Table of actual instruction handler
    void*       altHandlerTable;    // Table of breakout handlers
#else
    void*       unused0;            // Consume space to keep offsets
    void*       unused1;            //   the same between builds with
#endif

    /*
     * singleStepCount is a countdown timer used with the breakFlag
     * kInterpSingleStep.  If kInterpSingleStep is set in breakFlags,
     * singleStepCount will decremented each instruction execution.
     * Once it reaches zero, the kInterpSingleStep flag in breakFlags
     * will be cleared.  This can be used to temporarily prevent
     * execution from re-entering JIT'd code or force inter-instruction
     * checks by delaying the reset of curHandlerTable to mainHandlerTable.
     */
    int         singleStepCount;

#ifdef WITH_JIT
/* States of the interpreter when serving a JIT-related request */
    enum JitState {
        /* Entering states in the debug interpreter */
                kJitNot = 0,               // Non-JIT related reasons */
        kJitTSelectRequest = 1,    // Request a trace (subject to filtering)
        kJitTSelectRequestHot = 2, // Request a hot trace (bypass the filter)
        kJitSelfVerification = 3,  // Self Verification Mode

        /* Operational states in the debug interpreter */
                kJitTSelect = 4,           // Actively selecting a trace
        kJitTSelectEnd = 5,        // Done with the trace - wrap it up
        kJitDone = 6,              // No further JIT actions for interpBreak
    };

    struct JitToInterpEntries {
        void (*dvmJitToInterpNormal)(void);
        void (*dvmJitToInterpNoChain)(void);
        void (*dvmJitToInterpPunt)(void);
        void (*dvmJitToInterpSingleStep)(void);
        void (*dvmJitToInterpTraceSelect)(void);
#if defined(WITH_SELF_VERIFICATION)
        void (*dvmJitToInterpBackwardBranch)(void);
#else
        void (*unused)(void);  // Keep structure size constant
#endif
    };

    struct JitToInterpEntries jitToInterpEntries;
    /*
     * Whether the current top VM frame is in the interpreter or JIT cache:
     *   NULL    : in the interpreter
     *   non-NULL: entry address of the JIT'ed code (the actual value doesn't
     *             matter)
     */
    void*             inJitCodeCache;
    unsigned char*    pJitProfTable;
    int               jitThreshold;
    const void*       jitResumeNPC;     // Translation return point
    const u4*         jitResumeNSP;     // Native SP at return point
    const u2*         jitResumeDPC;     // Dalvik inst following single-step
    JitState    jitState;
    int         icRechainCount;
    const void* pProfileCountdown;
    const ClassObject* callsiteClass;
    const Method*     methodToCall;
#endif /* WITH_JIT */

    /* JNI local reference tracking */
    IndirectRefTable jniLocalRefTable;

#if defined(WITH_JIT)
    #if defined(WITH_SELF_VERIFICATION)
    /* Buffer for register state during self verification */
    struct ShadowSpace* shadowSpace;
#endif
    int         currTraceRun;
    int         totalTraceLen;  // Number of Dalvik insts in trace
    const u2*   currTraceHead;  // Start of the trace we're building
    const u2*   currRunHead;    // Start of run we're building
    int         currRunLen;     // Length of run in 16-bit words
    const u2*   lastPC;         // Stage the PC for the threaded interpreter
    const Method*  traceMethod; // Starting method of current trace
    intptr_t    threshFilter[JIT_TRACE_THRESH_FILTER_SIZE];
    JitTraceRun trace[MAX_JIT_RUN_LEN];
#endif

    /*
     * Thread's current status.  Can only be changed by the thread itself
     * (i.e. don't mess with this from other threads).
     */
    volatile ThreadStatus status;

    /* thread ID, only useful under Linux */
    pid_t       systemTid;

    /* start (high addr) of interp stack (subtract size to get malloc addr) */
    u1*         interpStackStart;

    /* the java/lang/Thread that we are associated with */
    Object*     threadObj;

    /* the JNIEnv pointer associated with this thread */
    JNIEnv*     jniEnv;

    /* internal reference tracking */
    ReferenceTable  internalLocalRefTable;


    /* JNI native monitor reference tracking (initialized on first use) */
    ReferenceTable  jniMonitorRefTable;

    /* hack to make JNI_OnLoad work right */
    Object*     classLoaderOverride;

    /* mutex to guard the interrupted and the waitMonitor members */
    pthread_mutex_t    waitMutex;

    /* pointer to the monitor lock we're currently waiting on */
    /* guarded by waitMutex */
    /* TODO: consider changing this to Object* for better JDWP interaction */
    Monitor*    waitMonitor;

    /* thread "interrupted" status; stays raised until queried or thrown */
    /* guarded by waitMutex */
    bool        interrupted;

    /* links to the next thread in the wait set this thread is part of */
    struct Thread*     waitNext;

    /* object to sleep on while we are waiting for a monitor */
    pthread_cond_t     waitCond;

    /*
     * Set to true when the thread is in the process of throwing an
     * OutOfMemoryError.
     */
    bool        throwingOOME;

    /* links to rest of thread list; grab global lock before traversing */
    struct Thread* prev;
    struct Thread* next;

    /* used by threadExitCheck when a thread exits without detaching */
    int         threadExitCheckCount;

    /* JDWP invoke-during-breakpoint support */
    DebugInvokeReq  invokeReq;

    /* base time for per-thread CPU timing (used by method profiling) */
    bool        cpuClockBaseSet;
    u8          cpuClockBase;

    /* memory allocation profiling state */
    AllocProfState allocProf;

#ifdef WITH_JNI_STACK_CHECK
    u4          stackCrc;
#endif

#if WITH_EXTRA_GC_CHECKS > 1
    /* PC, saved on every instruction; redundant with StackSaveArea */
    const u2*   currentPc2;
#endif

    /* Safepoint callback state */
    pthread_mutex_t   callbackMutex;
    SafePointCallback callback;
    void*             callbackArg;

#if defined(ARCH_IA32) && defined(WITH_JIT)
    u4 spillRegion[MAX_SPILL_JIT_IA];
#endif
};

/* start point for an internal thread; mimics pthread args */
typedef void* (*InternalThreadStart)(void* arg);

/* args for internal thread creation */
struct InternalStartArgs {
    /* inputs */
    InternalThreadStart func;
    void*       funcArg;
    char*       name;
    Object*     group;
    bool        isDaemon;
    /* result */
    volatile Thread** pThread;
    volatile int*     pCreateStatus;
};

/*
 * Thread suspend/resume, used by the GC and debugger.
 */
enum SuspendCause {
    SUSPEND_NOT = 0,
    SUSPEND_FOR_GC,
    SUSPEND_FOR_DEBUG,
    SUSPEND_FOR_DEBUG_EVENT,
    SUSPEND_FOR_STACK_DUMP,
    SUSPEND_FOR_DEX_OPT,
    SUSPEND_FOR_VERIFY,
    SUSPEND_FOR_HPROF,
#if defined(WITH_JIT)
    SUSPEND_FOR_TBL_RESIZE,  // jit-table resize
    SUSPEND_FOR_IC_PATCH,    // polymorphic callsite inline -cache patch
    SUSPEND_FOR_CC_RESET,    // code-cache reset
    SUSPEND_FOR_REFRESH,     // Reload data cached in interpState
#endif
};

/*
 * Used when changing thread state.  Threads may only change their own.
 * The "self" argument, which may be NULL, is accepted as an optimization.
 *
 * If you're calling this before waiting on a resource (e.g. THREAD_WAIT
 * or THREAD_MONITOR), do so in the same function as the wait -- this records
 * the current stack depth for the GC.
 *
 * If you're changing to THREAD_RUNNING, this will check for suspension.
 *
 * Returns the old status.
 */
ThreadStatus dvmChangeStatus(Thread* self, ThreadStatus newStatus);


#endif  // DALVIK_THREAD_H_
