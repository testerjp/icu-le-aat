/*
 *
 * (C) Copyright IBM Corp and Others. 1998-2013 - All Rights Reserved
 *
 */

#include "LETypes.h"
#include "LEGlyphStorage.h"
#include "LESwaps.h"
#include "LigatureSubstProc2.h"

U_NAMESPACE_BEGIN

#define ExtendedComplement(m) ((le_int32) (~((le_uint32) (m))))
#define SignBit(m) ((ExtendedComplement(m) >> 1) & (le_int32)(m))
#define SignExtend(v, m) (((v) & SignBit(m)) ? ((v) | ExtendedComplement(m)) : (v))

LigatureSubstitutionProcessor2::LigatureSubstitutionProcessor2(const LEReferenceTo<StateTableHeader2> &header, le_int32 dir, LEErrorCode &success)
    : StateTableProcessor2(header, dir, success),
      ligActionOffset(0), componentOffset(0), ligatureOffset(0), m(-1),
      ligatureSubstitutionHeader(header, success)
{
    if (LE_FAILURE(success))
        return;

    ligActionOffset = SWAPL(ligatureSubstitutionHeader->ligActionOffset);
    componentOffset = SWAPL(ligatureSubstitutionHeader->componentOffset);
    ligatureOffset  = SWAPL(ligatureSubstitutionHeader->ligatureOffset);
    entryTable      = LEReferenceToArrayOf<LigatureSubstitutionStateEntry2>(stateTableHeader, success, entryTableOffset, LE_UNBOUNDED_ARRAY);
}

LigatureSubstitutionProcessor2::~LigatureSubstitutionProcessor2()
{
}

void LigatureSubstitutionProcessor2::beginStateTable(LEGlyphStorage &, LEErrorCode &)
{
    m = -1;
}

le_uint16 LigatureSubstitutionProcessor2::processStateEntry(LEGlyphStorage &glyphStorage, le_int32 &currGlyph, EntryTableIndex2 index, LEErrorCode &success)
{
    if (LE_FAILURE(success))
        return 0;

    const LigatureSubstitutionStateEntry2 *entry = entryTable.getAlias(index, success);

    if (LE_FAILURE(success))
        return 0;

    le_uint16 newState       = SWAPW(entry->newStateIndex);
    le_uint16 flags          = SWAPW(entry->flags);
    le_uint16 ligActionIndex = SWAPW(entry->ligActionIndex);

    if (flags & lsfSetComponent) {
        if (nComponents <= m++) {
            currGlyph += dir;
            m          = -1;
            return 0;
        }
        componentStack[m] = currGlyph;
    }

    if (flags & lsfPerformAction) {
        LEReferenceToArrayOf<LigatureActionEntry> actionEntry(stateTableHeader, success, ligActionOffset, LE_UNBOUNDED_ARRAY);
        LEReferenceToArrayOf<le_uint16> componentTable(stateTableHeader, success, componentOffset, LE_UNBOUNDED_ARRAY);
        LEReferenceToArrayOf<TTGlyphID> ligatureTable(stateTableHeader, success, ligatureOffset, LE_UNBOUNDED_ARRAY);

        if (LE_FAILURE(success))
            return 0;

        le_int32 ligatureGlyphs[nComponents];
        le_int32 n = -1;

        le_int32 ligatureIndex = 0;
        LigatureActionEntry action;

        do {
            if (m == -1) {
                currGlyph += dir;
                m          = -1;
                return 0;
            }

            le_int32 componentGlyph = componentStack[m--];

            if (!(0 <= componentGlyph && componentGlyph < glyphStorage.getGlyphCount())) {
                currGlyph += dir;
                m          = -1;
                return 0;
            }

            action = lafLast;

            if (!actionEntry.getObject(ligActionIndex++, action, success))
                return 0;

            action = SWAPL(action);

            le_int32  offset = SignExtend(action & lafComponentOffsetMask, lafComponentOffsetMask);
            le_uint16 component;

            if (!componentTable.getObject(LE_GET_GLYPH(glyphStorage[componentGlyph]) + offset, component, success))
                return 0;

            component      = SWAPW(component);
            ligatureIndex += component;

            if (action & (lafLast | lafStore))  {
                TTGlyphID ligatureGlyph;
                if (!ligatureTable.getObject(ligatureIndex, ligatureGlyph, success))
                    return 0;

                ligatureGlyph                = SWAPW(ligatureGlyph);
                glyphStorage[componentGlyph] = LE_SET_GLYPH(glyphStorage[componentGlyph], ligatureGlyph);
                ligatureGlyphs[++n]          = componentGlyph;
                ligatureIndex                = 0;

            } else {
                glyphStorage[componentGlyph] = LE_SET_GLYPH(glyphStorage[componentGlyph], 0xFFFF);
            }
        } while (!(action & lafLast));

        while (0 <= n) {
            if (nComponents <= m++) {
                currGlyph += dir;
                m          = -1;
                return 0;
            }
            componentStack[m] = ligatureGlyphs[n--];
        }
    }

    if (!(flags & lsfDontAdvance))
        currGlyph += dir;

    if (newState == 0 || newState == 1)
         m = -1; // undocumented

    return newState;
}

void LigatureSubstitutionProcessor2::endStateTable(LEGlyphStorage &, LEErrorCode &)
{
}

U_NAMESPACE_END
