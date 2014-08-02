/*
 *
 * (C) Copyright IBM Corp.  and others 1998-2013 - All Rights Reserved
 *
 */

#ifndef __INDICREARRANGEMENTPROCESSOR2_H
#define __INDICREARRANGEMENTPROCESSOR2_H

/**
 * \file
 * \internal
 */

#include "LETypes.h"
#include "IndicRearrangement.h"
#include "StateTableProcessor2.h"

U_NAMESPACE_BEGIN

class LEGlyphStorage;

class IndicRearrangementProcessor2 : public StateTableProcessor2
{
public:
    virtual void beginStateTable(LEGlyphStorage &glyphStorage, LEErrorCode &success);

    virtual le_uint16 processStateEntry(LEGlyphStorage &glyphStorage, le_int32 &currGlyph, EntryTableIndex2 index, LEErrorCode &success);

    virtual void endStateTable(LEGlyphStorage &glyphStorage, LEErrorCode &success);

    IndicRearrangementProcessor2(const LEReferenceTo<StateTableHeader2> &header, le_int32 dir, LEErrorCode &success);
    virtual ~IndicRearrangementProcessor2();

protected:
    le_int32 firstGlyph;
    le_int32 lastGlyph;

    LEReferenceTo<IndicRearrangementSubtableHeader2> indicRearrangementSubtableHeader;
    LEReferenceToArrayOf<IndicRearrangementStateEntry2> entryTable;
};

U_NAMESPACE_END

#endif
