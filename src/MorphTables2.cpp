/*
 * (C) Copyright IBM Corp. and others 1998 - 2013 - All Rights Reserved
 *
 */

#include "LETypes.h"
#include "LayoutTables.h"
#include "MorphTables.h"
#include "MorphStateTables.h"
#include "SubtableProcessor.h"
#include "IndicRearrangementProcessor2.h"
#include "ContextualGlyphSubstProc2.h"
#include "LigatureSubstProc2.h"
#include "NonContextualGlyphSubstProc2.h"
#include "ContextualGlyphInsertionProc2.h"
#include "LEGlyphStorage.h"
#include "LESwaps.h"

U_NAMESPACE_BEGIN

void
MorphTableHeader2::process(const LEReferenceTo<MorphTableHeader2> &base, LEGlyphStorage &glyphStorage, le_int32 /* typoFlags */, LEErrorCode &success) const
{
    if (LE_FAILURE(success))
        return;

    le_uint32 chainCount  = SWAPL(nChains);
    le_uint32 chainOffset = 0;

    for (le_uint32 chain = 0; LE_SUCCESS(success) && (chain < chainCount); chain++) {
        LEReferenceTo<ChainHeader2> chainHeader(base, success, &chains[0], chainOffset);

        if (LE_FAILURE(success)) break;

        le_uint32 flag            = SWAPL(chainHeader->defaultFlags);
        le_uint32 chainLength     = SWAPL(chainHeader->chainLength);
        le_uint32 nFeatureEntries = SWAPL(chainHeader->nFeatureEntries);
        le_uint32 nSubtables      = SWAPL(chainHeader->nSubtables);
        chainOffset              += chainLength;

        le_uint32 subtableOffset = 0;

        for (le_uint32 subtable = 0;  LE_SUCCESS(success) && subtable < nSubtables; subtable++) {
            LEReferenceTo<MorphSubtableHeader2> subtableHeader(chainHeader, success, &chainHeader->featureTable[nFeatureEntries], subtableOffset);

            if (LE_FAILURE(success)) break;

            le_uint32 length           = SWAPL(subtableHeader->length);
            le_uint32 coverage         = SWAPL(subtableHeader->coverage);
            le_uint32 subtableFeatures = SWAPL(subtableHeader->subtableFeatures);
            subtableOffset            += length;

            // should check coverage more carefully...
            if (((coverage & scf2IgnoreVt) || !(coverage & scf2Vertical)) && (subtableFeatures & flag) != 0) {
                subtableHeader->process(subtableHeader, glyphStorage, success);
            }
        }
    }
}

void
MorphSubtableHeader2::process(const LEReferenceTo<MorphSubtableHeader2> &base, LEGlyphStorage &glyphStorage, LEErrorCode &success) const
{
    SubtableProcessor *processor = NULL;

    le_uint32 coverage = SWAPL(base->coverage);
    le_int32  dir      = 1;

    if (coverage & scf2Reverse)
        dir = -1;

    switch (coverage & scf2TypeMask) {
    case mstIndicRearrangement: {
        LEReferenceTo<MorphStateTableHeader2> morphStateTableHeader(base, success);
        LEReferenceTo<StateTableHeader2>      header(morphStateTableHeader, success, &morphStateTableHeader->stHeader);
        processor = new IndicRearrangementProcessor2(header, dir, success);
        break;
    }

    case mstContextualGlyphSubstitution: {
        LEReferenceTo<MorphStateTableHeader2> morphStateTableHeader(base, success);
        LEReferenceTo<StateTableHeader2>      header(morphStateTableHeader, success, &morphStateTableHeader->stHeader);
        processor = new ContextualGlyphSubstitutionProcessor2(header, dir, success);
        break;
    }

    case mstLigatureSubstitution: {
        LEReferenceTo<MorphStateTableHeader2> morphStateTableHeader(base, success);
        LEReferenceTo<StateTableHeader2>      header(morphStateTableHeader, success, &morphStateTableHeader->stHeader);
        processor = new LigatureSubstitutionProcessor2(header, dir, success);
        break;
    }

    case mstReservedUnused:
        break;

    case mstNonContextualGlyphSubstitution: {
        LEReferenceTo<NonContextualGlyphSubstitutionHeader2> header(base, success);
        LEReferenceTo<LookupTable>                           lookupTable(header, success, &header->table);
        processor = new NonContextualGlyphSubstitutionProcessor2(lookupTable, success);
        break;
    }

    case mstContextualGlyphInsertion: {
        LEReferenceTo<MorphStateTableHeader2> morphStateTableHeader(base, success);
        LEReferenceTo<StateTableHeader2>      header(morphStateTableHeader, success, &morphStateTableHeader->stHeader);
        processor = new ContextualGlyphInsertionProcessor2(header, dir, success);
        break;
    }

    default:
        return;
        break; /* NOTREACHED */
    }

    if (processor != NULL) {
        processor->process(glyphStorage, success);
        delete processor;
    } else {
        if (LE_SUCCESS(success)) {
            success = LE_MEMORY_ALLOCATION_ERROR; // because ptr is null and we didn't break out.
        }
    }
}

U_NAMESPACE_END
