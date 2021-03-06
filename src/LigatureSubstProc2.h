/*
 *
 * (C) Copyright IBM Corp.  and others 1998-2013 - All Rights Reserved
 *
 */

#ifndef __LIGATURESUBSTITUTIONPROCESSOR2_H
#define __LIGATURESUBSTITUTIONPROCESSOR2_H

/**
 * \file
 * \internal
 */

#include "LETypes.h"
#include "LigatureSubstitution.h"
#include "StateTableProcessor2.h"

U_NAMESPACE_BEGIN

class LEGlyphStorage;

#define nComponents 16

class LigatureSubstitutionProcessor2 : public StateTableProcessor2
{
public:
    virtual void beginStateTable(LEGlyphStorage &glyphStorage, LEErrorCode &success);
    virtual le_uint16 processStateEntry(LEGlyphStorage &glyphStorage, le_int32 &currGlyph, EntryTableIndex2 index, LEErrorCode &success);
    virtual void endStateTable(LEGlyphStorage &glyphStorage, LEErrorCode &success);

    LigatureSubstitutionProcessor2(const LEReferenceTo<StateTableHeader2> &header, le_int32 dir, LEErrorCode &success);
    virtual ~LigatureSubstitutionProcessor2();

private:
    LigatureSubstitutionProcessor2();

protected:
    le_uint32                                             ligActionOffset;
    le_uint32                                             componentOffset;
    le_uint32                                             ligatureOffset;
    le_int32                                              componentStack[nComponents];
    le_int16                                              m;
    LEReferenceTo<LigatureSubstitutionHeader2>            ligatureSubstitutionHeader;
    LEReferenceToArrayOf<LigatureSubstitutionStateEntry2> entryTable;
};

U_NAMESPACE_END

#endif
