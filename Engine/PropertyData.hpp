#pragma once

#include <string>

#include <angelscript.h>

struct PropertyData {
    std::string name;
    std::string typeName;
    int typeId;
    bool isPrivate;
    bool isProtected;
    int offset;
    bool isReference;
    asDWORD accessMask;
    int compositeOffset;
    bool isCompositeIndirect;

    std::string prettyName;

    asITypeInfo* typeInfo;
};