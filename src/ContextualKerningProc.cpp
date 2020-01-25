#include "LETypes.h"
#include "LEGlyphStorage.h"
#include "LESwaps.h"
#include "ContextualKerningProc.h"

U_NAMESPACE_BEGIN

ContextualKerningProcessor::ContextualKerningProcessor(const LEReferenceTo<StateTableHeader> &header, le_int32 dir, le_bool crossStream, LEErrorCode &success)
    : StateTableProcessor(header, dir, success)
    , crossStream(crossStream)
    , sp(-1)
    , kerningValues(0)
    , contextualKerningHeader(header, success)
{
    if (LE_FAILURE(success))
        return;

    entryTable = LEReferenceToArrayOf<ContextualKerningStateEntry>(stateTableHeader, success, entryTableOffset, LE_UNBOUNDED_ARRAY);
}

ContextualKerningProcessor::~ContextualKerningProcessor()
{
}

void
ContextualKerningProcessor::beginStateTable(LEGlyphStorage &glyphStorage, LEErrorCode &success)
{
    if (LE_FAILURE(success))
        return;

    sp = -1;

    kerningValues = LE_NEW_ARRAY(le_int16, glyphStorage.getGlyphCount() * 2);

    le_int32 glyph;

    for (glyph = 0; glyph < glyphStorage.getGlyphCount() * 2; glyph++)
        kerningValues[glyph] = 0;
}

le_uint16
ContextualKerningProcessor::processStateEntry(LEGlyphStorage &glyphStorage, le_int32 &currGlyph, EntryTableIndex index, LEErrorCode &success)
{
    if (LE_FAILURE(success))
        return stateArrayOffset;

    const ContextualKerningStateEntry *entry = entryTable.getAlias(index, success);

    if (LE_FAILURE(success))
        return stateArrayOffset;

    le_uint16 newState = SWAPW(entry->newStateOffset);
    le_uint16 flags    = SWAPW(entry->flags);

    if (flags & ckfPush) {
        if (nComponents <= sp++) {
            currGlyph += dir;
            sp         = -1;
            return stateArrayOffset;
        }
        kerningStack[sp] = currGlyph;
    }

    le_uint16 valueOffset = flags & ckfValueOffsetMask;

    if (valueOffset) {
        le_int16                       action;
        le_int16                       actionIndex = 0;
        LEReferenceToArrayOf<le_int16> actionEntry(stateTableHeader, success, valueOffset, LE_UNBOUNDED_ARRAY);

        if (LE_FAILURE(success))
            return stateArrayOffset;

        do {
            if (sp == -1) {
                currGlyph += dir;
                sp         = -1;
                return stateArrayOffset;
            }

            le_int32 kerningGlyph = kerningStack[sp--];

            if (!(0 <= kerningGlyph && kerningGlyph < glyphStorage.getGlyphCount())) {
                currGlyph += dir;
                sp         = -1;
                return stateArrayOffset;
            }

            action = 1; // end of list

            if (!actionEntry.getObject(actionIndex++, action, success))
                break;

            action = SWAPW(action);

            if (!crossStream) {
                kerningValues[kerningGlyph * 2 + 0] = action & ~1;
            } else {
                kerningValues[kerningGlyph * 2 + 1] = action & ~1;
            }
        } while (!(action & 1) && LE_SUCCESS(success));
    }

    if (!(flags & ckfDontAdvance))
        currGlyph += dir;

    le_int32 stateOffset = newState - stateArrayOffset;

    if (0 <= stateOffset && stateOffset < 2 * stateSize) // The state transits to start of text or line.
        sp = -1; // undocumented

    return newState;
}

void
ContextualKerningProcessor::endStateTable(LEGlyphStorage &glyphStorage, LEErrorCode &success)
{
    if (!kerningValues)
        return;

    const LEFontInstance *font = stateTableHeader.getFont();

    if (font) {
        le_int32 glyph;
        float    adjustX = 0, adjustY = 0;

        for (glyph = 0; glyph < glyphStorage.getGlyphCount(); glyph++) {
            adjustX +=  font->xUnitsToPoints(kerningValues[glyph * 2 + 0]);
            adjustY += -font->xUnitsToPoints(kerningValues[glyph * 2 + 1]);

            if ((le_uint16)kerningValues[glyph * 2 + 1] == 0x8000) // reset cross-stream
                adjustY = 0;

            glyphStorage.adjustPosition(glyph, adjustX, adjustY, success);
        }
        glyphStorage.adjustPosition(glyph, adjustX, adjustY, success);
    }

    LE_DELETE_ARRAY(kerningValues);
}

U_NAMESPACE_END
