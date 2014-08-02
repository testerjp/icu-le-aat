/*
 *
 * (C) Copyright IBM Corp.  and others 1998-2013 - All Rights Reserved
 *
 */

#ifndef __CONTEXTUALGLYPHSUBSTITUTIONPROCESSOR2_H
#define __CONTEXTUALGLYPHSUBSTITUTIONPROCESSOR2_H

/**
 * \file
 * \internal
 */

#include "LETypes.h"
#include "ContextualGlyphSubstitution.h"
#include "StateTableProcessor2.h"

U_NAMESPACE_BEGIN

class LEGlyphStorage;

class ContextualGlyphSubstitutionProcessor2 : public StateTableProcessor2
{
public:
    virtual void beginStateTable(LEGlyphStorage &glyphStorage, LEErrorCode &success);

    virtual le_uint16 processStateEntry(LEGlyphStorage &glyphStorage, le_int32 &currGlyph, EntryTableIndex2 index, LEErrorCode &success);

    virtual void endStateTable(LEGlyphStorage &glyphStorage, LEErrorCode &success);

    ContextualGlyphSubstitutionProcessor2(const LEReferenceTo<StateTableHeader2> &header, le_int32 dir, LEErrorCode &success);
    virtual ~ContextualGlyphSubstitutionProcessor2();

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
    ContextualGlyphSubstitutionProcessor2();
    le_bool lookup(le_uint32 offset, LEGlyphID gid, TTGlyphID &newGlyphs, LEErrorCode &success);

protected:
    le_int32 markGlyph;

    LEReferenceTo<ContextualGlyphSubstitutionHeader2> contextualGlyphSubstitutionHeader;
    LEReferenceToArrayOf<ContextualGlyphSubstitutionStateEntry2> entryTable;
    LEReferenceToArrayOf<le_uint32> substitutionTable;
};

U_NAMESPACE_END

#endif
