/*
 *
 * (C) Copyright IBM Corp. 1998-2013 - All Rights Reserved
 *
 */

#include "LETypes.h"
#include "LEGlyphStorage.h"
#include "LESwaps.h"
#include "LigatureSubstProc.h"

U_NAMESPACE_BEGIN

#define ExtendedComplement(m) ((le_int32) (~((le_uint32) (m))))
#define SignBit(m) ((ExtendedComplement(m) >> 1) & (le_int32)(m))
#define SignExtend(v,m) (((v) & SignBit(m))? ((v) | ExtendedComplement(m)): (v))

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(LigatureSubstitutionProcessor)

LigatureSubstitutionProcessor::LigatureSubstitutionProcessor(const LEReferenceTo<StateTableHeader> &header, le_int32 dir, LEErrorCode &success)
    : StateTableProcessor(header, dir, success),
      ligatureActionTableOffset(0), componentTableOffset(0), ligatureTableOffset(0), m(-1),
      ligatureSubstitutionHeader(header, success)
{
    if (LE_FAILURE(success)) return;

    ligatureActionTableOffset = SWAPW(ligatureSubstitutionHeader->ligatureActionTableOffset);
    componentTableOffset      = SWAPW(ligatureSubstitutionHeader->componentTableOffset);
    ligatureTableOffset       = SWAPW(ligatureSubstitutionHeader->ligatureTableOffset);

    entryTable                = LEReferenceToArrayOf<LigatureSubstitutionStateEntry>(stateTableHeader, success, entryTableOffset, LE_UNBOUNDED_ARRAY);
}

LigatureSubstitutionProcessor::~LigatureSubstitutionProcessor()
{
}

void LigatureSubstitutionProcessor::beginStateTable()
{
    m = -1;
}

ByteOffset LigatureSubstitutionProcessor::processStateEntry(LEGlyphStorage &glyphStorage, le_int32 &currGlyph, EntryTableIndex index)
{
    LEErrorCode success = LE_NO_ERROR;
    const LigatureSubstitutionStateEntry *entry = entryTable.getAlias(index, success);

    ByteOffset newState = SWAPW(entry->newStateOffset);
    le_int16   flags    = SWAPW(entry->flags);

    LE_TRACE_LOG("ligature state entry: flags = %x; glyph = %d; glyph index = %d; newState: %d", flags, 0 <= currGlyph && currGlyph < glyphStorage.getGlyphCount() ? glyphStorage[currGlyph] : -1, currGlyph, (newState - stateArrayOffset) / stateSize);

    if (flags & lsfSetComponent) {
        if (nComponents <= m++) {
            LE_TRACE_LOG("stack overflow");
            currGlyph += dir;
            m          = -1;
            return stateArrayOffset; // force start of text state
        }
        componentStack[m] = currGlyph;
        LE_TRACE_LOG("push[%d]", m);
    }

    ByteOffset actionOffset = flags & lsfActionOffsetMask;

    if (actionOffset != 0) {
        LEReferenceTo<LigatureActionEntry> actionEntry(stateTableHeader, success, actionOffset);
        LEReferenceToArrayOf<le_int16> componentTable(stateTableHeader, success, (size_t)0, LE_UNBOUNDED_ARRAY);

        if (LE_FAILURE(success)) { // FIXME
            currGlyph += dir;
            m          = -1;
            return stateArrayOffset; // force start of text state
        }

        le_int32 ligatureGlyphs[nComponents];
        le_int32 n = -1;

        le_int32 ligatureOffset = 0;
        LigatureActionEntry action;

        do {
            if (m == -1) {
                LE_TRACE_LOG("stack underflow");
                currGlyph += dir;
                return stateArrayOffset; // force start of text state
            }

            le_int32 componentGlyph = componentStack[m--];

            LE_TRACE_LOG("pop[%d] %d", m + 1, componentGlyph);

            if (glyphStorage.getGlyphCount() < componentGlyph) { // FIXME <= ?
                LE_TRACE_LOG("preposterous componentGlyph");
                currGlyph += dir;
                m          = -1;
                return stateArrayOffset; // force start of text state
            }

            action = SWAPL(*actionEntry.getAlias());

            le_int32 offset = SignExtend(action & lafComponentOffsetMask, lafComponentOffsetMask);
            ligatureOffset += SWAPW(componentTable(LE_GET_GLYPH(glyphStorage[componentGlyph]) + offset, success));

            LE_TRACE_LOG("action =  %x; signed offset = %d, add %d; ligatureOffset = %d", action, offset, LE_GET_GLYPH(glyphStorage[componentGlyph]) + offset, ligatureOffset);

            if (action & (lafLast | lafStore))  {
                LEReferenceTo<TTGlyphID> ligatureEntry(stateTableHeader, success, ligatureOffset);
                TTGlyphID ligatureGlyph      = SWAPW(*ligatureEntry.getAlias());
                glyphStorage[componentGlyph] = LE_SET_GLYPH(glyphStorage[componentGlyph], ligatureGlyph);
                ligatureGlyphs[++n]          = componentGlyph;
                ligatureOffset               = 0;

                LE_TRACE_LOG("replace with %d", ligatureGlyph);
            } else {
                glyphStorage[componentGlyph] = LE_SET_GLYPH(glyphStorage[componentGlyph], 0xFFFF);

                LE_TRACE_LOG("replace with deleted");
            }

            if (!(action & lafLast))
                actionEntry.addObject(success);
        } while (!(action & lafLast));

        while (0 <= n) {
            if (nComponents <= m++) {
                LE_TRACE_LOG("stack overflow");
                currGlyph += dir;
                m          = -1;
                return 0; // force start of text state
            }
            componentStack[m] = ligatureGlyphs[n--];
            LE_TRACE_LOG("push[%d]", m);
        }
    }

    if (!(flags & lsfDontAdvance))
        currGlyph += dir;

    le_int32 stateOffset = newState - stateArrayOffset;

    if (0 <= stateOffset && stateOffset < 2 * stateSize) // The state transits to start of text or line.
        m = -1; // undocumented

    return newState;
}

void LigatureSubstitutionProcessor::endStateTable()
{
}

U_NAMESPACE_END
