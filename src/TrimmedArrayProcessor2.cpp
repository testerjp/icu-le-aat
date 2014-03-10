/*
 *
 * (C) Copyright IBM Corp.  and others 1998-2013 - All Rights Reserved
 *
 */

#include "LETypes.h"
#include "LEGlyphStorage.h"
#include "LESwaps.h"
#include "TrimmedArrayProcessor2.h"

U_NAMESPACE_BEGIN

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(TrimmedArrayProcessor2)

TrimmedArrayProcessor2::TrimmedArrayProcessor2()
{
}

TrimmedArrayProcessor2::TrimmedArrayProcessor2(const LEReferenceTo<TrimmedArrayLookupTable> &lookupTable, LEErrorCode &success)
    : firstGlyph(0), lastGlyph(0),
      trimmedArrayLookupTable(lookupTable)
{
    if (LE_FAILURE(success)) return;

    firstGlyph = SWAPW(trimmedArrayLookupTable->firstGlyph);
    lastGlyph  = firstGlyph + SWAPW(trimmedArrayLookupTable->glyphCount);
}

TrimmedArrayProcessor2::~TrimmedArrayProcessor2()
{
}

void TrimmedArrayProcessor2::process(LEGlyphStorage &glyphStorage, LEErrorCode &success)
{
    if (LE_FAILURE(success)) return;

    le_int32 glyphCount = glyphStorage.getGlyphCount();
    le_int32 glyph;

    LEReferenceToArrayOf<LookupValue> valueArray = LEReferenceToArrayOf<LookupValue>(trimmedArrayLookupTable, success, &trimmedArrayLookupTable->valueArray[0], LE_UNBOUNDED_ARRAY);

    for (glyph = 0; glyph < glyphCount; glyph += 1) {
        LEGlyphID thisGlyph = glyphStorage[glyph];
        TTGlyphID ttGlyph = (TTGlyphID) LE_GET_GLYPH(thisGlyph);

        if ((ttGlyph > firstGlyph) && (ttGlyph < lastGlyph)) {
            TTGlyphID newGlyph  = SWAPW(valueArray(ttGlyph - firstGlyph, success));
            glyphStorage[glyph] = LE_SET_GLYPH(thisGlyph, newGlyph);
        }
    }
}

U_NAMESPACE_END
