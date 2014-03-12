/*
 *
 * (C) Copyright IBM Corp.  and others 1998-2013 - All Rights Reserved
 *
 */

#include "LETypes.h"
#include "LEGlyphStorage.h"
#include "LESwaps.h"
#include "IndicRearrangementProcessor2.h"

U_NAMESPACE_BEGIN

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(IndicRearrangementProcessor2)

IndicRearrangementProcessor2::IndicRearrangementProcessor2(const LEReferenceTo<StateTableHeader2> &header, le_int32 dir, LEErrorCode &success)
    : StateTableProcessor2(header, dir, success),
      firstGlyph(0), lastGlyph(0),
      indicRearrangementSubtableHeader(header, success),
      entryTable(stateTableHeader, success, entryTableOffset, LE_UNBOUNDED_ARRAY)
{
}

IndicRearrangementProcessor2::~IndicRearrangementProcessor2()
{
}

void IndicRearrangementProcessor2::beginStateTable(LEGlyphStorage &, LEErrorCode &)
{
    firstGlyph = 0;
    lastGlyph  = 0;
}

le_uint16 IndicRearrangementProcessor2::processStateEntry(LEGlyphStorage &glyphStorage, le_int32 &currGlyph,
                                                          EntryTableIndex2 index, LEErrorCode &success)
{
    if (LE_FAILURE(success))
        return 0;

    const IndicRearrangementStateEntry2 *entry = entryTable.getAlias(index, success);

    if (LE_FAILURE(success))
        return 0;

    le_uint16 newState = SWAPW(entry->newStateIndex);
    le_uint16 flags    = SWAPW(entry->flags);

    if (flags & irfMarkFirst) {
        firstGlyph = currGlyph;
    }

    if (flags & irfMarkLast) {
        lastGlyph = currGlyph;
    }

    doRearrangementAction(glyphStorage, firstGlyph, lastGlyph, (IndicRearrangementVerb) (flags & irfVerbMask), success);

    if (!(flags & irfDontAdvance)) {
        currGlyph += dir;
    }

    return newState;
}

void IndicRearrangementProcessor2::endStateTable(LEGlyphStorage &, LEErrorCode &)
{
}

U_NAMESPACE_END
