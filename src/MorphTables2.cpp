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

void MorphTableHeader2::process(const LEReferenceTo<MorphTableHeader2> &base, LEGlyphStorage &glyphStorage,
                                le_int32 typoFlags, LEErrorCode &success) const
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

        if (typoFlags != 0) {
            LEReferenceToArrayOf<FeatureTableEntry> featureTableRef(chainHeader, success, &chainHeader->featureTable[0], nFeatureEntries);

            if (LE_FAILURE(success)) break;

            for (le_uint32 featureEntry = 0; featureEntry < nFeatureEntries; featureEntry++) {
                const FeatureTableEntry &featureTableEntry = featureTableRef(featureEntry, success);

                if (LE_FAILURE(success)) break;

                le_uint16                featureType       = SWAPW(featureTableEntry.featureType);
                le_uint16                featureSetting    = SWAPW(featureTableEntry.featureSetting);
                le_uint32                enableFlags       = SWAPL(featureTableEntry.enableFlags);
                le_uint32                disableFlags      = SWAPL(featureTableEntry.disableFlags);

                switch (featureType) {
                case ligaturesType:
                    if ((typoFlags & LE_Ligatures_FEATURE_FLAG) && ((featureSetting & 0x1) ^ 0x1)) {
                        flag &= disableFlags;
                        flag |= enableFlags;
                    } else {
                        if (((typoFlags & LE_RLIG_FEATURE_FLAG) && featureSetting == requiredLigaturesOnSelector) ||
                            ((typoFlags & LE_CLIG_FEATURE_FLAG) && featureSetting == contextualLigaturesOnSelector) ||
                            ((typoFlags & LE_HLIG_FEATURE_FLAG) && featureSetting == historicalLigaturesOnSelector) ||
                            ((typoFlags & LE_LIGA_FEATURE_FLAG) && featureSetting == commonLigaturesOnSelector)) {
                            flag &= disableFlags;
                            flag |= enableFlags;
                        }
                    }
                    break;

                case letterCaseType:
                    if ((typoFlags & LE_SMCP_FEATURE_FLAG) && featureSetting == smallCapsSelector) {
                        flag &= disableFlags;
                        flag |= enableFlags;
                    }
                    break;

                case verticalSubstitutionType:
                    break;

                case linguisticRearrangementType:
                    break;

                case numberSpacingType:
                    break;

                case smartSwashType:
                    if ((typoFlags & LE_SWSH_FEATURE_FLAG) && ((featureSetting & 0x1) ^ 0x1)) {
                        flag &= disableFlags;
                        flag |= enableFlags;
                    }
                    break;

                case diacriticsType:
                    break;

                case verticalPositionType:
                    break;

                case fractionsType:
                    if (((typoFlags & LE_FRAC_FEATURE_FLAG) && featureSetting == diagonalFractionsSelector) ||
                        ((typoFlags & LE_AFRC_FEATURE_FLAG) && featureSetting == verticalFractionsSelector)) {
                        flag &= disableFlags;
                        flag |= enableFlags;
                    }
                    break;

                case typographicExtrasType:
                    if ((typoFlags & LE_ZERO_FEATURE_FLAG) && featureSetting == slashedZeroOnSelector) {
                        flag &= disableFlags;
                        flag |= enableFlags;
                    }
                    break;

                case mathematicalExtrasType:
                    break;

                case ornamentSetsType:
                    break;

                case characterAlternativesType:
                    break;

                case designComplexityType:
                    if (((typoFlags & LE_SS01_FEATURE_FLAG) && featureSetting == designLevel1Selector) ||
                        ((typoFlags & LE_SS02_FEATURE_FLAG) && featureSetting == designLevel2Selector) ||
                        ((typoFlags & LE_SS03_FEATURE_FLAG) && featureSetting == designLevel3Selector) ||
                        ((typoFlags & LE_SS04_FEATURE_FLAG) && featureSetting == designLevel4Selector) ||
                        ((typoFlags & LE_SS05_FEATURE_FLAG) && featureSetting == designLevel5Selector) ||
                        ((typoFlags & LE_SS06_FEATURE_FLAG) && featureSetting == designLevel6Selector) ||
                        ((typoFlags & LE_SS07_FEATURE_FLAG) && featureSetting == designLevel7Selector)) {

                        flag &= disableFlags;
                        flag |= enableFlags;
                    }
                    break;

                case styleOptionsType:
                    break;

                case characterShapeType:
                    break;

                case numberCaseType:
                    break;

                case textSpacingType:
                    break;

                case transliterationType:
                    break;

                case annotationType:
                    if ((typoFlags & LE_NALT_FEATURE_FLAG) && featureSetting == circleAnnotationSelector) {
                        flag &= disableFlags;
                        flag |= enableFlags;
                    }
                    break;

                case kanaSpacingType:
                    break;

                case ideographicSpacingType:
                    break;

                case rubyKanaType:
                    if ((typoFlags & LE_RUBY_FEATURE_FLAG) && featureSetting == rubyKanaOnSelector) {
                        flag &= disableFlags;
                        flag |= enableFlags;
                    }
                    break;

                case cjkRomanSpacingType:
                    break;

                default:
                    break;
                }
            }
        }

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
                LE_TRACE_LOG("subtable %d", subtable);
                subtableHeader->process(subtableHeader, glyphStorage, success);
            }
        }
    }
}

void MorphSubtableHeader2::process(const LEReferenceTo<MorphSubtableHeader2> &base, LEGlyphStorage &glyphStorage, LEErrorCode &success) const
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
