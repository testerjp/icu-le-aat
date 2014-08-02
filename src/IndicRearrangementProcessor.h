/*
 *
 * (C) Copyright IBM Corp. 1998-2013 - All Rights Reserved
 *
 */

#ifndef __INDICREARRANGEMENTPROCESSOR_H
#define __INDICREARRANGEMENTPROCESSOR_H

/**
 * \file
 * \internal
 */

#include "LETypes.h"
#include "IndicRearrangement.h"
#include "StateTableProcessor.h"

U_NAMESPACE_BEGIN

class LEGlyphStorage;

class IndicRearrangementProcessor : public StateTableProcessor
{
public:
    virtual void beginStateTable(LEGlyphStorage &glyphStorage, LEErrorCode &success);

    virtual le_uint16 processStateEntry(LEGlyphStorage &glyphStorage, le_int32 &currGlyph, EntryTableIndex index, LEErrorCode &success);

    virtual void endStateTable(LEGlyphStorage &glyphStorage, LEErrorCode &success);

    IndicRearrangementProcessor(const LEReferenceTo<StateTableHeader> &header, le_int32 dir, LEErrorCode &success);
    virtual ~IndicRearrangementProcessor();

protected:
    le_int32 firstGlyph;
    le_int32 lastGlyph;

    LEReferenceTo<IndicRearrangementSubtableHeader> indicRearrangementSubtableHeader;
    LEReferenceToArrayOf<IndicRearrangementStateEntry> entryTable;
};

U_NAMESPACE_END

#endif
