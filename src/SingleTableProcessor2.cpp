/*
 *
 * (C) Copyright IBM Corp.  and others 1998-2013 - All Rights Reserved
 *
 */

#include "LETypes.h"
#include "LEGlyphStorage.h"
#include "LESwaps.h"
#include "SingleTableProcessor2.h"

U_NAMESPACE_BEGIN

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(SingleTableProcessor2)

SingleTableProcessor2::SingleTableProcessor2()
{
}

SingleTableProcessor2::SingleTableProcessor2(const LEReferenceTo<SingleTableLookupTable> &lookupTable, LEErrorCode & /* success */)
    : singleTableLookupTable(lookupTable)
{
}

SingleTableProcessor2::~SingleTableProcessor2()
{
}

void SingleTableProcessor2::process(LEGlyphStorage &glyphStorage, LEErrorCode &success)
{
    const LookupSingle *entries = singleTableLookupTable->entries;
    le_int32 glyph;
    le_int32 glyphCount = glyphStorage.getGlyphCount();

    for (glyph = 0; glyph < glyphCount; glyph += 1) {
        const LookupSingle *lookupSingle = singleTableLookupTable->lookupSingle(singleTableLookupTable, entries, glyphStorage[glyph], success);

        if (lookupSingle != NULL) {
            glyphStorage[glyph] = SWAPW(lookupSingle->value);
        }
    }
}

U_NAMESPACE_END
