/*
 * @(#)KernTable.cpp	1.1 04/10/13
 *
 * (C) Copyright IBM Corp. 2004-2013 - All Rights Reserved
 *
 */

#include "LETypes.h"
#include "LEGlyphStorage.h"
#include "LESwaps.h"
#include "KernStateTables.h"
#include "KernTable.h"
#include "SubtableProcessor.h"
#include "ContextualKerningProc.h"
#include "OpenTypeUtilities.h"

#define SWAP_KEY(p) (((le_uint32) SWAPW((p)->left) << 16) | SWAPW((p)->right))

U_NAMESPACE_BEGIN

/*
 * Process the glyph positions.  The positions array has two floats for each
 * glyph, plus a trailing pair to mark the end of the last glyph.
 */
static void kerningPairs(LEReferenceTo<KernSubtableKerningPairs> subtableHeader, LEGlyphStorage &storage, const LEFontInstance *font, LEErrorCode &success)
{
    le_uint16 nPairs        = SWAPW(subtableHeader->nPairs);
    le_uint16 searchRange   = SWAPW(subtableHeader->searchRange);;
    // le_uint16 entrySelector = SWAPW(subtableHeader->entrySelector);
    le_uint16 rangeShift    = SWAPW(subtableHeader->rangeShift);

    // pairs is an instance member, and table is on the stack.
    // set 'pairs' based on table.getAlias(). This will range check it.
    LEReferenceToArrayOf<KerningPair> pairs = LEReferenceToArrayOf<KerningPair>(subtableHeader, success, (const KerningPair*)subtableHeader.getAlias(), sizeof(KernSubtableKerningPairs), nPairs);
    if (LE_FAILURE(success)) return;

    le_uint32 key    = storage[0]; // no need to mask off high bits
    float     adjust = 0;
    le_int32  glyph;

    for (glyph = 1; LE_SUCCESS(success) && glyph < storage.getGlyphCount(); ++glyph) { // FIXME: is start index valid?
        key = key << 16 | (storage[glyph] & 0xffff);

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
        storage.adjustPosition(glyph, adjust, 0, success);
    }
    storage.adjustPosition(storage.getGlyphCount(), adjust, 0, success);
}

/*
 * This implementation isn't careful about the kern table flags, and
 * might invoke kerning when it is not supposed to.  That too I'm
 * leaving for a bug fix.
 *
 * TODO: respect header flags
 */
KernTable::KernTable(const LETableReference &base, LEErrorCode &success)
    : version(0), nTables(0), table(base)
{
    LEReferenceTo<KernTableHeader> kernTableHeader(table, success);

    if (LE_FAILURE(success))
        return;

    if (!kernTableHeader.getAlias()) // FIXME: LEReferenceTo(const LEFontInstance *font.. may return NULL
        return;

    version = SWAPW(kernTableHeader->version);

    if (!version)
        nTables = SWAPW(kernTableHeader->nTables);

    if (version == 1) {
        LEReferenceTo<KernTableHeader2> kernTableHeader(table, success);

        if (LE_FAILURE(success))
            return;

        nTables = SWAPL(kernTableHeader->nTables);
    }
}

void KernTable::process(LEGlyphStorage &glyphStorage, LEErrorCode &success)
{
    if (!version && nTables) {
        LEReferenceTo<KernSubtableHeader> subtableHeader(table, success, sizeof(KernTableHeader));

        if (LE_FAILURE(success))
            return;

        le_uint32 subtable;

        for (subtable = 0; LE_SUCCESS(success) && (subtable < nTables); subtable++) {
            le_uint16 length   = SWAPW(subtableHeader->length);
            le_uint16 coverage = SWAPW(subtableHeader->coverage);

            if (coverage & kcfHorizontal) {
                le_uint16 format = (coverage & kcfFormatMask) >> kcfFormatShift;
                switch (format) {
                case kfKerningPairs: {
                    LEReferenceTo<KernSubtableKerningPairs> subtableKerningPairs(subtableHeader, success, sizeof(KernSubtableHeader));

                    if (LE_FAILURE(success))
                        return;

                    kerningPairs(subtableKerningPairs, glyphStorage, subtableKerningPairs.getFont(), success);
                }
                }
            }
            subtableHeader.addOffset(length, success);
        }
    }

    if (version == 1 && nTables) {
        LEReferenceTo<KernSubtableHeader2> subtableHeader(table, success, sizeof(KernTableHeader2));

        if (LE_FAILURE(success))
            return;

        le_uint32 subtable;

        for (subtable = 0; LE_SUCCESS(success) && (subtable < nTables); subtable++) {
            le_uint32 length   = SWAPL(subtableHeader->length);
            le_uint16 coverage = SWAPW(subtableHeader->coverage);

            if (!(coverage & kcf2Vertical) && LE_SUCCESS(success)) {
                LE_TRACE_LOG("kern subtable %d", subtable);
                subtableHeader->process(subtableHeader, glyphStorage, success);
            }
            subtableHeader.addOffset(length, success);
        }
    }
}

void KernSubtableHeader2::process(const LEReferenceTo<KernSubtableHeader2> &base, LEGlyphStorage &glyphStorage, LEErrorCode &success) const
{
    le_uint16 coverage = SWAPW(base->coverage);
    le_uint16 format   = (coverage & kcf2FormatMask) >> kcf2FormatShift;

    switch (format) {
    case kfKerningPairs: {
        LEReferenceTo<KernSubtableKerningPairs> subtableKerningPairs(base, success, sizeof(KernSubtableHeader2));

        if (LE_FAILURE(success))
            return;

        kerningPairs(subtableKerningPairs, glyphStorage, base.getFont(), success);
    }

    case kfContextualKerning: {
        LEReferenceTo<KernStateTableHeader> kernStateTableHeader(base, success);
        LEReferenceTo<StateTableHeader>     header(kernStateTableHeader, success, &kernStateTableHeader->stHeader);

        if (LE_FAILURE(success))
            return;

        SubtableProcessor *processor = new ContextualKerningProcessor(header, 1, success);
        if (processor != NULL) {
            if (LE_SUCCESS(success))
                processor->process(glyphStorage, success);

            delete processor;
        }
    }
    }
}

U_NAMESPACE_END
