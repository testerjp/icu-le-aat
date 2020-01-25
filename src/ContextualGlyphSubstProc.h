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
    virtual void beginStateTable(LEGlyphStorage &glyphStorage, LEErrorCode &success);
    virtual le_uint16 processStateEntry(LEGlyphStorage &glyphStorage, le_int32 &currGlyph, EntryTableIndex index, LEErrorCode &success);
    virtual void endStateTable(LEGlyphStorage &glyphStorage, LEErrorCode &success);

    ContextualGlyphSubstitutionProcessor(const LEReferenceTo<StateTableHeader> &header, le_int32 dir, LEErrorCode &success);
    virtual ~ContextualGlyphSubstitutionProcessor();

private:
    ContextualGlyphSubstitutionProcessor();

protected:
    le_int32                                                    markGlyph;
    LEReferenceTo<ContextualGlyphSubstitutionHeader>            contextualGlyphSubstitutionHeader;
    LEReferenceToArrayOf<ContextualGlyphSubstitutionStateEntry> entryTable;
    LEReferenceToArrayOf<le_int16>                              int16Table;
};

U_NAMESPACE_END

#endif
