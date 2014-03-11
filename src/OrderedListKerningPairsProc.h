#ifndef _ORDEREDLISTKERNINGPAIRSPROC_H
#define _ORDEREDLISTKERNINGPAIRSPROC_H

/**
 * \file
 * \internal
 */

#include "LETypes.h"
#include "LETableReference.h"
#include "SubtableProcessor.h"
#include "KernTables.h"

U_NAMESPACE_BEGIN

class OrderedListKerningPairsProcessor : public SubtableProcessor
{
public:
    void process(LEGlyphStorage &glyphStorage, LEErrorCode &success);

    OrderedListKerningPairsProcessor(LEReferenceTo<KernSubtableKerningPairs> subtableHeader, LEErrorCode &success);

protected:
    OrderedListKerningPairsProcessor();
    virtual ~OrderedListKerningPairsProcessor();

    le_uint16 nPairs;
    le_uint16 searchRange;
    le_uint16 entrySelector;
    le_uint16 rangeShift;

    LEReferenceTo<KernSubtableKerningPairs> subtableHeader;
    LEReferenceToArrayOf<KerningPair> pairs;

private:
    OrderedListKerningPairsProcessor(const OrderedListKerningPairsProcessor &other); // forbid copying of this class
    OrderedListKerningPairsProcessor &operator=(const OrderedListKerningPairsProcessor &other); // forbid copying of this class
};

U_NAMESPACE_END

#endif
