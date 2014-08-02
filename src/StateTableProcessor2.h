/*
 *
 * (C) Copyright IBM Corp.  and others 1998-2013 - All Rights Reserved
 *
 */

#ifndef __STATETABLEPROCESSOR2_H
#define __STATETABLEPROCESSOR2_H

/**
 * \file
 * \internal
 */

#include "LETypes.h"
#include "LookupTables.h"
#include "StateTables.h"
#include "SubtableProcessor.h"

U_NAMESPACE_BEGIN

class LEGlyphStorage;
class LookupTableProcessor;

class StateTableProcessor2 : public SubtableProcessor
{
public:
    void process(LEGlyphStorage &glyphStorage, LEErrorCode &success);

    virtual void beginStateTable(LEGlyphStorage &glyphStorage, LEErrorCode &success) = 0;

    virtual le_uint16 processStateEntry(LEGlyphStorage &glyphStorage, le_int32 &currGlyph, EntryTableIndex2 index, LEErrorCode &success) = 0;

    virtual void endStateTable(LEGlyphStorage &glyphStorage, LEErrorCode &success) = 0;

protected:
    StateTableProcessor2(const LEReferenceTo<StateTableHeader2> &header, le_int32 dir, LEErrorCode &success);
    virtual ~StateTableProcessor2();

    StateTableProcessor2();

    le_int32  dir;
    le_uint32 nClasses;
    le_uint32 classTableOffset;
    le_uint32 stateArrayOffset;
    le_uint32 entryTableOffset;

    LEReferenceTo<StateTableHeader2> stateTableHeader;
    LEReferenceTo<LookupTable> classTable;
    LEReferenceToArrayOf<EntryTableIndex2> stateArray;

    LookupTableProcessor *processor;

private:
    StateTableProcessor2(const StateTableProcessor2 &other); // forbid copying of this class
    StateTableProcessor2 &operator=(const StateTableProcessor2 &other); // forbid copying of this class
};

U_NAMESPACE_END

#endif
