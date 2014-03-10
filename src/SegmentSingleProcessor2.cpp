/*
 *
 * (C) Copyright IBM Corp.  and others 1998-2013 - All Rights Reserved
 *
 */

#include "LETypes.h"
#include "LEGlyphStorage.h"
#include "LESwaps.h"
#include "SegmentSingleProcessor2.h"

U_NAMESPACE_BEGIN

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(SegmentSingleProcessor2)

SegmentSingleProcessor2::SegmentSingleProcessor2()
{
}

SegmentSingleProcessor2::SegmentSingleProcessor2(const LEReferenceTo<SegmentSingleLookupTable> &lookupTable, LEErrorCode & /* success */)
    : segmentSingleLookupTable(lookupTable)
{
}

SegmentSingleProcessor2::~SegmentSingleProcessor2()
{
}

void SegmentSingleProcessor2::process(LEGlyphStorage &glyphStorage, LEErrorCode &success)
{
    const LookupSegment *segments = segmentSingleLookupTable->segments;
    le_int32 glyphCount = glyphStorage.getGlyphCount();
    le_int32 glyph;

    for (glyph = 0; glyph < glyphCount; glyph += 1) {
        LEGlyphID thisGlyph = glyphStorage[glyph];
        const LookupSegment *lookupSegment = segmentSingleLookupTable->lookupSegment(segmentSingleLookupTable, segments, thisGlyph, success);

        if (lookupSegment != NULL && LE_SUCCESS(success)) {
            TTGlyphID   newGlyph  = (TTGlyphID) LE_GET_GLYPH(thisGlyph) + SWAPW(lookupSegment->value);
            glyphStorage[glyph]   = LE_SET_GLYPH(thisGlyph, newGlyph);
        }
    }
}

U_NAMESPACE_END
