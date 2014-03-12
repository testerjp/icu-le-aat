/*
 *
 * (C) Copyright IBM Corp. 1998-2013 - All Rights Reserved
 *
 */

#ifndef __LIGATURESUBSTITUTIONPROCESSOR_H
#define __LIGATURESUBSTITUTIONPROCESSOR_H

/**
 * \file
 * \internal
 */

#include "LETypes.h"
#include "LigatureSubstitution.h"
#include "StateTableProcessor.h"

U_NAMESPACE_BEGIN

class LEGlyphStorage;

#define nComponents 16

class LigatureSubstitutionProcessor : public StateTableProcessor
{
public:
    virtual void beginStateTable(LEGlyphStorage &glyphStorage, LEErrorCode &success);

    virtual ByteOffset processStateEntry(LEGlyphStorage &glyphStorage, le_int32 &currGlyph, EntryTableIndex index, LEErrorCode &success);

    virtual void endStateTable(LEGlyphStorage &glyphStorage, LEErrorCode &success);

    LigatureSubstitutionProcessor(const LEReferenceTo<StateTableHeader> &header, le_int32 dir, LEErrorCode &success);
    virtual ~LigatureSubstitutionProcessor();

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

private:
    LigatureSubstitutionProcessor();

protected:
    ByteOffset ligatureActionTableOffset;
    ByteOffset componentTableOffset;
    ByteOffset ligatureTableOffset;

    le_int32 componentStack[nComponents];
    le_int16 m;

    LEReferenceTo<LigatureSubstitutionHeader> ligatureSubstitutionHeader;
    LEReferenceToArrayOf<LigatureSubstitutionStateEntry> entryTable;

};

U_NAMESPACE_END

#endif
