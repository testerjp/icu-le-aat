/*
 *
 * (C) Copyright IBM Corp. 1998-2013 - All Rights Reserved
 *
 */

#include "LETypes.h"
#include "LESwaps.h"
#include "NonContextualGlyphSubstProc.h"
#include "SimpleArrayProcessor.h"
#include "SegmentSingleProcessor.h"
#include "SegmentArrayProcessor.h"
#include "SingleTableProcessor.h"
#include "TrimmedArrayProcessor.h"

U_NAMESPACE_BEGIN

NonContextualGlyphSubstitutionProcessor::NonContextualGlyphSubstitutionProcessor()
{
}

NonContextualGlyphSubstitutionProcessor::~NonContextualGlyphSubstitutionProcessor()
{
}

SubtableProcessor *NonContextualGlyphSubstitutionProcessor::createInstance(const LEReferenceTo<MorphSubtableHeader> &morphSubtableHeader, LEErrorCode &success)
{
    LEReferenceTo<NonContextualGlyphSubstitutionHeader> header(morphSubtableHeader, success);

    if (LE_FAILURE(success)) return NULL;

    switch (SWAPW(header->table.format)) {
    case ltfSimpleArray: {
        LEReferenceTo<SimpleArrayLookupTable> lookupTable(header, success, &header->table);
        return new SimpleArrayProcessor(lookupTable, success);
    }

    case ltfSegmentSingle: {
        LEReferenceTo<SegmentSingleLookupTable> lookupTable(header, success, &header->table);
        return new SegmentSingleProcessor(lookupTable, success);
    }

    case ltfSegmentArray: {
        LEReferenceTo<SegmentArrayLookupTable> lookupTable(header, success, &header->table);
        return new SegmentArrayProcessor(lookupTable, success);
    }

    case ltfSingleTable: {
        LEReferenceTo<SingleTableLookupTable> lookupTable(header, success, &header->table);
        return new SingleTableProcessor(lookupTable, success);
    }

    case ltfTrimmedArray: {
        LEReferenceTo<TrimmedArrayLookupTable> lookupTable(header, success, &header->table);
        return new TrimmedArrayProcessor(lookupTable, success);
    }

    default:
        return NULL;
    }
}

U_NAMESPACE_END
