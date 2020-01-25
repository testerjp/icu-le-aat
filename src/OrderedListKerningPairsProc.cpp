/*
 *
 * (C) Copyright IBM Corp. 2004-2013 - All Rights Reserved
 *
 */

#include "LETypes.h"
#include "LEGlyphStorage.h"
#include "LESwaps.h"
#include "OrderedListKerningPairsProc.h"

U_NAMESPACE_BEGIN

OrderedListKerningPairsProcessor::OrderedListKerningPairsProcessor(LEReferenceTo<OrderedListKerningPairsHeader> header, LEErrorCode &success)
    : nPairs(0)
    , searchRange(0)
    , entrySelector(0)
    , rangeShift(0)
    , orderedListKerningPairsHeader(header, success)
{
    if (LE_FAILURE(success))
        return;

    nPairs        = SWAPW(orderedListKerningPairsHeader->nPairs);
    searchRange   = SWAPW(orderedListKerningPairsHeader->searchRange);;
    entrySelector = SWAPW(orderedListKerningPairsHeader->entrySelector);
    rangeShift    = SWAPW(orderedListKerningPairsHeader->rangeShift);

    pairs         = LEReferenceToArrayOf<KerningPair>(orderedListKerningPairsHeader, success, &orderedListKerningPairsHeader->entries[0], nPairs);

    if (!orderedListKerningPairsHeader->validate(header))
        success = LE_INDEX_OUT_OF_BOUNDS_ERROR;
}

OrderedListKerningPairsProcessor::~OrderedListKerningPairsProcessor()
{
}

const KerningPair *
OrderedListKerningPairsProcessor::search(le_uint32 key, LEErrorCode &success) const
{
    le_uint16 unity = sizeof(KerningPair);
    le_uint16 probe = searchRange;
    le_uint16 extra = rangeShift;
    LEReferenceTo<KerningPair> entry(pairs, success);
    LEReferenceTo<KerningPair> trial(entry, success, extra);

    if (LE_FAILURE(success))
        return NULL;

    if (trial.getAlias()->key() <= key)
        entry = trial;

    while (probe > unity) {
        probe >>= 1;
        trial   = entry; // copy
        trial.addOffset(probe, success);

        if (LE_FAILURE(success))
            return NULL;

        if (trial.getAlias()->key() <= key)
            entry = trial;
    }

    if (entry.getAlias()->key() == key)
        return entry.getAlias();

    return NULL;
}

void OrderedListKerningPairsProcessor::process(LEGlyphStorage &glyphStorage, LEErrorCode &success)
{
    if (LE_FAILURE(success))
        return;

    if (!pairs.isValid())
        return;

    if (!glyphStorage.getGlyphCount())
        return;

    const LEFontInstance *font = orderedListKerningPairsHeader.getFont();

    if (!font)
        return;

    float     adjust = 0.0;
    le_uint32 key    = LE_GET_GLYPH(glyphStorage[0]);
    le_int32  glyph;

    for (glyph = 1; LE_SUCCESS(success) && glyph < glyphStorage.getGlyphCount(); ++glyph) {
        key = key << 16 | LE_GET_GLYPH(glyphStorage[glyph]);

        const KerningPair *pair = search(key, success);

        if (pair) {
            le_int16 value = SWAPW(pair->value);
            adjust        += font->xUnitsToPoints(value);
        }
        glyphStorage.adjustPosition(glyph, adjust, 0, success);
    }
    glyphStorage.adjustPosition(glyphStorage.getGlyphCount(), adjust, 0, success);
}

U_NAMESPACE_END
