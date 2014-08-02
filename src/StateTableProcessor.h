/*
 *
 * (C) Copyright IBM Corp. 1998-2013 - All Rights Reserved
 *
 */

#ifndef __STATETABLEPROCESSOR_H
#define __STATETABLEPROCESSOR_H

/**
 * \file
 * \internal
 */

#include "LETypes.h"
#include "StateTables.h"
#include "SubtableProcessor.h"

U_NAMESPACE_BEGIN

class LEGlyphStorage;

class StateTableProcessor : public SubtableProcessor
{
public:
    void process(LEGlyphStorage &glyphStorage, LEErrorCode &success);

    virtual void beginStateTable(LEGlyphStorage &glyphStorage, LEErrorCode &success) = 0;

    virtual le_uint16 processStateEntry(LEGlyphStorage &glyphStorage, le_int32 &currGlyph, EntryTableIndex index, LEErrorCode &success) = 0;

    virtual void endStateTable(LEGlyphStorage &glyphStorage, LEErrorCode &success) = 0;

protected:
    StateTableProcessor(const LEReferenceTo<StateTableHeader> &header, le_int32 dir, LEErrorCode &success);
    virtual ~StateTableProcessor();

    StateTableProcessor();

    le_int32  dir;
    le_uint16 stateSize;       // size of row, that is number of class
    le_uint16 classTableOffset;
    le_uint16 stateArrayOffset;
    le_uint16 entryTableOffset;

    TTGlyphID firstGlyph;
    TTGlyphID nGlyphs;

    LEReferenceTo<StateTableHeader> stateTableHeader;
    LEReferenceTo<ClassTable> classTable;
    LEReferenceToArrayOf<le_uint8> classArray;

private:
    StateTableProcessor(const StateTableProcessor &other); // forbid copying of this class
    StateTableProcessor &operator=(const StateTableProcessor &other); // forbid copying of this class
};

U_NAMESPACE_END
#endif
