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

    virtual void beginStateTable() = 0;

    virtual ByteOffset processStateEntry(LEGlyphStorage &glyphStorage, le_int32 &currGlyph, EntryTableIndex index) = 0;

    virtual void endStateTable() = 0;

protected:
    StateTableProcessor(const LEReferenceTo<StateTableHeader> &header, LEErrorCode &success);
    virtual ~StateTableProcessor();

    StateTableProcessor();

    le_int16 stateSize; // size of row, that is number of class
    ByteOffset classTableOffset;
    ByteOffset stateArrayOffset;
    ByteOffset entryTableOffset;

    TTGlyphID firstGlyph;
    TTGlyphID lastGlyph;

    LEReferenceTo<StateTableHeader> stateTableHeader;
    LEReferenceTo<ClassTable> classTable;

private:
    StateTableProcessor(const StateTableProcessor &other); // forbid copying of this class
    StateTableProcessor &operator=(const StateTableProcessor &other); // forbid copying of this class
};

U_NAMESPACE_END
#endif
