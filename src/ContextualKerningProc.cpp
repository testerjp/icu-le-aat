#include "LETypes.h"
#include "LEGlyphStorage.h"
#include "LESwaps.h"
#include "ContextualKerningProc.h"

U_NAMESPACE_BEGIN

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(ContextualKerningProcessor)

ContextualKerningProcessor::ContextualKerningProcessor(const LEReferenceTo<StateTableHeader> &header, le_int32 dir, LEErrorCode &success)
    : StateTableProcessor(header, dir, success),
      sp(-1), kerningValues(0),
      contextualKerningHeader(header, success)
{
    if (LE_FAILURE(success)) return;

    entryTable = LEReferenceToArrayOf<ContextualKerningStateEntry>(stateTableHeader, success, entryTableOffset, LE_UNBOUNDED_ARRAY);
}

ContextualKerningProcessor::~ContextualKerningProcessor()
{
}

void ContextualKerningProcessor::beginStateTable(LEGlyphStorage &glyphStorage, LEErrorCode & /* success */)
{
    sp = -1;

    kerningValues = LE_NEW_ARRAY(le_int16, glyphStorage.getGlyphCount());

    le_int32 glyph;

    for (glyph = 0; glyph < glyphStorage.getGlyphCount(); glyph++)
        kerningValues[glyph] = 0;
}

ByteOffset ContextualKerningProcessor::processStateEntry(LEGlyphStorage &glyphStorage, le_int32 &currGlyph, EntryTableIndex index)
{
    LEErrorCode success = LE_NO_ERROR;
    const ContextualKerningStateEntry *entry = entryTable.getAlias(index, success);

    ByteOffset newState = SWAPW(entry->newStateOffset);
    le_uint16  flags    = SWAPW(entry->flags);

    LE_TRACE_LOG("kerning state entry: flags = %x; glyph = %d; glyph index = %d; newState: %d", flags, 0 <= currGlyph && currGlyph < glyphStorage.getGlyphCount() ? glyphStorage[currGlyph] : -1, currGlyph, (newState - stateArrayOffset) / stateSize);

    if (flags & ckfPush) {
        if (nComponents <= sp++) {
            LE_TRACE_LOG("stack overflow");
            currGlyph += dir;
            sp         = -1;
            return stateArrayOffset;
        }
        kerningStack[sp] = currGlyph;
        LE_TRACE_LOG("push[%d]", sp);
    }

    ByteOffset valueOffset = flags & ckfValueOffsetMask;

    if (valueOffset) {
        LEReferenceTo<le_uint16> actionEntry(stateTableHeader, success, valueOffset);

        if (LE_FAILURE(success)) {
            currGlyph += dir;
            sp         = -1;
            return stateArrayOffset;
        }

        le_int16 action;

        do {
            if (sp == -1) {
                LE_TRACE_LOG("stack underflow");
                currGlyph += dir;
                sp         = -1;
                return stateArrayOffset;
            }

            le_int32 kerningGlyph = kerningStack[sp--];

            LE_TRACE_LOG("pop[%d] %d", sp + 1, kerningGlyph);

            if (!(0 <= kerningGlyph && kerningGlyph < glyphStorage.getGlyphCount())) {
                LE_TRACE_LOG("preposterous componentGlyph");
                currGlyph += dir;
                sp         = -1;
                return stateArrayOffset;
            }

            action                      = SWAPW(*actionEntry.getAlias());
            kerningValues[kerningGlyph] = action & ~1;

            if (!(action & 1))
                actionEntry.addObject(success);
        } while (!(action & 1) && LE_SUCCESS(success));
    }

    if (!(flags & ckfDontAdvance))
        currGlyph += dir;

    le_int32 stateOffset = newState - stateArrayOffset;

    if (0 <= stateOffset && stateOffset < 2 * stateSize) // The state transits to start of text or line.
        sp = -1; // undocumented

    return newState;
}

void ContextualKerningProcessor::endStateTable(LEGlyphStorage &glyphStorage, LEErrorCode &success)
{
    if (!kerningValues)
        return;

    const LEFontInstance *font = stateTableHeader.getFont();

    if (font) {
        le_int32 glyph;
        float    adjust = 0.0;

        for (glyph = 0; glyph < glyphStorage.getGlyphCount(); glyph++) {
            if (kerningValues[glyph])
                adjust += font->xUnitsToPoints(kerningValues[glyph]);

            glyphStorage.adjustPosition(glyph, adjust, 0, success);
        }
        glyphStorage.adjustPosition(glyph, adjust, 0, success);
    }

    LE_DELETE_ARRAY(kerningValues);
}

U_NAMESPACE_END
