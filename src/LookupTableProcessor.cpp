#include "LETypes.h"
#include "LESwaps.h"
#include "LookupTableProcessor.h"

U_NAMESPACE_BEGIN

LookupTableProcessor::~LookupTableProcessor()
{
}

LookupTableProcessor *LookupTableProcessor::createInstance(LookupTableFormat format, const LEReferenceTo<LookupTable> &lookupTable, LEErrorCode &success)
{
    switch (format) {
    case ltfSimpleArray:
        return new SimpleArrayLookupTableProcessor(lookupTable, success);

    case ltfSegmentSingle:
        return new SegmentSingleLookupTableProcessor(lookupTable, success);

    case ltfSegmentArray:
        return new SegmentArrayLookupTableProcessor(lookupTable, success);

    case ltfSingleTable:
        return new SingleTableLookupTableProcessor(lookupTable, success);

    case ltfTrimmedArray:
        return new TrimmedArrayLookupTableProcessor(lookupTable, success);

    default:
        return 0;
    }
}

SimpleArrayLookupTableProcessor::SimpleArrayLookupTableProcessor(const LEReferenceTo<LookupTable> &lookupTable, LEErrorCode &success)
    : simpleArrayLookupTable(lookupTable, success)
{
    if (LE_FAILURE(success))
        return;

    valueArray = LEReferenceToArrayOf<LookupValue>(simpleArrayLookupTable, success, &simpleArrayLookupTable->valueArray[0], LE_UNBOUNDED_ARRAY);
    valueCount = valueArray.getCount();
}

SimpleArrayLookupTableProcessor::~SimpleArrayLookupTableProcessor()
{
}

le_bool SimpleArrayLookupTableProcessor::lookup(LEGlyphID glyph, LookupValue &value, LEErrorCode &success)
{
    TTGlyphID ttGlyph = (TTGlyphID)LE_GET_GLYPH(glyph);

    if (ttGlyph == 0xFFFF)
        return FALSE;

    if (ttGlyph < valueCount) {
        TTGlyphID v = SWAPW(valueArray(ttGlyph, success));

        if (LE_SUCCESS(success)) {
            value = v;
            return TRUE;
        }
    } else {
        success = LE_INDEX_OUT_OF_BOUNDS_ERROR;
    }

    return FALSE;
}

SegmentSingleLookupTableProcessor::SegmentSingleLookupTableProcessor(const LEReferenceTo<LookupTable> &lookupTable, LEErrorCode &success)
    : segmentSingleLookupTable(lookupTable, success)
{
    if (LE_FAILURE(success))
        return;

    if (!segmentSingleLookupTable->validate(segmentSingleLookupTable, sizeof(LookupSegment)))
        success = LE_INDEX_OUT_OF_BOUNDS_ERROR;
}

SegmentSingleLookupTableProcessor::~SegmentSingleLookupTableProcessor()
{
}

le_bool SegmentSingleLookupTableProcessor::lookup(LEGlyphID glyph, LookupValue &value, LEErrorCode &success)
{
    TTGlyphID ttGlyph = (TTGlyphID)LE_GET_GLYPH(glyph);

    if (ttGlyph == 0xFFFF)
        return FALSE;

    const LookupSegment *lookupSegment = segmentSingleLookupTable->lookupSegment(segmentSingleLookupTable, segmentSingleLookupTable->segments, glyph, success);

    if (lookupSegment == NULL)
        return FALSE;

    value = SWAPW(lookupSegment->value);

    return TRUE;
}

SegmentArrayLookupTableProcessor::SegmentArrayLookupTableProcessor(const LEReferenceTo<LookupTable> &lookupTable, LEErrorCode &success)
    : segmentArrayLookupTable(lookupTable, success)
{
    if (LE_FAILURE(success))
        return;

    if (!segmentArrayLookupTable->validate(segmentArrayLookupTable, sizeof(LookupSegment)))
        success = LE_INDEX_OUT_OF_BOUNDS_ERROR;
}

SegmentArrayLookupTableProcessor::~SegmentArrayLookupTableProcessor()
{
}

le_bool SegmentArrayLookupTableProcessor::lookup(LEGlyphID glyph, LookupValue &value, LEErrorCode &success)
{
    TTGlyphID ttGlyph = (TTGlyphID)LE_GET_GLYPH(glyph);

    if (ttGlyph == 0xFFFF)
        return FALSE;

    const LookupSegment *lookupSegment = segmentArrayLookupTable->lookupSegment(segmentArrayLookupTable, segmentArrayLookupTable->segments, glyph, success);

    if (lookupSegment == NULL)
        return FALSE;

    TTGlyphID   firstGlyph = SWAPW(lookupSegment->firstGlyph);
    TTGlyphID   lastGlyph  = SWAPW(lookupSegment->lastGlyph);
    LookupValue offset     = SWAPW(lookupSegment->value);

    LEReferenceToArrayOf<LookupValue> glyphArray(segmentArrayLookupTable, success, offset, lastGlyph - firstGlyph + 1);

    if (LE_SUCCESS(success) && firstGlyph <= ttGlyph && ttGlyph <= lastGlyph) {
        LookupValue v = SWAPW(glyphArray(ttGlyph - firstGlyph, success));

        if (LE_SUCCESS(success)) {
            value = v;
            return TRUE;
        }
    }

    return FALSE;
}

SingleTableLookupTableProcessor::SingleTableLookupTableProcessor(const LEReferenceTo<LookupTable> &lookupTable, LEErrorCode &success)
    : singleTableLookupTable(lookupTable, success)
{
    if (LE_FAILURE(success))
        return;

    if (!singleTableLookupTable->validate(singleTableLookupTable, sizeof(LookupSingle)))
        success = LE_INDEX_OUT_OF_BOUNDS_ERROR;
}

SingleTableLookupTableProcessor::~SingleTableLookupTableProcessor()
{
}

le_bool SingleTableLookupTableProcessor::lookup(LEGlyphID glyph, LookupValue &value, LEErrorCode &success)
{
    TTGlyphID ttGlyph = (TTGlyphID)LE_GET_GLYPH(glyph);

    if (ttGlyph == 0xFFFF)
        return FALSE;

    const LookupSingle *lookupSingle = singleTableLookupTable->lookupSingle(singleTableLookupTable, singleTableLookupTable->entries, glyph, success);

    if (lookupSingle == NULL)
        return FALSE;

    value = SWAPW(lookupSingle->value);

    return TRUE;
}

TrimmedArrayLookupTableProcessor::TrimmedArrayLookupTableProcessor(const LEReferenceTo<LookupTable> &lookupTable, LEErrorCode &success)
    : trimmedArrayLookupTable(lookupTable, success)
{
    if (LE_FAILURE(success))
        return;

    firstGlyph = SWAPW(trimmedArrayLookupTable->firstGlyph);
    glyphCount = SWAPW(trimmedArrayLookupTable->glyphCount);
    limitGlyph = firstGlyph + glyphCount;

    valueArray = LEReferenceToArrayOf<LookupValue>(trimmedArrayLookupTable, success, &trimmedArrayLookupTable->valueArray[0], glyphCount);
}

TrimmedArrayLookupTableProcessor::~TrimmedArrayLookupTableProcessor()
{
}

le_bool TrimmedArrayLookupTableProcessor::lookup(LEGlyphID glyph, LookupValue &value, LEErrorCode &success)
{
    TTGlyphID ttGlyph = (TTGlyphID)LE_GET_GLYPH(glyph);

    if (ttGlyph == 0xFFFF)
        return FALSE;

    if (firstGlyph <= ttGlyph && ttGlyph < limitGlyph) {
        LookupValue v = SWAPW(valueArray(ttGlyph - firstGlyph, success));

        if (LE_SUCCESS(success)) {
            value = v;
            return TRUE;
        }
    }

    return FALSE;
}

U_NAMESPACE_END
