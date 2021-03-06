/*
 *
 * (C) Copyright IBM Corp. 1998-2013 - All Rights Reserved
 *
 */

#ifndef __LOOKUPTABLES_H
#define __LOOKUPTABLES_H

/**
 * \file
 * \internal
 */

#include "LETypes.h"
#include "LayoutTables.h"
#include "LETableReference.h"

U_NAMESPACE_BEGIN

enum LookupTableFormat
{
    ltfSimpleArray      = 0,
    ltfSegmentSingle    = 2,
    ltfSegmentArray     = 4,
    ltfSingleTable      = 6,
    ltfTrimmedArray     = 8
};

typedef le_uint16 LookupValue;

struct LookupTable
{
    le_uint16 format;
};

struct LookupSegment
{
    TTGlyphID   lastGlyph;
    TTGlyphID   firstGlyph;
    LookupValue value;
};

struct LookupSingle
{
    TTGlyphID   glyph;
    LookupValue value;
};

struct BinarySearchLookupTable : LookupTable
{
    le_uint16 unitSize;
    le_uint16 nUnits;
    le_uint16 searchRange;
    le_uint16 entrySelector;
    le_uint16 rangeShift;

    le_bool validate(const LETableReference &base, le_uint32 _unitSize) const;
    const LookupSingle *lookupSingle(const LETableReference &base, const LookupSingle *entries, LEGlyphID glyph, LEErrorCode &success) const;
    const LookupSegment *lookupSegment(const LETableReference &base, const LookupSegment *segments, LEGlyphID glyph, LEErrorCode &success) const;
};

struct SimpleArrayLookupTable : LookupTable
{
    LookupValue valueArray[ANY_NUMBER];
};
LE_VAR_ARRAY(SimpleArrayLookupTable, valueArray)

struct SegmentSingleLookupTable : BinarySearchLookupTable
{
    LookupSegment segments[ANY_NUMBER];
};
LE_VAR_ARRAY(SegmentSingleLookupTable, segments)

struct SegmentArrayLookupTable : BinarySearchLookupTable
{
    LookupSegment segments[ANY_NUMBER];
};
LE_VAR_ARRAY(SegmentArrayLookupTable, segments)

struct SingleTableLookupTable : BinarySearchLookupTable
{
    LookupSingle entries[ANY_NUMBER];
};
LE_VAR_ARRAY(SingleTableLookupTable, entries)

struct TrimmedArrayLookupTable : LookupTable
{
    TTGlyphID   firstGlyph;
    TTGlyphID   glyphCount;
    LookupValue valueArray[ANY_NUMBER];
};
LE_VAR_ARRAY(TrimmedArrayLookupTable, valueArray)

U_NAMESPACE_END

#endif
