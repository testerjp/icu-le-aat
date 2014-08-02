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

    inline le_bool validate(const LETableReference &pairs) const {
        le_uint16 unity = sizeof(KerningPair);
        le_uint16 probe = SWAPW(searchRange);
        le_uint16 extra = SWAPW(rangeShift);

        if (probe % unity == 0 && (probe / unity & (probe / unity - 1)) == 0 && (!pairs.hasBounds() || probe + extra < pairs.getLength()))
            return TRUE;

        return FALSE;
    }
};
LE_VAR_ARRAY(OrderedListKerningPairsHeader, entries);

U_NAMESPACE_END

#endif
