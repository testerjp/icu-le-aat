/*
 *
 * (C) Copyright IBM Corp and Others. 1998-2013 - All Rights Reserved
 *
 */

#include "LETypes.h"
#include "MorphTables.h"
#include "StateTables.h"
#include "MorphStateTables.h"
#include "SubtableProcessor2.h"
#include "StateTableProcessor2.h"
#include "LigatureSubstProc2.h"
#include "LEGlyphStorage.h"
#include "LESwaps.h"

U_NAMESPACE_BEGIN

#define ExtendedComplement(m) ((le_int32) (~((le_uint32) (m))))
#define SignBit(m) ((ExtendedComplement(m) >> 1) & (le_int32)(m))
#define SignExtend(v, m) (((v) & SignBit(m)) ? ((v) | ExtendedComplement(m)) : (v))

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(LigatureSubstitutionProcessor2)

LigatureSubstitutionProcessor2::LigatureSubstitutionProcessor2(const LEReferenceTo<MorphSubtableHeader2> &morphSubtableHeader, LEErrorCode &success)
    : StateTableProcessor2(morphSubtableHeader, success),
      ligActionOffset(0), componentOffset(0), ligatureOffset(0), entryTable(),
      ligatureSubstitutionHeader(morphSubtableHeader, success)
{
    if (LE_FAILURE(success)) return;

    ligActionOffset = SWAPL(ligatureSubstitutionHeader->ligActionOffset);
    componentOffset = SWAPL(ligatureSubstitutionHeader->componentOffset);
    ligatureOffset  = SWAPL(ligatureSubstitutionHeader->ligatureOffset);

    entryTable = LEReferenceToArrayOf<LigatureSubstitutionStateEntry2>(stHeader, success, entryTableOffset, LE_UNBOUNDED_ARRAY);
}

LigatureSubstitutionProcessor2::~LigatureSubstitutionProcessor2()
{
}

void LigatureSubstitutionProcessor2::beginStateTable()
{
    m = -1;
}

le_uint16 LigatureSubstitutionProcessor2::processStateEntry(LEGlyphStorage &glyphStorage, le_int32 &currGlyph, EntryTableIndex2 index, LEErrorCode &success)
{
    const LigatureSubstitutionStateEntry2 *entry = entryTable.getAlias(index, success);
    if (LE_FAILURE(success)) return 0;

    le_uint16 nextStateIndex = SWAPW(entry->nextStateIndex);
    le_uint16 flags          = SWAPW(entry->entryFlags);
    le_uint16 ligActionIndex = SWAPW(entry->ligActionIndex);

    LE_TRACE_LOG("ligature state entry: flags = %x; ligature action index = %d; glyph = %d; glyph index = %d; ", flags, ligActionIndex, glyphStorage[currGlyph], currGlyph);

    if (flags & lsfSetComponent) {
        if (nComponents <= m++) {
            LE_TRACE_LOG("stack overflow");
            currGlyph += dir;
            m          = -1;
            return 0; // force start of text state
        }
        componentStack[m] = currGlyph;
        LE_TRACE_LOG("push[%d]", m);
    }

    if (flags & lsfPerformAction) {
        LEReferenceTo<LigatureActionEntry> actionEntry(stHeader, success, ligActionOffset + ligActionIndex * sizeof(LigatureActionEntry));
        LEReferenceToArrayOf<le_uint16> componentTable(stHeader, success, componentOffset, LE_UNBOUNDED_ARRAY);
        LEReferenceToArrayOf<TTGlyphID> ligatureTable(stHeader, success, ligatureOffset, LE_UNBOUNDED_ARRAY);

        if (LE_FAILURE(success)) { // FIXME
            currGlyph += dir;
            m          = -1;
            return 0; // force start of text state
        }

        le_int32 ligatureGlyphs[nComponents];
        le_int32 n = -1;

        le_int32 ligatureIndex = 0;
        LigatureActionEntry action;

        do {
            if (m == -1) {
                LE_TRACE_LOG("stack underflow");
                currGlyph += dir;
                return 0; // force start of text state
            }

            le_int32 componentGlyph = componentStack[m--];

            LE_TRACE_LOG("pop[%d]; %d", m + 1, componentGlyph);

            if (glyphStorage.getGlyphCount() < componentGlyph) { // FIXME <= ?
                LE_TRACE_LOG("preposterous componentGlyph");
                currGlyph += dir;
                m          = -1;
                return 0; // force start of text state
            }

            action = SWAPL(*actionEntry.getAlias());

            le_int32 offset = SignExtend(action & lafComponentOffsetMask, lafComponentOffsetMask);
            ligatureIndex  += SWAPW(componentTable(LE_GET_GLYPH(glyphStorage[componentGlyph]) + offset, success));

            LE_TRACE_LOG("action = %x; offset = %d; component index = %d; ", action, offset, ligatureIndex);

            if (action & (lafLast | lafStore))  {
                TTGlyphID ligatureGlyph      = SWAPW(ligatureTable(ligatureIndex, success)); // FIXME: check index and gid boundary
                glyphStorage[componentGlyph] = LE_SET_GLYPH(glyphStorage[componentGlyph], ligatureGlyph);
                ligatureGlyphs[++n]          = componentGlyph;
                ligatureIndex                = 0;

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

    if (nextStateIndex == 0 || nextStateIndex == 1)
         m = -1; // undocumented

    return nextStateIndex;
}

void LigatureSubstitutionProcessor2::endStateTable()
{
}

U_NAMESPACE_END
