/*
 *
 * (C) Copyright IBM Corp.  and others 1998-2013 - All Rights Reserved
 *
 */

#include "LETypes.h"
#include "LEGlyphStorage.h"
#include "LESwaps.h"
#include "SimpleArrayProcessor2.h"

U_NAMESPACE_BEGIN

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(SimpleArrayProcessor2)

SimpleArrayProcessor2::SimpleArrayProcessor2()
{
}

SimpleArrayProcessor2::SimpleArrayProcessor2(const LEReferenceTo<SimpleArrayLookupTable> &lookupTable, LEErrorCode & /* success */)
    : simpleArrayLookupTable(lookupTable)
{
}

SimpleArrayProcessor2::~SimpleArrayProcessor2()
{
}

void SimpleArrayProcessor2::process(LEGlyphStorage &glyphStorage, LEErrorCode &success)
{
    le_int32 glyphCount = glyphStorage.getGlyphCount();
    le_int32 glyph;

    LEReferenceToArrayOf<LookupValue> valueArray(simpleArrayLookupTable, success, (const LookupValue*)&simpleArrayLookupTable->valueArray, LE_UNBOUNDED_ARRAY);

    for (glyph = 0; LE_SUCCESS(success) && (glyph < glyphCount); glyph += 1) {
        LEGlyphID thisGlyph = glyphStorage[glyph];
        if (LE_GET_GLYPH(thisGlyph) < 0xFFFF) {
            TTGlyphID newGlyph  = SWAPW(valueArray.getObject(LE_GET_GLYPH(thisGlyph),success));
            glyphStorage[glyph] = LE_SET_GLYPH(thisGlyph, newGlyph);
        }
    }
}

U_NAMESPACE_END
