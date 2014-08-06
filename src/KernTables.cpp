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
#include "KernTables.h"
#include "SubtableProcessor.h"
#include "ContextualKerningProc.h"
#include "OrderedListKerningPairsProc.h"

U_NAMESPACE_BEGIN

KernTable::KernTable(const LETableReference &base, LEErrorCode &success)
    : version(0), nTables(0), table(base)
{
    if (LE_FAILURE(success))
        return;

    LEReferenceTo<KernTableHeader> kernTableHeader(table, success);

    if (LE_FAILURE(success))
        return;

    version = SWAPW(kernTableHeader->version);

    if (version == 0)
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
    if (LE_FAILURE(success))
        return;

    switch (version) {
    case 0: {
        le_uint32 subtableOffset = 0;

        for (le_uint32 subtable = 0; LE_SUCCESS(success) && (subtable < nTables); subtable++) {
            LEReferenceTo<KernSubtableHeader> subtableHeader(table, success, sizeof(KernTableHeader) + subtableOffset);

            if (LE_FAILURE(success))
                break;

            le_uint32 length   = SWAPL(subtableHeader->length);
            le_uint16 coverage = SWAPW(subtableHeader->coverage);
            subtableOffset    += length;

            if ((coverage & kcfHorizontal) && !(coverage & kcfCrossStream) && !(coverage & kcfMinimum))
                subtableHeader->process(subtableHeader, glyphStorage, success);
        }
        break;
    }

    case 1: {
        le_uint32 subtableOffset = 0;

        for (le_uint32 subtable = 0; LE_SUCCESS(success) && (subtable < nTables); subtable++) {
            LEReferenceTo<KernSubtableHeader2> subtableHeader(table, success, sizeof(KernTableHeader2) + subtableOffset);

            if (LE_FAILURE(success))
                break;

            le_uint32 length   = SWAPL(subtableHeader->length);
            le_uint16 coverage = SWAPW(subtableHeader->coverage);
            subtableOffset    += length;

            if (!(coverage & kcf2Vertical) && !(coverage & kcf2Variation))
                subtableHeader->process(subtableHeader, glyphStorage, success);
        }
        break;
    }
    }
}

void KernSubtableHeader::process(const LEReferenceTo<KernSubtableHeader> &base, LEGlyphStorage &glyphStorage, LEErrorCode &success) const
{
    SubtableProcessor *processor = NULL;

    le_uint16 coverage = SWAPW(base->coverage);
    le_uint16 format   = (coverage & kcfFormatMask) >> kcfFormatShift;

    switch (format) {
    case kfKerningPairs: {
        LEReferenceTo<OrderedListKerningPairsHeader> header(base, success, sizeof(KernSubtableHeader));
        processor = new OrderedListKerningPairsProcessor(header, success);
        break;
    }
    }

    if (processor != NULL) {
        processor->process(glyphStorage, success);
        delete processor;
    }
}

void KernSubtableHeader2::process(const LEReferenceTo<KernSubtableHeader2> &base, LEGlyphStorage &glyphStorage, LEErrorCode &success) const
{
    SubtableProcessor *processor = NULL;

    le_uint16 coverage = SWAPW(base->coverage);
    le_uint16 format   = (coverage & kcf2FormatMask) >> kcf2FormatShift;

    switch (format) {
    case kfKerningPairs: {
        if (coverage & kcf2CrossStream)
            break;

        LEReferenceTo<OrderedListKerningPairsHeader> header(base, success, sizeof(KernSubtableHeader2));
        processor = new OrderedListKerningPairsProcessor(header, success);
        break;
    }

    case kfContextualKerning: {
        LEReferenceTo<KernStateTableHeader> kernStateTableHeader(base, success);
        LEReferenceTo<StateTableHeader>     header(kernStateTableHeader, success, &kernStateTableHeader->stHeader);
        processor = new ContextualKerningProcessor(header, 1, coverage & kcf2CrossStream ? TRUE : FALSE, success);
        break;
    }
    }

    if (processor != NULL) {
        processor->process(glyphStorage, success);
        delete processor;
    }
}

U_NAMESPACE_END
