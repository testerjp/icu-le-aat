/*
 * @(#)KernTable.h	1.1 04/10/13
 *
 * (C) Copyright IBM Corp. 2004-2013 - All Rights Reserved
 *
 */

#ifndef __KERNTABLE_H
#define __KERNTABLE_H

#include "LETypes.h"
#include "LETableReference.h"

U_NAMESPACE_BEGIN

/**
 * Windows type 0 kerning table support only for now.
 */

struct PairInfo {
    le_uint16 left;  // left glyph of kern pair
    le_uint16 right; // right glyph of kern pair
    le_int16  value; // fword, kern value in funits
};
#define KERN_PAIRINFO_SIZE 6
LE_CORRECT_SIZE(PairInfo, KERN_PAIRINFO_SIZE)

#define SWAP_KEY(p) (((le_uint32) SWAPW((p)->left) << 16) | SWAPW((p)->right))

struct Subtable_0 {
    le_uint16 nPairs;
    le_uint16 searchRange;
    le_uint16 entrySelector;
    le_uint16 rangeShift;
};
#define KERN_SUBTABLE_0_HEADER_SIZE 8
LE_CORRECT_SIZE(Subtable_0, KERN_SUBTABLE_0_HEADER_SIZE)

struct SubtableHeader {
    le_uint16 version;
    le_uint16 length;
    le_uint16 coverage;
};
#define KERN_SUBTABLE_HEADER_SIZE 6
LE_CORRECT_SIZE(SubtableHeader, KERN_SUBTABLE_HEADER_SIZE)

struct KernTableHeader {
    le_uint16 version;
    le_uint16 nTables;
};
#define KERN_TABLE_HEADER_SIZE 4
LE_CORRECT_SIZE(KernTableHeader, KERN_TABLE_HEADER_SIZE)

#define COVERAGE_HORIZONTAL 0x1
#define COVERAGE_MINIMUM    0x2
#define COVERAGE_CROSS      0x4
#define COVERAGE_OVERRIDE   0x8

class U_LAYOUT_API KernTable
{
private:
    le_uint16 coverage;
    le_uint16 nPairs;

    LEReferenceToArrayOf<PairInfo> pairs;

    const LETableReference &fTable;

    le_uint16 searchRange;
    le_uint16 entrySelector;
    le_uint16 rangeShift;

public:
    KernTable(const LETableReference &table, LEErrorCode &success);

    void process(LEGlyphStorage& storage, LEErrorCode &success);
};

U_NAMESPACE_END

#endif
