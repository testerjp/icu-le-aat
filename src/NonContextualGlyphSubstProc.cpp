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

SubtableProcessor *NonContextualGlyphSubstitutionProcessor::createInstance(le_int16 format, const LEReferenceTo<LookupTable> &lookupTable, LEErrorCode &success)
{
    switch (format) {
    case ltfSimpleArray: {
        LEReferenceTo<SimpleArrayLookupTable> simpleArrayLookupTable(lookupTable, success);
        return new SimpleArrayProcessor(simpleArrayLookupTable, success);
    }

    case ltfSegmentSingle: {
        LEReferenceTo<SegmentSingleLookupTable> segmentSingleLookupTable(lookupTable, success);
        return new SegmentSingleProcessor(segmentSingleLookupTable, success);
    }

    case ltfSegmentArray: {
        LEReferenceTo<SegmentArrayLookupTable> segmentArrayLookupTable(lookupTable, success);
        return new SegmentArrayProcessor(segmentArrayLookupTable, success);
    }

    case ltfSingleTable: {
        LEReferenceTo<SingleTableLookupTable> singleTableLookupTable(lookupTable, success);
        return new SingleTableProcessor(singleTableLookupTable, success);
    }

    case ltfTrimmedArray: {
        LEReferenceTo<TrimmedArrayLookupTable> trimmedArrayLookupTable(lookupTable, success);
        return new TrimmedArrayProcessor(trimmedArrayLookupTable, success);
    }

    default:
        return NULL;
    }
}

U_NAMESPACE_END
