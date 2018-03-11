#include "Thread.h"

/*
 * Update our status.
 *
 * The "self" argument, which may be NULL, is accepted as an optimization.
 *
 * Returns the old status.
 */
ThreadStatus dvmChangeStatus(Thread *self, ThreadStatus newStatus) {
    ThreadStatus oldStatus;

    LOGVV("threadid=%d: (status %d -> %d)",
          self->threadId, self->status, newStatus);

    oldStatus = (ThreadStatus) self->status;
    if (oldStatus == newStatus)
        return oldStatus;

    return oldStatus;
}