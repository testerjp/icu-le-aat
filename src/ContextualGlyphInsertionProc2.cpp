/*
 *
 * (C) Copyright IBM Corp.  and others 1998-2013 - All Rights Reserved
 *
 */

#include "LETypes.h"
#include "LEGlyphStorage.h"
#include "LESwaps.h"
#include "ContextualGlyphInsertionProc2.h"

U_NAMESPACE_BEGIN

ContextualGlyphInsertionProcessor2::ContextualGlyphInsertionProcessor2(const LEReferenceTo<StateTableHeader2> &header, le_int32 dir, LEErrorCode &success)
    : StateTableProcessor2(header, dir, success),
      contextualGlyphInsertionHeader(header, success)
{
    if (LE_FAILURE(success))
        return;

    le_uint32 insertionActionOffset = SWAPL(contextualGlyphInsertionHeader->insertionActionOffset);
    entryTable      = LEReferenceToArrayOf<ContextualGlyphInsertionStateEntry2>(stateTableHeader, success, entryTableOffset, LE_UNBOUNDED_ARRAY);
    insertionAction = LEReferenceToArrayOf<le_uint16>(stateTableHeader, success, insertionActionOffset, LE_UNBOUNDED_ARRAY);
}

ContextualGlyphInsertionProcessor2::~ContextualGlyphInsertionProcessor2()
{
}

void ContextualGlyphInsertionProcessor2::beginStateTable(LEGlyphStorage &, LEErrorCode &)
{
    markGlyph = 0;
}

void ContextualGlyphInsertionProcessor2::doInsertion(LEGlyphStorage &glyphStorage, le_int16 atGlyph, le_uint16 &index, le_uint16 count, le_bool /* isKashidaLike */, le_bool isBefore, LEErrorCode &success)
{
    if (!count)
        return;

    LEGlyphID *insertGlyphs = glyphStorage.insertGlyphs(atGlyph, count + 1, success);

    if (LE_FAILURE(success))
        return;

    for (le_uint32 glyph = 0; glyph < count + 1; glyph++)
        insertGlyphs[glyph] = 0; // undef

    // Note: Kashida vs Split Vowel seems to only affect selection and highlighting.
    // We note the flag, but do not layout different.
    // https://developer.apple.com/fonts/TTRefMan/RM06/Chap6mort.html

    le_int16 targetIndex = 0;

    if (isBefore) {
        insertGlyphs[count] = glyphStorage[atGlyph];
    } else {
        insertGlyphs[targetIndex++] = glyphStorage[atGlyph];
    }

    while(count-- && LE_SUCCESS(success)) {
        le_uint16 insertGlyph;

        if (insertionAction.getObject(index++, insertGlyph, success))
            insertGlyphs[targetIndex++] = SWAPW(insertGlyph);
    }
}

le_uint16 ContextualGlyphInsertionProcessor2::processStateEntry(LEGlyphStorage &glyphStorage, le_int32 &currGlyph, EntryTableIndex2 index, LEErrorCode &success)
{
    if (LE_FAILURE(success))
        return 0;

    const ContextualGlyphInsertionStateEntry2 *entry = entryTable.getAlias(index, success);

    if (LE_FAILURE(success))
        return 0;

    le_uint16 newState  = SWAPW(entry->newStateIndex);
    le_uint16 flags     = SWAPW(entry->flags);
    le_uint16 markIndex = SWAPW(entry->markedInsertIndex);
    le_uint16 currIndex = SWAPW(entry->currentInsertIndex);

    if (markIndex != 0xFFFF) {
        le_uint16 count         = (flags & cgiMarkedInsertCountMask) >> cgiMarkedInsertCountShift;
        le_bool   isKashidaLike = (flags & cgiMarkedIsKashidaLike);
        le_bool   isBefore      = (flags & cgiMarkInsertBefore);
        doInsertion(glyphStorage, markGlyph, markIndex, count, isKashidaLike, isBefore, success);
    }

    if (currIndex != 0xFFFF) {
        le_uint16 count         = (flags & cgiCurrentInsertCountMask) >> cgiCurrentInsertCountShift;
        le_bool   isKashidaLike = (flags & cgiCurrentIsKashidaLike);
        le_bool   isBefore      = (flags & cgiCurrentInsertBefore);
        doInsertion(glyphStorage, currGlyph, currIndex, count, isKashidaLike, isBefore, success);
    }

    if (flags & cgiSetMark) {
        markGlyph = currGlyph;
    }

    if (!(flags & cgiDontAdvance)) {
        currGlyph += dir;
    }

    return newState;
}

void ContextualGlyphInsertionProcessor2::endStateTable(LEGlyphStorage &glyphStorage, LEErrorCode &)
{
    glyphStorage.applyInsertions();
}

U_NAMESPACE_END
