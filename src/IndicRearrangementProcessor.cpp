/*
 *
 * (C) Copyright IBM Corp. 1998-2013 - All Rights Reserved
 *
 */

#include "LETypes.h"
#include "LEGlyphStorage.h"
#include "LESwaps.h"
#include "IndicRearrangementProcessor.h"

U_NAMESPACE_BEGIN

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(IndicRearrangementProcessor)

IndicRearrangementProcessor::IndicRearrangementProcessor(const LEReferenceTo<StateTableHeader> &header, le_int32 dir, LEErrorCode &success)
    : StateTableProcessor(header, dir, success),
      indicRearrangementSubtableHeader(header, success),
      entryTable(stateTableHeader, success, entryTableOffset, LE_UNBOUNDED_ARRAY)
{
}

IndicRearrangementProcessor::~IndicRearrangementProcessor()
{
}

void IndicRearrangementProcessor::beginStateTable(LEGlyphStorage &, LEErrorCode &)
{
    firstGlyph = 0;
    lastGlyph  = 0;
}

le_uint16 IndicRearrangementProcessor::processStateEntry(LEGlyphStorage &glyphStorage, le_int32 &currGlyph, EntryTableIndex index, LEErrorCode &success)
{
    if (LE_FAILURE(success))
        return stateArrayOffset;

    const IndicRearrangementStateEntry *entry = entryTable.getAlias(index, success);

    if (LE_FAILURE(success))
        return stateArrayOffset;

    le_uint16 newState = SWAPW(entry->newStateOffset);
    le_uint16 flags    = SWAPW(entry->flags);

    if (flags & irfMarkFirst) {
        firstGlyph = currGlyph;
    }

    if (flags & irfMarkLast) {
        lastGlyph = currGlyph;
    }

    doRearrangementAction(glyphStorage, firstGlyph, lastGlyph, (IndicRearrangementVerb) (flags & irfVerbMask), success);

    if (!(flags & irfDontAdvance))
        currGlyph += dir;

    return newState;
}

void IndicRearrangementProcessor::endStateTable(LEGlyphStorage &, LEErrorCode &)
{
}

U_NAMESPACE_END
