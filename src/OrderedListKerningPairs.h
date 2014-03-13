#ifndef __ORDEREDLLISTKERNINGPAIRS_H
#define __ORDEREDLLISTKERNINGPAIRS_H

#include "LETypes.h"
#include "LETableReference.h"
#include "LayoutTables.h"

U_NAMESPACE_BEGIN

struct KerningPair {
    le_uint16 left;
    le_uint16 right;
    le_int16  value;

    inline le_uint32 key() const {
        return (le_uint32)SWAPW(left) << 16 | SWAPW(right);
    };
};

struct OrderedListKerningPairsHeader {
    le_uint16 nPairs;
    le_uint16 searchRange;
    le_uint16 entrySelector;
    le_uint16 rangeShift;

    KerningPair entries[ANY_NUMBER];
};
LE_VAR_ARRAY(OrderedListKerningPairsHeader, entries);

U_NAMESPACE_END

#endif
