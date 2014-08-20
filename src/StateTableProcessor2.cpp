/*
 *
 * (C) Copyright IBM Corp.  and others 1998-2013 - All Rights Reserved
 *
 */

#include "LETypes.h"
#include "LEGlyphStorage.h"
#include "LESwaps.h"
#include "LookupTables.h"
#include "LookupTableProcessor.h"
#include "StateTableProcessor2.h"

U_NAMESPACE_BEGIN

StateTableProcessor2::StateTableProcessor2()
{
}

StateTableProcessor2::StateTableProcessor2(const LEReferenceTo<StateTableHeader2> &header, le_int32 direction, LEErrorCode &success)
    : dir(direction), nClasses(0), classTableOffset(0), stateArrayOffset(0), entryTableOffset(0),
      stateTableHeader(header), processor(0)
{
    if (LE_FAILURE(success))
        return;

    nClasses         = SWAPL(stateTableHeader->nClasses);
    classTableOffset = SWAPL(stateTableHeader->classTableOffset);
    stateArrayOffset = SWAPL(stateTableHeader->stateArrayOffset);
    entryTableOffset = SWAPL(stateTableHeader->entryTableOffset);

    classTable       = LEReferenceTo<LookupTable>(stateTableHeader, success, classTableOffset);

    if (LE_FAILURE(success))
        return;

    processor        = LookupTableProcessor::createInstance((LookupTableFormat)SWAPW(classTable->format), classTable, success);

    if (!processor)
        success = LE_INDEX_OUT_OF_BOUNDS_ERROR;

    stateArray       = LEReferenceToArrayOf<EntryTableIndex2>(stateTableHeader, success, stateArrayOffset, LE_UNBOUNDED_ARRAY);
}

StateTableProcessor2::~StateTableProcessor2()
{
    if (processor)
        delete processor;
}

void StateTableProcessor2::process(LEGlyphStorage &glyphStorage, LEErrorCode &success)
{
    if (LE_FAILURE(success) || !processor)
        return;

    le_uint16 currentState = 0; // XXX: How do we know when to start at state 1?
    le_int32  glyphCount   = glyphStorage.getGlyphCount();

    LE_STATE_PATIENCE_INIT();

    le_int32 currGlyph = 0;

    if (dir == -1) // process glyphs in descending order
        currGlyph = glyphCount - 1;
    else
        dir = 1;

    beginStateTable(glyphStorage, success);

    while (((dir == 1 && currGlyph <= (glyphCount = glyphStorage.getGlyphCount())) || (dir == -1 && -1 <= currGlyph)) && LE_SUCCESS(success)) {
        if (LE_STATE_PATIENCE_DECR())
            break;

        LookupValue classCode = classCodeOOB;

        if (currGlyph == glyphCount || currGlyph == -1) {
            classCode = classCodeEOT; // XXX: How do we handle EOT vs. EOL?
        } else {
            LEGlyphID gid       = glyphStorage[currGlyph];
            TTGlyphID glyphCode = (TTGlyphID)LE_GET_GLYPH(gid);

            if (glyphCode == 0xFFFF) {
                classCode = classCodeDEL;
            } else {
                LookupValue value;

                if (processor->lookup(gid, value, success))
                    classCode = value;

                if (LE_FAILURE(success))
                    break;
            }
        }

        EntryTableIndex2 entryTableIndex;

        if (stateArray.getObject(classCode + currentState * nClasses, entryTableIndex, success))
            entryTableIndex = SWAPW(entryTableIndex);
        else
            break;

        LE_STATE_PATIENCE_CURR(le_int32, currGlyph);
        currentState = processStateEntry(glyphStorage, currGlyph, entryTableIndex, success); // return a zero-based index instead of a byte offset
        LE_STATE_PATIENCE_INCR(currGlyph);
    }

    endStateTable(glyphStorage, success);
}

U_NAMESPACE_END
