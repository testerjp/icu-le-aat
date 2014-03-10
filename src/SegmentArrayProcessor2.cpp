/*
 *
 * (C) Copyright IBM Corp.  and others 1998-2013 - All Rights Reserved
 *
 */

#include "LETypes.h"
#include "LEGlyphStorage.h"
#include "LESwaps.h"
#include "SegmentArrayProcessor2.h"

U_NAMESPACE_BEGIN

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(SegmentArrayProcessor2)

SegmentArrayProcessor2::SegmentArrayProcessor2()
{
}

SegmentArrayProcessor2::SegmentArrayProcessor2(const LEReferenceTo<SegmentArrayLookupTable> &lookupTable, LEErrorCode & /* success */)
    : segmentArrayLookupTable(lookupTable)
{
}

SegmentArrayProcessor2::~SegmentArrayProcessor2()
{
}

void SegmentArrayProcessor2::process(LEGlyphStorage &glyphStorage, LEErrorCode &success)
{
    const LookupSegment *segments = segmentArrayLookupTable->segments;
    le_int32 glyphCount = glyphStorage.getGlyphCount();
    le_int32 glyph;

    for (glyph = 0; glyph < glyphCount; glyph += 1) {
        LEGlyphID thisGlyph = glyphStorage[glyph];
        const LookupSegment *lookupSegment = segmentArrayLookupTable->lookupSegment(segmentArrayLookupTable, segments, thisGlyph, success);

        if (lookupSegment != NULL)  {
            TTGlyphID firstGlyph = SWAPW(lookupSegment->firstGlyph);
            le_int16  offset     = SWAPW(lookupSegment->value);

            if (offset != 0) {
                LEReferenceToArrayOf<TTGlyphID> glyphArray(segmentArrayLookupTable, success, offset, LE_UNBOUNDED_ARRAY);
                TTGlyphID newGlyph  = SWAPW(glyphArray(LE_GET_GLYPH(thisGlyph) - firstGlyph, success));
                glyphStorage[glyph] = LE_SET_GLYPH(thisGlyph, newGlyph);
            }
        }
    }
}

U_NAMESPACE_END
