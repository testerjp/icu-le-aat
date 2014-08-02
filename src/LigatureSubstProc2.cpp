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

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(LigatureSubstitutionProcessor2)

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

    LE_TRACE_LOG("ligature state entry: flags = %x; ligature action index = %d; glyph = %d; glyph index = %d", flags, ligActionIndex, 0 <= currGlyph && currGlyph < glyphStorage.getGlyphCount() ? glyphStorage[currGlyph] : -1, currGlyph);

    if (flags & lsfSetComponent) {
        if (nComponents <= m++) {
            LE_TRACE_LOG("stack overflow");
            currGlyph += dir;
            m          = -1;
            return 0;
        }
        componentStack[m] = currGlyph;
        LE_TRACE_LOG("push[%d]", m);
    }

    if (flags & lsfPerformAction) {
        LEReferenceTo<LigatureActionEntry> actionEntry(stateTableHeader, success, ligActionOffset + ligActionIndex * sizeof(LigatureActionEntry));
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
                LE_TRACE_LOG("stack underflow");
                currGlyph += dir;
                m          = -1;
                return 0;
            }

            le_int32 componentGlyph = componentStack[m--];

            LE_TRACE_LOG("pop[%d]; %d", m + 1, componentGlyph);

            if (!(0 <= componentGlyph && componentGlyph < glyphStorage.getGlyphCount())) {
                LE_TRACE_LOG("preposterous componentGlyph");
                currGlyph += dir;
                m          = -1;
                return 0;
            }

            action = SWAPL(*actionEntry.getAlias());

            le_int32  offset = SignExtend(action & lafComponentOffsetMask, lafComponentOffsetMask);
            le_uint16 component;

            if (!componentTable.getObject(LE_GET_GLYPH(glyphStorage[componentGlyph]) + offset, component, success))
                return 0;

            component      = SWAPW(component);
            ligatureIndex += component;

            LE_TRACE_LOG("action = %x; offset = %d; component index = %d; ", action, offset, ligatureIndex);

            if (action & (lafLast | lafStore))  {
                TTGlyphID ligatureGlyph;
                if (!ligatureTable.getObject(ligatureIndex, ligatureGlyph, success))
                    return 0;

                ligatureGlyph                = SWAPW(ligatureGlyph);
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
                return 0;
            }
            componentStack[m] = ligatureGlyphs[n--];
            LE_TRACE_LOG("push[%d]", m);
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
