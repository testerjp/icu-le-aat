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

struct PairInfo;

/**
 * Windows type 0 kerning table support only for now.
 */
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
