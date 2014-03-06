/*
 *
 * (C) Copyright IBM Corp.  and others 1998-2013 - All Rights Reserved
 *
 */

#include "LETypes.h"
#include "MorphTables.h"
#include "StateTables.h"
#include "MorphStateTables.h"
#include "SubtableProcessor2.h"
#include "StateTableProcessor2.h"
#include "ContextualGlyphSubstProc2.h"
#include "LEGlyphStorage.h"
#include "LESwaps.h"

U_NAMESPACE_BEGIN

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(ContextualGlyphSubstitutionProcessor2)

ContextualGlyphSubstitutionProcessor2::ContextualGlyphSubstitutionProcessor2(
                                  const LEReferenceTo<MorphSubtableHeader2> &morphSubtableHeader, LEErrorCode &success)
  : StateTableProcessor2(morphSubtableHeader, success), contextualGlyphHeader(morphSubtableHeader, success)
{
    le_uint32 perGlyphTableOffset = SWAPL(contextualGlyphHeader->perGlyphTableOffset);
    perGlyphTable = LEReferenceToArrayOf<le_uint32> (stHeader, success, perGlyphTableOffset, LE_UNBOUNDED_ARRAY);
    entryTable = LEReferenceToArrayOf<ContextualGlyphStateEntry2>(stHeader, success, entryTableOffset, LE_UNBOUNDED_ARRAY);
}

ContextualGlyphSubstitutionProcessor2::~ContextualGlyphSubstitutionProcessor2()
{
}

void ContextualGlyphSubstitutionProcessor2::beginStateTable()
{
    markGlyph = 0;
}

le_uint16 ContextualGlyphSubstitutionProcessor2::processStateEntry(LEGlyphStorage &glyphStorage, le_int32 &currGlyph,
    EntryTableIndex2 index, LEErrorCode &success)
{
    const ContextualGlyphStateEntry2 *entry = entryTable.getAlias(index, success);
    if (LE_FAILURE(success)) return 0;

    le_uint16 newState  = SWAPW(entry->newStateIndex);
    le_uint16 flags     = SWAPW(entry->flags);
    le_int16  markIndex = SWAPW(entry->markIndex);
    le_int16  currIndex = SWAPW(entry->currIndex);

    if (markIndex != -1) {
        le_uint32 offset        = SWAPL(perGlyphTable(markIndex, success));
        LEGlyphID mGlyph        = glyphStorage[markGlyph];
        TTGlyphID newGlyph      = lookup(offset, mGlyph, success);
        glyphStorage[markGlyph] = LE_SET_GLYPH(mGlyph, newGlyph);
    }

    if (currIndex != -1) {
        le_uint32 offset        = SWAPL(perGlyphTable(currIndex, success));
        LEGlyphID thisGlyph     = glyphStorage[currGlyph];
        TTGlyphID newGlyph      = lookup(offset, thisGlyph, success);
        glyphStorage[currGlyph] = LE_SET_GLYPH(thisGlyph, newGlyph);
    }

    if (flags & cgsSetMark) {
        markGlyph = currGlyph;
    }

    if (!(flags & cgsDontAdvance)) {
        currGlyph += dir;
    }

    return newState;
}

TTGlyphID ContextualGlyphSubstitutionProcessor2::lookup(le_uint32 offset, LEGlyphID gid, LEErrorCode &success)
{
    TTGlyphID newGlyph = 0xFFFF;

    LEReferenceTo<LookupTable> lookupTable(perGlyphTable, success, offset);
    if (LE_FAILURE(success)) return newGlyph;

    le_int16 format = SWAPW(lookupTable->format);

    switch (format) {
    case ltfSimpleArray: {
        LEReferenceTo<SimpleArrayLookupTable> simpleArrayLookupTable(lookupTable, success);
        LEReferenceToArrayOf<LookupValue> valueArray(simpleArrayLookupTable, success, &simpleArrayLookupTable->valueArray[0], LE_UNBOUNDED_ARRAY);
        newGlyph = SWAPW(valueArray((TTGlyphID) LE_GET_GLYPH(gid), success));
        break;
    }

    case ltfSegmentSingle: {
        LEReferenceTo<SegmentSingleLookupTable> segmentSingleLookupTable(lookupTable, success);
        const LookupSegment *segment = segmentSingleLookupTable->lookupSegment(segmentSingleLookupTable, segmentSingleLookupTable->segments, gid, success);
        if (segment != NULL) {
            newGlyph = SWAPW(segment->value);
        }
        break;
    }

    case ltfSegmentArray: {
        LEReferenceTo<SegmentArrayLookupTable> segmentArrayLookupTable(lookupTable, success);
        const LookupSegment *segment = segmentArrayLookupTable->lookupSegment(segmentArrayLookupTable, segmentArrayLookupTable->segments, gid, success);
        if (segment != NULL) {
            LookupValue firstGlyph = SWAPW(segment->firstGlyph);
            LookupValue offset     = SWAPW(segment->value);
            LEReferenceToArrayOf<LookupValue> glyphs(segmentArrayLookupTable, success, offset, LE_UNBOUNDED_ARRAY);
            newGlyph = SWAPW(glyphs(gid - firstGlyph, success));
        }
        break;
    }

    case ltfSingleTable: {
        LEReferenceTo<SingleTableLookupTable> singleTableLookupTable(lookupTable, success);
        const LookupSingle *lookupSingle = singleTableLookupTable->lookupSingle(singleTableLookupTable, singleTableLookupTable->entries, gid, success);
        if (lookupSingle != NULL) {
            newGlyph = SWAPW(lookupSingle->value);
        }
        break;
    }

    case ltfTrimmedArray: {
        LEReferenceTo<TrimmedArrayLookupTable> trimmedArrayLookupTable(lookupTable, success);
        TTGlyphID firstGlyph = SWAPW(trimmedArrayLookupTable->firstGlyph);
        TTGlyphID glyphCount = SWAPW(trimmedArrayLookupTable->glyphCount);
        TTGlyphID glyphCode  = (TTGlyphID) LE_GET_GLYPH(gid);
        if (firstGlyph <= glyphCode && glyphCode < firstGlyph + glyphCount) {
            LEReferenceToArrayOf<LookupValue> valueArray(trimmedArrayLookupTable, success, &trimmedArrayLookupTable->valueArray[0], glyphCount);
            newGlyph = SWAPW(valueArray(glyphCode - firstGlyph, success));
        }
    }

    default:
        break;
    }
    return newGlyph;
}

void ContextualGlyphSubstitutionProcessor2::endStateTable()
{
}

U_NAMESPACE_END
