/*
 *
 * (C) Copyright IBM Corp. 1998-2013 - All Rights Reserved
 *
 */

#include "LETypes.h"
#include "LEGlyphStorage.h"
#include "LESwaps.h"
#include "SimpleArrayProcessor.h"

U_NAMESPACE_BEGIN

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(SimpleArrayProcessor)

SimpleArrayProcessor::SimpleArrayProcessor()
{
}

SimpleArrayProcessor::SimpleArrayProcessor(const LEReferenceTo<SimpleArrayLookupTable> &lookupTable, LEErrorCode & /* success */)
    : simpleArrayLookupTable(lookupTable)
{
}

SimpleArrayProcessor::~SimpleArrayProcessor()
{
}

void SimpleArrayProcessor::process(LEGlyphStorage &glyphStorage, LEErrorCode &success)
{
    if (LE_FAILURE(success))
        return;

    le_int32 glyphCount = glyphStorage.getGlyphCount();
    le_int32 glyph;

    LEReferenceToArrayOf<LookupValue> valueArray(simpleArrayLookupTable, success, (const LookupValue*)&simpleArrayLookupTable->valueArray, LE_UNBOUNDED_ARRAY);

    for (glyph = 0; LE_SUCCESS(success) && glyph < glyphCount; glyph += 1) {
        LEGlyphID thisGlyph = glyphStorage[glyph];
        if (LE_GET_GLYPH(thisGlyph) < 0xFFFF) {
            TTGlyphID newGlyph  = SWAPW(valueArray.getObject(LE_GET_GLYPH(thisGlyph),success));
            glyphStorage[glyph] = LE_SET_GLYPH(thisGlyph, newGlyph);
        }
    }
}

U_NAMESPACE_END
