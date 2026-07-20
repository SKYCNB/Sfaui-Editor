#include "Sfaui_fitType.h"
using namespace nonstd;

AnyType GetAnyType(const any& item)
{
    if (!item.has_value())
        return AnyType_Empty;
    for (size_t i = 0; i < typeTableCount; ++i)
    {
        const auto& entry = typeTable[i];
        if (*entry.tinfo == item.type())
            return entry.type;
    }
    return AnyType_Unknown;
}

