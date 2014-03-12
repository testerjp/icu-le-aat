/*
 *
 * (C) Copyright IBM Corp.  and others 2013 - All Rights Reserved
 *
 */

#ifndef __CONTEXTUALGLYPHINSERTIONPROCESSOR2_H
#define __CONTEXTUALGLYPHINSERTIONPROCESSOR2_H

/**
 * \file
 * \internal
 */

#include "LETypes.h"
#include "ContextualGlyphInsertion.h"
#include "StateTableProcessor2.h"

U_NAMESPACE_BEGIN

class LEGlyphStorage;

class ContextualGlyphInsertionProcessor2 : public StateTableProcessor2
{
public:
    virtual void beginStateTable(LEGlyphStorage &glyphStorage, LEErrorCode &success);

    virtual le_uint16 processStateEntry(LEGlyphStorage &glyphStorage,
                                        le_int32 &currGlyph, EntryTableIndex2 index, LEErrorCode &success);

    virtual void endStateTable(LEGlyphStorage &glyphStorage, LEErrorCode &success);

    ContextualGlyphInsertionProcessor2(const LEReferenceTo<StateTableHeader2> &header, le_int32 dir, LEErrorCode &success);
    virtual ~ContextualGlyphInsertionProcessor2();

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
    ContextualGlyphInsertionProcessor2();

    /**
     * Perform the actual insertion
     * @param atGlyph index of glyph to insert at
     * @param index index into the insertionTable (in/out)
     * @param count number of insertions
     * @param isKashidaLike Kashida like (vs Split Vowel like). No effect currently.
     * @param isBefore if true, insert extra glyphs before the marked glyph
     */
    void doInsertion(LEGlyphStorage &glyphStorage, le_int16 atGlyph, le_uint16 &index, le_int16 count, le_bool isKashidaLike, le_bool isBefore, LEErrorCode &success);

protected:
    le_int32 markGlyph;

    LEReferenceTo<ContextualGlyphInsertionHeader2> contextualGlyphInsertionHeader;
    LEReferenceToArrayOf<ContextualGlyphInsertionStateEntry2> entryTable;
    LEReferenceToArrayOf<le_uint16> insertionAction;
};

U_NAMESPACE_END

#endif
