/*
 *
 * (C) Copyright IBM Corp.  and others 1998-2013 - All Rights Reserved
 *
 */

#include "LETypes.h"
#include "LEGlyphStorage.h"
#include "LESwaps.h"
#include "LookupTableProcessor.h"
#include "ContextualGlyphSubstProc2.h"

U_NAMESPACE_BEGIN

ContextualGlyphSubstitutionProcessor2::ContextualGlyphSubstitutionProcessor2(const LEReferenceTo<StateTableHeader2> &header, le_int32 dir, LEErrorCode &success)
    : StateTableProcessor2(header, dir, success),
      markGlyph(0),
      contextualGlyphSubstitutionHeader(header, success)
{
    if (LE_FAILURE(success))
        return;

    le_uint32 substitutionTableOffset = SWAPL(contextualGlyphSubstitutionHeader->substitutionTableOffset);
    entryTable        = LEReferenceToArrayOf<ContextualGlyphSubstitutionStateEntry2>(stateTableHeader, success, entryTableOffset, LE_UNBOUNDED_ARRAY);
    substitutionTable = LEReferenceToArrayOf<le_uint32>(stateTableHeader, success, substitutionTableOffset, LE_UNBOUNDED_ARRAY);
}

ContextualGlyphSubstitutionProcessor2::~ContextualGlyphSubstitutionProcessor2()
{
}

void ContextualGlyphSubstitutionProcessor2::beginStateTable(LEGlyphStorage &, LEErrorCode &)
{
    markGlyph = 0;
}

le_uint16 ContextualGlyphSubstitutionProcessor2::processStateEntry(LEGlyphStorage &glyphStorage, le_int32 &currGlyph,
    EntryTableIndex2 index, LEErrorCode &success)
{
    if (LE_FAILURE(success))
        return 0;

    const ContextualGlyphSubstitutionStateEntry2 *entry = entryTable.getAlias(index, success);

    if (LE_FAILURE(success))
        return 0;

    le_uint16 newState  = SWAPW(entry->newStateIndex);
    le_uint16 flags     = SWAPW(entry->flags);
    le_uint16 markIndex = SWAPW(entry->markIndex);
    le_uint16 currIndex = SWAPW(entry->currIndex);

    if (markIndex != 0xFFFF) {
        le_uint32 offset;

        if (substitutionTable.getObject(markIndex, offset, success)) {
            offset = SWAPL(offset);

            LEGlyphID mGlyph = glyphStorage[markGlyph];
            TTGlyphID newGlyph;

            if (lookup(offset, mGlyph, newGlyph, success))
                glyphStorage[markGlyph] = LE_SET_GLYPH(mGlyph, newGlyph);
        }
    }

    if (currIndex != 0xFFFF) {
        le_uint32 offset;

        if (substitutionTable.getObject(currIndex, offset, success)) {
            offset = SWAPL(offset);

            LEGlyphID thisGlyph = glyphStorage[currGlyph];
            TTGlyphID newGlyph;

            if (lookup(offset, thisGlyph, newGlyph, success))
                glyphStorage[currGlyph] = LE_SET_GLYPH(thisGlyph, newGlyph);
        }
    }

    if (flags & cgsSetMark) {
        markGlyph = currGlyph;
    }

    if (!(flags & cgsDontAdvance)) {
        currGlyph += dir;
    }

    return newState;
}

le_bool ContextualGlyphSubstitutionProcessor2::lookup(le_uint32 offset, LEGlyphID gid, TTGlyphID &newGlyph, LEErrorCode &success)
{
    LEReferenceTo<LookupTable> lookupTable(substitutionTable, success, offset);

    if (LE_FAILURE(success))
        return FALSE;

    le_bool               result    = FALSE;
    LookupTableProcessor *processor = LookupTableProcessor::createInstance((LookupTableFormat)SWAPW(lookupTable->format), lookupTable, success);

    if (processor && LE_SUCCESS(success)) {
        LookupValue value;

        if (processor->lookup(gid, value, success) && value) {
            newGlyph = value;
            result   = TRUE;
        }
    }

    if (processor)
        delete processor;

    return result;
}

void ContextualGlyphSubstitutionProcessor2::endStateTable(LEGlyphStorage &, LEErrorCode &)
{
}

U_NAMESPACE_END
