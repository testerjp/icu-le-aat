#include "LETypes.h"
#include "LEGlyphStorage.h"
#include "LESwaps.h"
#include "OrderedListKerningPairsProc.h"

#define SWAP_KEY(p) (((le_uint32) SWAPW((p)->left) << 16) | SWAPW((p)->right))

U_NAMESPACE_BEGIN

OrderedListKerningPairsProcessor::OrderedListKerningPairsProcessor(LEReferenceTo<KernSubtableKerningPairs> header, LEErrorCode &success)
    : nPairs(0), searchRange(0), entrySelector(0), rangeShift(0),
      subtableHeader(header, success)
{
    if (LE_FAILURE(success))
        return;

    nPairs        = SWAPW(subtableHeader->nPairs);
    searchRange   = SWAPW(subtableHeader->searchRange);;
    entrySelector = SWAPW(subtableHeader->entrySelector);
    rangeShift    = SWAPW(subtableHeader->rangeShift);

    // pairs is an instance member, and table is on the stack.
    // set 'pairs' based on table.getAlias(). This will range check it.
    pairs = LEReferenceToArrayOf<KerningPair>(subtableHeader, success, (const KerningPair*)subtableHeader.getAlias(), sizeof(KernSubtableKerningPairs), nPairs);
}

OrderedListKerningPairsProcessor::~OrderedListKerningPairsProcessor()
{
}

/*
 * Process the glyph positions.  The positions array has two floats for each
 * glyph, plus a trailing pair to mark the end of the last glyph.
 */
void OrderedListKerningPairsProcessor::process(LEGlyphStorage &glyphStorage, LEErrorCode &success)
{
    const LEFontInstance *font = subtableHeader.getFont();

    le_uint32 key    = glyphStorage[0]; // no need to mask off high bits
    float     adjust = 0;
    le_int32  glyph;

    for (glyph = 1; LE_SUCCESS(success) && glyph < glyphStorage.getGlyphCount(); ++glyph) { // FIXME: is start index valid?
        key = key << 16 | (glyphStorage[glyph] & 0xffff);

        // argh, to do a binary search, we need to have the pair list in sorted order
        // but it is not in sorted order on win32 platforms because of the endianness difference
        // so either I have to swap the element each time I examine it, or I have to swap
        // all the elements ahead of time and store them in the font

        const KerningPair *p = pairs.getAlias(0, success);

        LEReferenceTo<KerningPair> tpRef(pairs, success, rangeShift);
        if (LE_FAILURE(success)) return;

        const KerningPair *tp = tpRef.getAlias();

        if (key > SWAP_KEY(tp))
            p = tp;

        le_uint32 probe = searchRange;

        while (sizeof(KerningPair) < probe) {
            probe >>= 1;
            tpRef = LEReferenceTo<KerningPair>(pairs, success, p, probe);
            if (LE_FAILURE(success)) break;

            tp = tpRef.getAlias();
            le_uint32 tkey = SWAP_KEY(tp);

            if (tkey <= key) {
                if (tkey == key) {
                    le_int16 value = SWAPW(tp->value);
                    adjust        += font->xUnitsToPoints(value);
                    break;
                }
                p = tp;
            }
        }
        glyphStorage.adjustPosition(glyph, adjust, 0, success);
    }
    glyphStorage.adjustPosition(glyphStorage.getGlyphCount(), adjust, 0, success);
}

U_NAMESPACE_END
