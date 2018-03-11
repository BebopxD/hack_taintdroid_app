#include "Object.h"

ArrayObject* StringObject::array() const
{
    return (ArrayObject*) dvmGetFieldObject(this, 8 /* magic number appears... */);
}