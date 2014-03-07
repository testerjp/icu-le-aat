/*
 * @(#)KernTable.cpp	1.1 04/10/13
 *
 * (C) Copyright IBM Corp. 2004-2013 - All Rights Reserved
 *
 */

#include "LETypes.h"
#include "KernTable.h"
#include "LEGlyphStorage.h"
#include "LESwaps.h"
#include "OpenTypeUtilities.h"

#define DEBUG 0

U_NAMESPACE_BEGIN

/*
 * This implementation has support for only one subtable, so if the font has
 * multiple subtables, only the first will be used.  If this turns out to
 * be a problem in practice we should add it.
 *
 * This also supports only version 0 of the kern table header, only
 * Apple supports the latter.
 *
 * This implementation isn't careful about the kern table flags, and
 * might invoke kerning when it is not supposed to.  That too I'm
 * leaving for a bug fix.
 *
 * TODO: support multiple subtables
 * TODO: respect header flags
 */
KernTable::KernTable(const LETableReference& base, LEErrorCode &success)
    : pairs(), fTable(base)
{
    if (LE_FAILURE(success)) return;

    if (fTable.isEmpty())
        return;

    LEReferenceTo<KernTableHeader> header(fTable, success);
    if (LE_FAILURE(success)) return;

    if (header.isEmpty())
        return;

    if (header->version != 0)
        return;

    if (SWAPW(header->nTables) <= 0)
        return;

    LEReferenceTo<SubtableHeader> subhead(header, success, KERN_TABLE_HEADER_SIZE);

    if (LE_FAILURE(success)) return;

    if (subhead.isEmpty())
        return;

    if (subhead->version != 0)
        return;

    coverage = SWAPW(subhead->coverage);

    if (!(coverage & COVERAGE_HORIZONTAL)) // only handle horizontal kerning
        return;

    LEReferenceTo<Subtable_0> table(subhead, success, KERN_SUBTABLE_HEADER_SIZE);

    if (LE_FAILURE(success)) return;

    if (table.isEmpty())
        return;

    nPairs        = SWAPW(table->nPairs);

#if 0   // some old fonts have bad values here...
    searchRange   = SWAPW(table->searchRange);
    entrySelector = SWAPW(table->entrySelector);
    rangeShift    = SWAPW(table->rangeShift);
#else
    entrySelector = OpenTypeUtilities::highBit(nPairs);
    searchRange   = (1 << entrySelector) * KERN_PAIRINFO_SIZE;
    rangeShift    = (nPairs * KERN_PAIRINFO_SIZE) - searchRange;
#endif

    if (LE_SUCCESS(success) && nPairs > 0) {
        // pairs is an instance member, and table is on the stack.
        // set 'pairs' based on table.getAlias(). This will range check it.
        pairs = LEReferenceToArrayOf<PairInfo>(fTable, success, (const PairInfo*)table.getAlias(), KERN_SUBTABLE_0_HEADER_SIZE, nPairs);
    }

#if 0
    fprintf(stderr, "coverage: %0.4x nPairs: %d pairs %p\n", coverage, nPairs, pairs.getAlias());
    fprintf(stderr, "  searchRange: %d entrySelector: %d rangeShift: %d\n", searchRange, entrySelector, rangeShift);
    fprintf(stderr, "[[ ignored font table entries: range %d selector %d shift %d ]]\n", SWAPW(table->searchRange), SWAPW(table->entrySelector), SWAPW(table->rangeShift));
#endif

#if DEBUG
    fprintf(stderr, "coverage: %0.4x nPairs: %d pairs 0x%x\n", coverage, nPairs, pairs);
    fprintf(stderr, "  searchRange: %d entrySelector: %d rangeShift: %d\n", searchRange, entrySelector, rangeShift);

    if (LE_SUCCESS(success)) {
        // dump part of the pair list
        char ids[256];

        for (int i = 256; --i >= 0;) {
            LEGlyphID id = font->mapCharToGlyph(i);

            if (id < 256) {
                ids[id] = (char)i;
            }
        }

        for (i = 0; i < nPairs; ++i) {
            const PairInfo& p = pairs[i, success];

            le_uint16 left = p->left;
            le_uint16 right = p->right;


            if (left < 256 && right < 256) {
                char c = ids[left];

                if (c > 0x20 && c < 0x7f) {
                    fprintf(stderr, "%c/", c & 0xff);
                } else {
                    printf(stderr, "%0.2x/", c & 0xff);
                }

                c = ids[right];
                if (c > 0x20 && c < 0x7f) {
                    fprintf(stderr, "%c ", c & 0xff);
                } else {
                    fprintf(stderr, "%0.2x ", c & 0xff);
                }
            }
        }
    }
#endif
}

/*
 * Process the glyph positions.  The positions array has two floats for each
 * glyph, plus a trailing pair to mark the end of the last glyph.
 */
void KernTable::process(LEGlyphStorage& storage, LEErrorCode &success)
{
    if (LE_FAILURE(success)) return;

    if (pairs.isEmpty())
        return;

    le_uint32 key    = storage[0]; // no need to mask off high bits
    float     adjust = 0;
    le_int32  glyph;

    for (glyph = 1; LE_SUCCESS(success) && glyph < storage.getGlyphCount(); ++glyph) { // FIXME: is start index valid?
        key = key << 16 | (storage[glyph] & 0xffff);

        // argh, to do a binary search, we need to have the pair list in sorted order
        // but it is not in sorted order on win32 platforms because of the endianness difference
        // so either I have to swap the element each time I examine it, or I have to swap
        // all the elements ahead of time and store them in the font

        const PairInfo *p = pairs.getAlias(0, success);

        LEReferenceTo<PairInfo> tpRef(pairs, success, rangeShift);
        if (LE_FAILURE(success)) return;

        const PairInfo *tp = tpRef.getAlias();

        if (key > SWAP_KEY(tp))
            p = tp;

        le_uint32 probe = searchRange;

        while (probe > KERN_PAIRINFO_SIZE && LE_SUCCESS(success)) {
            probe >>= 1;
            tpRef = LEReferenceTo<PairInfo>(pairs, success, p, probe);
            if (LE_FAILURE(success)) break;

            tp = tpRef.getAlias();
            le_uint32 tkey = SWAP_KEY(tp);

            if (tkey <= key && LE_SUCCESS(success)) {
                if (tkey == key) {
                    le_int16 value = SWAPW(tp->value);
                    adjust        += fTable.getFont()->xUnitsToPoints(value);
                    break;
                }
                p = tp;
            }
        }
        storage.adjustPosition(glyph, adjust, 0, success);
    }
    storage.adjustPosition(storage.getGlyphCount(), adjust, 0, success);
}

U_NAMESPACE_END
