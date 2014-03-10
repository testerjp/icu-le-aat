/*
 *
 * (C) Copyright IBM Corp. 1998-2013 - All Rights Reserved
 *
 */

#include "LETypes.h"
#include "LEGlyphStorage.h"
#include "LESwaps.h"
#include "StateTableProcessor.h"

U_NAMESPACE_BEGIN

StateTableProcessor::StateTableProcessor()
{
}

StateTableProcessor::StateTableProcessor(const LEReferenceTo<StateTableHeader> &header, LEErrorCode &success)
    : stateTableHeader(header)
{
    if (LE_FAILURE(success)) return;

    stateSize        = SWAPW(stateTableHeader->stateSize);
    classTableOffset = SWAPW(stateTableHeader->classTableOffset);
    stateArrayOffset = SWAPW(stateTableHeader->stateArrayOffset);
    entryTableOffset = SWAPW(stateTableHeader->entryTableOffset);

    classTable       = LEReferenceTo<ClassTable>(stateTableHeader, success, classTableOffset);

    if (LE_FAILURE(success)) return;

    firstGlyph       = SWAPW(classTable->firstGlyph);
    lastGlyph        = firstGlyph + SWAPW(classTable->nGlyphs);
}

StateTableProcessor::~StateTableProcessor()
{
}

void StateTableProcessor::process(LEGlyphStorage &glyphStorage, LEErrorCode &success)
{
    LE_STATE_PATIENCE_INIT();

    // Start at state 0
    // XXX: How do we know when to start at state 1?
    ByteOffset currentState = stateArrayOffset;

    // XXX: reverse?
    le_int32 currGlyph = 0;
    le_int32 glyphCount = glyphStorage.getGlyphCount();

    beginStateTable();

    while (currGlyph <= glyphCount) {
        if (LE_STATE_PATIENCE_DECR()) break; // patience exceeded.
        ClassCode classCode = classCodeOOB;
        if (currGlyph == glyphCount) {
            // XXX: How do we handle EOT vs. EOL?
            classCode = classCodeEOT;
        } else {
            TTGlyphID glyphCode = (TTGlyphID) LE_GET_GLYPH(glyphStorage[currGlyph]);
            if (glyphCode == 0xFFFF) {
                classCode = classCodeDEL;
            } else if ((glyphCode >= firstGlyph) && (glyphCode < lastGlyph)) {
                classCode = classTable->classArray[glyphCode - firstGlyph];
            }
        }

        LEReferenceToArrayOf<EntryTableIndex> stateArray(stateTableHeader, success, currentState, LE_UNBOUNDED_ARRAY);
        EntryTableIndex entryTableIndex = stateArray.getObject((le_uint8)classCode, success);
        LE_STATE_PATIENCE_CURR(le_int32, currGlyph);
        currentState = processStateEntry(glyphStorage, currGlyph, entryTableIndex);
        LE_STATE_PATIENCE_INCR(currGlyph);
    }

    endStateTable();
}

U_NAMESPACE_END
