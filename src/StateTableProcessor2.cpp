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
#include "LEGlyphStorage.h"
#include "LESwaps.h"
#include "LookupTables.h"

U_NAMESPACE_BEGIN

StateTableProcessor2::StateTableProcessor2()
{
}

StateTableProcessor2::StateTableProcessor2(const LEReferenceTo<MorphSubtableHeader2> &morphSubtableHeader, LEErrorCode &success)
    : SubtableProcessor2(morphSubtableHeader, success),
      dir(1), format(0), nClasses(0), classTableOffset(0), stateArrayOffset(0), entryTableOffset(0),
      classTable(), stateArray(),
      stateTableHeader(morphSubtableHeader, success),
      stHeader(stateTableHeader, success, (const StateTableHeader2*)&stateTableHeader->stHeader)
{
    if (LE_FAILURE(success)) return;

    nClasses         = SWAPL(stHeader->nClasses);
    classTableOffset = SWAPL(stHeader->classTableOffset);
    stateArrayOffset = SWAPL(stHeader->stateArrayOffset);
    entryTableOffset = SWAPL(stHeader->entryTableOffset);

    classTable       = LEReferenceTo<LookupTable>(stHeader, success, classTableOffset);
    format           = SWAPW(classTable->format);

    stateArray       = LEReferenceToArrayOf<EntryTableIndex2>(stHeader, success, stateArrayOffset, LE_UNBOUNDED_ARRAY);
}

StateTableProcessor2::~StateTableProcessor2()
{
}

void StateTableProcessor2::process(LEGlyphStorage &glyphStorage, LEErrorCode &success)
{
    // Start at state 0
    // XXX: How do we know when to start at state 1?
    le_uint16 currentState = 0;
    le_int32 glyphCount = glyphStorage.getGlyphCount();

    LE_STATE_PATIENCE_INIT();

    le_int32 currGlyph = 0;
    if ((coverage & scfReverse2) != 0) { // process glyphs in descending order
        currGlyph = glyphCount - 1;
        dir = -1;
    } else {
        dir = 1;
    }

    beginStateTable();

    switch (format) {
    case ltfSimpleArray: {
        LEReferenceTo<SimpleArrayLookupTable> simpleArrayLookupTable(classTable, success);
        if (LE_FAILURE(success)) break;

        while ((dir == 1 && currGlyph <= glyphCount) || (dir == -1 && currGlyph >= -1)) {
            if (LE_FAILURE(success)) break;

            if (LE_STATE_PATIENCE_DECR()) {
                LE_TRACE_LOG("patience exceeded - state table not moving");
                break; // patience exceeded.
            }

            LookupValue classCode = classCodeOOB;
            if (currGlyph == glyphCount || currGlyph == -1) {
                // XXX: How do we handle EOT vs. EOL?
                classCode = classCodeEOT;
            } else {
                LEGlyphID gid       = glyphStorage[currGlyph];
                TTGlyphID glyphCode = (TTGlyphID) LE_GET_GLYPH(gid);

                if (glyphCode == 0xFFFF) {
                    classCode = classCodeDEL;
                } else {
                    classCode = SWAPW(simpleArrayLookupTable->valueArray[gid]);
                }
            }

            EntryTableIndex2 entryTableIndex = SWAPW(stateArray(classCode + currentState * nClasses, success));
            LE_STATE_PATIENCE_CURR(le_int32, currGlyph);
            LE_TRACE_LOG("process state entry: classCode %d; nClasses %d; eti %d; current state %d; nClasses; current glyph %d", classCode, nClasses, entryTableIndex, currentState, glyphStorage[currGlyph]);
            currentState = processStateEntry(glyphStorage, currGlyph, entryTableIndex, success); // return a zero-based index instead of a byte offset
            LE_TRACE_LOG("new state %d; current glyph %d", currentState, glyphStorage[currGlyph]);
            LE_STATE_PATIENCE_INCR(currGlyph);
        }

        break;
    }

    case ltfSegmentSingle: {
        LEReferenceTo<SegmentSingleLookupTable> segmentSignleLookupTable(classTable, success);
        if (LE_FAILURE(success)) break;

        while ((dir == 1 && currGlyph <= glyphCount) || (dir == -1 && currGlyph >= -1)) {
            if (LE_FAILURE(success)) break;

            if (LE_STATE_PATIENCE_DECR()) {
                LE_TRACE_LOG("patience exceeded  - state table not moving");
                break; // patience exceeded.
            }

            LookupValue classCode = classCodeOOB;
            if (currGlyph == glyphCount || currGlyph == -1) {
                // XXX: How do we handle EOT vs. EOL?
                classCode = classCodeEOT;
            } else {
                LEGlyphID gid       = glyphStorage[currGlyph];
                TTGlyphID glyphCode = (TTGlyphID) LE_GET_GLYPH(gid);

                if (glyphCode == 0xFFFF) {
                    classCode = classCodeDEL;
                } else {
                    const LookupSegment *segment =
                        segmentSignleLookupTable->lookupSegment(segmentSignleLookupTable, segmentSignleLookupTable->segments, gid, success);
                    if (segment != NULL && LE_SUCCESS(success))
                        classCode = SWAPW(segment->value);
                }
            }

            EntryTableIndex2 entryTableIndex = SWAPW(stateArray(classCode + currentState * nClasses, success));
            LE_STATE_PATIENCE_CURR(le_int32, currGlyph);
            LE_TRACE_LOG("process state entry: classCode %d; nClasses %d; eti %d; current state %d; nClasses; current glyph %d", classCode, nClasses, entryTableIndex, currentState, glyphStorage[currGlyph]);
            currentState = processStateEntry(glyphStorage, currGlyph, entryTableIndex, success);
            LE_TRACE_LOG("new state %d; current glyph %d", currentState, glyphStorage[currGlyph]);
            LE_STATE_PATIENCE_INCR(currGlyph);
        }
        break;
    }

    case ltfSegmentArray: {
        LEReferenceTo<SegmentArrayLookupTable> segmentArrayLookupTable(classTable, success);
        if (LE_FAILURE(success)) break;

        while ((dir == 1 && currGlyph <= glyphCount) || (dir == -1 && currGlyph >= -1)) {
            if (LE_FAILURE(success)) break;

            if (LE_STATE_PATIENCE_DECR()) {
                LE_TRACE_LOG("patience exceeded  - state table not moving");
                break; // patience exceeded.
            }

            LookupValue classCode = classCodeOOB;
            if (currGlyph == glyphCount || currGlyph == -1) {
                // XXX: How do we handle EOT vs. EOL?
                classCode = classCodeEOT;
            } else {
                LEGlyphID gid       = glyphStorage[currGlyph];
                TTGlyphID glyphCode = (TTGlyphID) LE_GET_GLYPH(gid);

                if (glyphCode == 0xFFFF) {
                    classCode = classCodeDEL;
                } else {
                    const LookupSegment *segment =
                        segmentArrayLookupTable->lookupSegment(segmentArrayLookupTable, segmentArrayLookupTable->segments, gid, success);
                    if (segment != NULL && LE_SUCCESS(success)) {
                        LookupValue firstGlyph = SWAPW(segment->firstGlyph);
                        LookupValue offset     = SWAPW(segment->value);

                        LEReferenceToArrayOf<LookupValue> classes(segmentArrayLookupTable, success, offset, LE_UNBOUNDED_ARRAY);
                        classCode = SWAPW(classes(gid - firstGlyph, success));
                    }
                }
            }

            EntryTableIndex2 entryTableIndex = SWAPW(stateArray(classCode + currentState * nClasses, success));
            LE_STATE_PATIENCE_CURR(le_int32, currGlyph);
            LE_TRACE_LOG("process state entry: classCode %d; nClasses %d; eti %d; current state %d; nClasses; current glyph %d", classCode, nClasses, entryTableIndex, currentState, glyphStorage[currGlyph]);
            currentState = processStateEntry(glyphStorage, currGlyph, entryTableIndex, success);
            LE_TRACE_LOG("new state %d; current glyph %d", currentState, glyphStorage[currGlyph]);
            LE_STATE_PATIENCE_INCR(currGlyph);
        }
        break;
    }

    case ltfSingleTable: {
        LEReferenceTo<SingleTableLookupTable> singleTableLookupTable(classTable, success);
        while ((dir == 1 && currGlyph <= glyphCount) || (dir == -1 && currGlyph >= -1)) {
            if (LE_FAILURE(success)) break;
            if (LE_STATE_PATIENCE_DECR()) {
                LE_TRACE_LOG("patience exceeded - state table not moving");
                break; // patience exceeded.
            }
            LookupValue classCode = classCodeOOB;
            if (currGlyph == glyphCount || currGlyph == -1) {
                // XXX: How do we handle EOT vs. EOL?
                classCode = classCodeEOT;
            } else if (currGlyph > glyphCount) {
                // note if > glyphCount, we've run off the end (bad font)
                currGlyph = glyphCount;
                classCode = classCodeEOT;
            } else {
                LEGlyphID gid       = glyphStorage[currGlyph];
                TTGlyphID glyphCode = (TTGlyphID) LE_GET_GLYPH(gid);

                if (glyphCode == 0xFFFF) {
                    classCode = classCodeDEL;
                } else {
                    const LookupSingle *single = singleTableLookupTable->lookupSingle(singleTableLookupTable, singleTableLookupTable->entries, gid, success);
                    if (single != NULL)
                        classCode = SWAPW(single->value);
                }
            }

            EntryTableIndex2 entryTableIndex = SWAPW(stateArray(classCode + currentState * nClasses, success));
            LE_STATE_PATIENCE_CURR(le_int32, currGlyph);
            LE_TRACE_LOG("process state entry: current state %d; current glyph %d", currentState, glyphStorage[currGlyph]);
            currentState = processStateEntry(glyphStorage, currGlyph, entryTableIndex, success);
            LE_STATE_PATIENCE_INCR(currGlyph);
        }
        break;
    }

    case ltfTrimmedArray: {
        LEReferenceTo<TrimmedArrayLookupTable> trimmedArrayLookupTable(classTable, success);
        if (LE_FAILURE(success)) break;

        TTGlyphID firstGlyph = SWAPW(trimmedArrayLookupTable->firstGlyph);
        TTGlyphID lastGlyph  = firstGlyph + SWAPW(trimmedArrayLookupTable->glyphCount);

        while ((dir == 1 && currGlyph <= glyphCount) || (dir == -1 && currGlyph >= -1)) {
            if (LE_STATE_PATIENCE_DECR()) {
                LE_TRACE_LOG("patience exceeded - state table not moving");
                break; // patience exceeded.
            }

            LookupValue classCode = classCodeOOB;
            if (currGlyph == glyphCount || currGlyph == -1) {
                // XXX: How do we handle EOT vs. EOL?
                classCode = classCodeEOT;
            } else {
                TTGlyphID glyphCode = (TTGlyphID) LE_GET_GLYPH(glyphStorage[currGlyph]);
                if (glyphCode == 0xFFFF) {
                    classCode = classCodeDEL;
                } else if ((glyphCode >= firstGlyph) && (glyphCode < lastGlyph)) {
                    classCode = SWAPW(trimmedArrayLookupTable->valueArray[glyphCode - firstGlyph]);
                }
            }

            EntryTableIndex2 entryTableIndex = SWAPW(stateArray(classCode + currentState * nClasses, success));
            LE_STATE_PATIENCE_CURR(le_int32, currGlyph);
            currentState = processStateEntry(glyphStorage, currGlyph, entryTableIndex, success);
            LE_STATE_PATIENCE_INCR(currGlyph);
        }
        break;
    }

    default:
        break;
    }

    endStateTable();
}

U_NAMESPACE_END
