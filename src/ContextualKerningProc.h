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
    virtual le_uint16 processStateEntry(LEGlyphStorage &glyphStorage, le_int32 &currGlyph, EntryTableIndex index, LEErrorCode &success);
    virtual void endStateTable(LEGlyphStorage &glyphStorage, LEErrorCode &success);

    ContextualKerningProcessor(const LEReferenceTo<StateTableHeader> &header, le_int32 dir, le_bool crossStream, LEErrorCode &success);
    virtual ~ContextualKerningProcessor();

protected:
    le_bool                                           crossStream;
    le_int32                                          kerningStack[nComponents];
    le_int16                                          sp;
    le_int16                                         *kerningValues;
    LEReferenceTo<ContextualKerningHeader>            contextualKerningHeader;
    LEReferenceToArrayOf<ContextualKerningStateEntry> entryTable;
};

U_NAMESPACE_END

#endif // __CONTEXTUALKERNINGPROCESSOR_H
