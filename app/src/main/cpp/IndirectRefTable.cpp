#include "IndirectRefTable.h"

Object *IndirectRefTable::get(IndirectRef iref) const {
    IndirectRefKind kind = indirectRefKind(iref);

    if (kind != kind_) {
        if (iref == NULL) {
            return kInvalidIndirectRefObject;
        }
        if (kind == kIndirectKindInvalid) {
            return kInvalidIndirectRefObject;
        }
        // References of the requested kind cannot appear within this table.
        return kInvalidIndirectRefObject;
    }

    u4 topIndex = segmentState.parts.topIndex;
    u4 index = (((u4) iref) >> 2) & 0xffff;
    if (index >= topIndex) {
        /* bad -- stale reference? */
        return kInvalidIndirectRefObject;
    }

    Object *obj = table_[index].obj;

    if (obj == NULL) {
        return kInvalidIndirectRefObject;
    }
    return obj;
}