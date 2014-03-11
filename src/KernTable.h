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
#include "OpenTypeTables.h"

U_NAMESPACE_BEGIN

enum KernFormat
{
    kfKerningPairs      = 0,
    kfContextualKerning = 1,
    kfKerningValue      = 2,
    kfKerningIndices    = 3,
};

enum KernCoverageFlags
{
    kcfHorizontal  = 0x1,
    kcfMinimum     = 0x2,
    kcfCrossStream = 0x4,
    kcfOverride    = 0x8,
    kcfFormatMask  = 0xFF00,
    kcfFormatShift = 8,
};

enum KernCoverageFlags2
{
    kcf2Vertical    = 0x800,
    kcf2CrossStream = 0x400,
    kcf2Variation   = 0x200,
    kcf2FormatMask  = 0x00FF,
    kcf2FormatShift = 0,
};

struct KerningPair {
    le_uint16 left;
    le_uint16 right;
    le_int16  value;
};

struct KernSubtableKerningPairs {
    le_uint16 nPairs;
    le_uint16 searchRange;
    le_uint16 entrySelector;
    le_uint16 rangeShift;
};

struct KernSubtableHeader {
    le_uint16 version;
    le_uint16 length;
    le_uint16 coverage;
};

struct KernTableHeader {
    le_uint16 version;
    le_uint16 nTables;
};

struct KernSubtableHeader2 {
    le_uint32 length;
    le_uint16 coverage;
    le_uint16 tupleIndex;

    void process(const LEReferenceTo<KernSubtableHeader2> &base, LEGlyphStorage &glyphStorage, LEErrorCode &success) const;
};

struct KernTableHeader2 {
    fixed32   version;
    le_uint32 nTables;
};

class U_LAYOUT_API KernTable
{
public:
    KernTable(const LETableReference &table, LEErrorCode &success);

    inline fixed32 getVersion() const { return version; }
    void process(LEGlyphStorage& storage, LEErrorCode &success);

private:
    fixed32   version;
    le_uint32 nTables;

    LETableReference table;
};

U_NAMESPACE_END

#endif
