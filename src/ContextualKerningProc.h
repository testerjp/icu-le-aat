#ifndef __CONTEXTUALKERNINGPROCESSOR_H
#define __CONTEXTUALKERNINGPROCESSOR_H

/**
 * \file
 * \internal
 */

#include "LETypes.h"
#include "ContextualKerning.h"
#include "StateTableProcessor.h"

U_NAMESPACE_BEGIN

#define nComponents 8

class ContextualKerningProcessor : public StateTableProcessor
{
public:
    virtual void beginStateTable(LEGlyphStorage &glyphStorage, LEErrorCode &success);

    virtual ByteOffset processStateEntry(LEGlyphStorage &glyphStorage, le_int32 &currGlyph, EntryTableIndex index, LEErrorCode &success);

    virtual void endStateTable(LEGlyphStorage &glyphStorage, LEErrorCode &success);

    ContextualKerningProcessor(const LEReferenceTo<StateTableHeader> &header, le_int32 dir, LEErrorCode &success);
    virtual ~ContextualKerningProcessor();

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
    le_int32 kerningStack[nComponents];
    le_int16 sp;

    le_int16 *kerningValues;

    LEReferenceTo<ContextualKerningHeader> contextualKerningHeader;
    LEReferenceToArrayOf<ContextualKerningStateEntry> entryTable;
};

U_NAMESPACE_END

#endif // __CONTEXTUALKERNINGPROCESSOR_H
