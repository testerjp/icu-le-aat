/*
 * %W% %W%
 *
 * (C) Copyright IBM Corp. 1998 - 2013 - All Rights Reserved
 *
 */


#include "LETypes.h"
#include "LayoutTables.h"
#include "MorphTables.h"
#include "MorphStateTables.h"
#include "SubtableProcessor.h"
#include "IndicRearrangementProcessor.h"
#include "ContextualGlyphSubstProc.h"
#include "LigatureSubstProc.h"
#include "NonContextualGlyphSubstProc.h"
//#include "ContextualGlyphInsertionProcessor.h"
#include "LEGlyphStorage.h"
#include "LESwaps.h"

U_NAMESPACE_BEGIN

void
MorphTableHeader::process(const LETableReference &base, LEGlyphStorage &glyphStorage, LEErrorCode &success) const
{
    le_uint32 chainCount  = SWAPL(nChains);
    le_uint32 chainOffset = 0;

    for (le_uint32 chain = 0; LE_SUCCESS(success) && (chain < chainCount); chain += 1) {
        LEReferenceTo<ChainHeader> chainHeader(base, success, &chains[0], chainOffset);

        if (LE_FAILURE(success)) break;

        le_uint32 defaultFlags    = SWAPL(chainHeader->defaultFlags);
        le_uint32 chainLength     = SWAPL(chainHeader->chainLength);
        le_uint16 nFeatureEntries = SWAPW(chainHeader->nFeatureEntries);
        le_uint16 nSubtables      = SWAPW(chainHeader->nSubtables);
        chainOffset              += chainLength;

        le_uint32 subtableOffset = 0;

        for (le_uint16 subtable = 0; LE_SUCCESS(success) && (subtable < nSubtables); subtable += 1) {
            LEReferenceTo<MorphSubtableHeader> subtableHeader(chainHeader,success, &chainHeader->featureTable[nFeatureEntries], subtableOffset);

            if (LE_FAILURE(success)) break;

            le_uint16 length           = SWAPW(subtableHeader->length);
            le_uint16 coverage         = SWAPW(subtableHeader->coverage);
            le_uint32 subtableFeatures = SWAPL(subtableHeader->subtableFeatures);
            subtableOffset            += length;

            // should check coverage more carefully...
            if ((coverage & scfVertical) == 0 && (subtableFeatures & defaultFlags) != 0  && LE_SUCCESS(success)) {
                subtableHeader->process(subtableHeader, glyphStorage, success);
            }
        }
    }
}

void
MorphSubtableHeader::process(const LEReferenceTo<MorphSubtableHeader> &base, LEGlyphStorage &glyphStorage, LEErrorCode &success) const
{
    SubtableProcessor *processor = NULL;

    le_uint16 coverage = SWAPW(base->coverage);
    le_int32  dir      = 1;

    if (coverage & scfReverse)
        dir = -1;

    switch (coverage & scfTypeMask) {
    case mstIndicRearrangement: {
        LEReferenceTo<MorphStateTableHeader> morphStateTableHeader(base, success);
        LEReferenceTo<StateTableHeader>      header(morphStateTableHeader, success, &morphStateTableHeader->stHeader);
        processor = new IndicRearrangementProcessor(header, dir, success);
        break;
    }

    case mstContextualGlyphSubstitution: {
        LEReferenceTo<MorphStateTableHeader> morphStateTableHeader(base, success);
        LEReferenceTo<StateTableHeader>      header(morphStateTableHeader, success, &morphStateTableHeader->stHeader);
        processor = new ContextualGlyphSubstitutionProcessor(header, dir, success);
        break;
    }

    case mstLigatureSubstitution: {
        LEReferenceTo<MorphStateTableHeader> morphStateTableHeader(base, success);
        LEReferenceTo<StateTableHeader>      header(morphStateTableHeader, success, &morphStateTableHeader->stHeader);
        processor = new LigatureSubstitutionProcessor(header, dir, success);
        break;
    }

    case mstReservedUnused:
        break;

    case mstNonContextualGlyphSubstitution: {
        LEReferenceTo<NonContextualGlyphSubstitutionHeader> header(base, success);
        LEReferenceTo<LookupTable>                          lookupTable(header, success, &header->table);
        processor = new NonContextualGlyphSubstitutionProcessor(lookupTable, success);
        break;
    }

    /*
    case mstContextualGlyphInsertion:
        processor = new ContextualGlyphInsertionProcessor(this);
        break;
    */

    default:
        break;
    }

    if (processor != NULL) {
        processor->process(glyphStorage, success);
        delete processor;
    }
}

U_NAMESPACE_END
