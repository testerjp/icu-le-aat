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

LigatureSubstitutionProcessor::LigatureSubstitutionProcessor(const LEReferenceTo<StateTableHeader> &header, le_int32 dir, LEErrorCode &success)
    : StateTableProcessor(header, dir, success)
    , ligatureActionTableOffset(0)
    , componentTableOffset(0)
    , ligatureTableOffset(0)
    , m(-1)
    , ligatureSubstitutionHeader(header, success)
{
    if (LE_FAILURE(success))
        return;

    ligatureActionTableOffset = SWAPW(ligatureSubstitutionHeader->ligatureActionTableOffset);
    componentTableOffset      = SWAPW(ligatureSubstitutionHeader->componentTableOffset);
    ligatureTableOffset       = SWAPW(ligatureSubstitutionHeader->ligatureTableOffset);

    entryTable                = LEReferenceToArrayOf<LigatureSubstitutionStateEntry>(stateTableHeader, success, entryTableOffset, LE_UNBOUNDED_ARRAY);
}

LigatureSubstitutionProcessor::~LigatureSubstitutionProcessor()
{
}

void
LigatureSubstitutionProcessor::beginStateTable(LEGlyphStorage &, LEErrorCode &)
{
    m = -1;
}

le_uint16
LigatureSubstitutionProcessor::processStateEntry(LEGlyphStorage &glyphStorage, le_int32 &currGlyph, EntryTableIndex index, LEErrorCode &success)
{
    if (LE_FAILURE(success))
        return stateArrayOffset;

    const LigatureSubstitutionStateEntry *entry = entryTable.getAlias(index, success);

    if (LE_FAILURE(success))
        return stateArrayOffset;

    le_uint16 newState = SWAPW(entry->newStateOffset);
    le_uint16 flags    = SWAPW(entry->flags);

    if (flags & lsfSetComponent) {
        if (nComponents <= m++) {
            currGlyph += dir;
            m          = -1;
            return stateArrayOffset;
        }
        componentStack[m] = currGlyph;
    }

    le_uint16 actionOffset = flags & lsfActionOffsetMask;

    if (actionOffset != 0) {
        LigatureActionEntry                       action;
        le_int16                                  actionIndex = 0;
        LEReferenceToArrayOf<LigatureActionEntry> actionEntry(stateTableHeader, success, actionOffset, LE_UNBOUNDED_ARRAY);

        LEReferenceToArrayOf<le_int16> componentTable(stateTableHeader, success, (size_t)0, LE_UNBOUNDED_ARRAY);

        if (LE_FAILURE(success))
            return stateArrayOffset;

        le_int32 ligatureGlyphs[nComponents];
        le_int32 n = -1;

        le_int32 ligatureOffset = 0;

        do {
            if (m == -1) {
                currGlyph += dir;
                m          = -1;
                return stateArrayOffset;
            }

            le_int32 componentGlyph = componentStack[m--];

            if (!(0 <= componentGlyph && componentGlyph < glyphStorage.getGlyphCount())) {
                currGlyph += dir;
                m          = -1;
                return stateArrayOffset;
            }

            action = lafLast;

            if (!actionEntry.getObject(actionIndex++, action, success))
                return stateArrayOffset;

            action = SWAPL(action);

            le_int32 offset = SignExtend(action & lafComponentOffsetMask, lafComponentOffsetMask);
            offset         += LE_GET_GLYPH(glyphStorage[componentGlyph]);

            le_int16 component;

            if (!componentTable.getObject(offset, component, success))
                return stateArrayOffset;

            component       = SWAPW(component);
            ligatureOffset += component;

            if (action & (lafLast | lafStore))  {
                LEReferenceTo<TTGlyphID> ligatureEntry(stateTableHeader, success, ligatureOffset);

                if (LE_FAILURE(success))
                    return stateArrayOffset;

                TTGlyphID ligatureGlyph      = SWAPW(*ligatureEntry.getAlias());
                glyphStorage[componentGlyph] = LE_SET_GLYPH(glyphStorage[componentGlyph], ligatureGlyph);
                ligatureGlyphs[++n]          = componentGlyph;
                ligatureOffset               = 0;
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

    le_int32 stateOffset = newState - stateArrayOffset;

    if (0 <= stateOffset && stateOffset < 2 * stateSize) // The state transits to start of text or line.
        m = -1; // undocumented

    return newState;
}

void
LigatureSubstitutionProcessor::endStateTable(LEGlyphStorage &, LEErrorCode &)
{
}

U_NAMESPACE_END
