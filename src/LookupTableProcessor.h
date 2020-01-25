#ifndef __LOOKUPTABLEPROCESSOR_H
#define __LOOKUPTABLEPROCESSOR_H

#include "LETypes.h"
#include "LookupTables.h"

U_NAMESPACE_BEGIN

class LookupTableProcessor : public UMemory {
public:
    virtual le_bool lookup(LEGlyphID glyph, LookupValue &value, LEErrorCode &success) = 0;

    static LookupTableProcessor *createInstance(LookupTableFormat format, const LEReferenceTo<LookupTable> &lookupTable, LEErrorCode &success);

    virtual ~LookupTableProcessor();

protected:
    inline LookupTableProcessor() {}
};

class SimpleArrayLookupTableProcessor : public LookupTableProcessor {
public:
    le_bool lookup(LEGlyphID glyph, LookupValue &value, LEErrorCode &success);

    SimpleArrayLookupTableProcessor(const LEReferenceTo<LookupTable> &lookupTable, LEErrorCode &success);

    virtual ~SimpleArrayLookupTableProcessor();

private:
    SimpleArrayLookupTableProcessor();

protected:
    LEReferenceTo<SimpleArrayLookupTable> simpleArrayLookupTable;

    LEReferenceToArrayOf<LookupValue> valueArray;
    le_uint32 valueCount;
};

class SegmentSingleLookupTableProcessor : public LookupTableProcessor {
public:
    le_bool lookup(LEGlyphID glyph, LookupValue &value, LEErrorCode &success);

    SegmentSingleLookupTableProcessor(const LEReferenceTo<LookupTable> &lookupTable, LEErrorCode &success);

    virtual ~SegmentSingleLookupTableProcessor();

private:
    SegmentSingleLookupTableProcessor();

protected:
    LEReferenceTo<SegmentSingleLookupTable> segmentSingleLookupTable;
};

class SegmentArrayLookupTableProcessor : public LookupTableProcessor {
public:
    le_bool lookup(LEGlyphID glyph, LookupValue &value, LEErrorCode &success);

    SegmentArrayLookupTableProcessor(const LEReferenceTo<LookupTable> &lookupTable, LEErrorCode &success);

    virtual ~SegmentArrayLookupTableProcessor();

private:
    SegmentArrayLookupTableProcessor();

protected:
    LEReferenceTo<SegmentArrayLookupTable> segmentArrayLookupTable;
};

class SingleTableLookupTableProcessor : public LookupTableProcessor {
public:
    le_bool lookup(LEGlyphID glyph, LookupValue &value, LEErrorCode &success);

    SingleTableLookupTableProcessor(const LEReferenceTo<LookupTable> &lookupTable, LEErrorCode &success);

    virtual ~SingleTableLookupTableProcessor();

private:
    SingleTableLookupTableProcessor();

protected:
    LEReferenceTo<SingleTableLookupTable> singleTableLookupTable;
};

class TrimmedArrayLookupTableProcessor : public LookupTableProcessor {
public:
    le_bool lookup(LEGlyphID glyph, LookupValue &value, LEErrorCode &success);

    TrimmedArrayLookupTableProcessor(const LEReferenceTo<LookupTable> &lookupTable, LEErrorCode &success);

    virtual ~TrimmedArrayLookupTableProcessor();

private:
    TrimmedArrayLookupTableProcessor();

protected:
    LEReferenceTo<TrimmedArrayLookupTable> trimmedArrayLookupTable;
    le_uint32                              firstGlyph;
    le_uint32                              glyphCount;
    le_uint32                              limitGlyph;
    LEReferenceToArrayOf<LookupValue>      valueArray;
};

U_NAMESPACE_END

#endif
