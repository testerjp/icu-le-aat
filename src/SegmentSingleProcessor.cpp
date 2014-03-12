/*
 *
 * (C) Copyright IBM Corp. 1998-2013 - All Rights Reserved
 *
 */

#include "LETypes.h"
#include "LEGlyphStorage.h"
#include "LESwaps.h"
#include "SegmentSingleProcessor.h"

U_NAMESPACE_BEGIN

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(SegmentSingleProcessor)

SegmentSingleProcessor::SegmentSingleProcessor()
{
}

SegmentSingleProcessor::SegmentSingleProcessor(const LEReferenceTo<SegmentSingleLookupTable> &lookupTable, LEErrorCode & /* success */)
    : segmentSingleLookupTable(lookupTable)
{
}

SegmentSingleProcessor::~SegmentSingleProcessor()
{
}

void SegmentSingleProcessor::process(LEGlyphStorage &glyphStorage, LEErrorCode &success)
{
    if (LE_FAILURE(success))
        return;

    const LookupSegment *segments = segmentSingleLookupTable->segments;
    le_int32 glyphCount = glyphStorage.getGlyphCount();
    le_int32 glyph;

    for (glyph = 0; LE_SUCCESS(success) && glyph < glyphCount; glyph += 1) {
        LEGlyphID thisGlyph = glyphStorage[glyph];
        const LookupSegment *lookupSegment = segmentSingleLookupTable->lookupSegment(segmentSingleLookupTable, segments, thisGlyph, success);

        if (lookupSegment != NULL && LE_SUCCESS(success)) {
            TTGlyphID   newGlyph = (TTGlyphID) LE_GET_GLYPH(thisGlyph) + SWAPW(lookupSegment->value);
            glyphStorage[glyph]  = LE_SET_GLYPH(thisGlyph, newGlyph);
        }
    }
}

U_NAMESPACE_END
