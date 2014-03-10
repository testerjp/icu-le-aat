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
    virtual void beginStateTable();

    virtual le_uint16 processStateEntry(LEGlyphStorage &glyphStorage, le_int32 &currGlyph, EntryTableIndex2 index, LEErrorCode &success);

    virtual void endStateTable();

    void doRearrangementAction(LEGlyphStorage &glyphStorage, IndicRearrangementVerb verb) const;

    IndicRearrangementProcessor2(const LEReferenceTo<StateTableHeader2> &header, le_int32 dir, LEErrorCode &success);
    virtual ~IndicRearrangementProcessor2();

    /**
     * ICU "poor man's RTTI", returns a UClassID for the actual class.
     *
     * @stable ICU 2.8
     */
    virtual UClassID getDynamicClassID() const;

    /**
     * ICU "poor man's RTTI", returns a UClassID for this class.
     *
     * @stable ICU 2.8
     */
    static UClassID getStaticClassID();

protected:
    le_int32 firstGlyph;
    le_int32 lastGlyph;

    LEReferenceTo<IndicRearrangementSubtableHeader2> indicRearrangementSubtableHeader;
    LEReferenceToArrayOf<IndicRearrangementStateEntry2> entryTable;
};

U_NAMESPACE_END

#endif
