/*
 *
 * (C) Copyright IBM Corp.  and others 1998-2013 - All Rights Reserved
 *
 */

#include "LETypes.h"
#include "LESwaps.h"
#include "MorphTables.h"
#include "SubtableProcessor2.h"
#include "NonContextualGlyphSubst.h"
#include "NonContextualGlyphSubstProc2.h"
#include "SimpleArrayProcessor2.h"
#include "SegmentSingleProcessor2.h"
#include "SegmentArrayProcessor2.h"
#include "SingleTableProcessor2.h"
#include "TrimmedArrayProcessor2.h"

U_NAMESPACE_BEGIN

NonContextualGlyphSubstitutionProcessor2::NonContextualGlyphSubstitutionProcessor2()
{
}

NonContextualGlyphSubstitutionProcessor2::~NonContextualGlyphSubstitutionProcessor2()
{
}

SubtableProcessor2 *NonContextualGlyphSubstitutionProcessor2::createInstance(const LEReferenceTo<MorphSubtableHeader2> &morphSubtableHeader, LEErrorCode &success)
{
    const LEReferenceTo<NonContextualGlyphSubstitutionHeader2> header(morphSubtableHeader, success);

    if (LE_FAILURE(success)) return NULL;

    switch (SWAPW(header->table.format)) {
    case ltfSimpleArray: {
        LEReferenceTo<SimpleArrayLookupTable> lookupTable(header, success, &header->table);
        return new SimpleArrayProcessor2(lookupTable, success);
    }

    case ltfSegmentSingle: {
        LEReferenceTo<SegmentSingleLookupTable> lookupTable(header, success, &header->table);
        return new SegmentSingleProcessor2(lookupTable, success);
    }

    case ltfSegmentArray: {
        LEReferenceTo<SegmentArrayLookupTable> lookupTable(header, success, &header->table);
        return new SegmentArrayProcessor2(lookupTable, success);
    }

    case ltfSingleTable: {
        LEReferenceTo<SingleTableLookupTable> lookupTable(header, success, &header->table);
        return new SingleTableProcessor2(lookupTable, success);
    }

    case ltfTrimmedArray: {
        LEReferenceTo<TrimmedArrayLookupTable> lookupTable(header, success, &header->table);
        return new TrimmedArrayProcessor2(lookupTable, success);
    }

    default:
        return NULL;
    }
}

U_NAMESPACE_END
