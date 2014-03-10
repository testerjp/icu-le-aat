/*
 *
 * (C) Copyright IBM Corp. 1998-2013 - All Rights Reserved
 *
 */

#ifndef __CONTEXTUALGLYPHSUBSTITUTIONPROCESSOR_H
#define __CONTEXTUALGLYPHSUBSTITUTIONPROCESSOR_H

/**
 * \file
 * \internal
 */

#include "LETypes.h"
#include "ContextualGlyphSubstitution.h"
#include "StateTableProcessor.h"

U_NAMESPACE_BEGIN

class LEGlyphStorage;

class ContextualGlyphSubstitutionProcessor : public StateTableProcessor
{
public:
    virtual void beginStateTable();

    virtual ByteOffset processStateEntry(LEGlyphStorage &glyphStorage, le_int32 &currGlyph, EntryTableIndex index);

    virtual void endStateTable();

    ContextualGlyphSubstitutionProcessor(const LEReferenceTo<StateTableHeader> &header, le_int32 dir, LEErrorCode &success);
    virtual ~ContextualGlyphSubstitutionProcessor();

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
    ContextualGlyphSubstitutionProcessor();

protected:
    ByteOffset substitutionTableOffset;
    le_int32 markGlyph;

    LEReferenceTo<ContextualGlyphSubstitutionHeader> contextualGlyphSubstitutionHeader;
    LEReferenceToArrayOf<ContextualGlyphSubstitutionStateEntry> entryTable;
    LEReferenceToArrayOf<le_int16> int16Table;
};

U_NAMESPACE_END

#endif
