/*
 *
 * (C) Copyright IBM Corp. 1998-2013 - All Rights Reserved
 *
 */

#ifndef __NONCONTEXTUALGLYPHSUBSTITUTIONPROCESSOR_H
#define __NONCONTEXTUALGLYPHSUBSTITUTIONPROCESSOR_H

/**
 * \file
 * \internal
 */

#include "LETypes.h"
#include "SubtableProcessor.h"
#include "NonContextualGlyphSubst.h"

U_NAMESPACE_BEGIN

class LEGlyphStorage;
class LookupTableProcessor;

class NonContextualGlyphSubstitutionProcessor : public SubtableProcessor
{
public:
    void process(LEGlyphStorage &glyphStorage, LEErrorCode &success);

    NonContextualGlyphSubstitutionProcessor(const LEReferenceTo<LookupTable> &lookupTable, LEErrorCode &success);

    virtual ~NonContextualGlyphSubstitutionProcessor();

    static SubtableProcessor *createInstance(le_uint16 format, const LEReferenceTo<LookupTable> &lookupTable, LEErrorCode &success);

private:
    NonContextualGlyphSubstitutionProcessor();
    NonContextualGlyphSubstitutionProcessor(const NonContextualGlyphSubstitutionProcessor &other); // forbid copying of this class
    NonContextualGlyphSubstitutionProcessor &operator=(const NonContextualGlyphSubstitutionProcessor &other); // forbid copying of this class

    LookupTableProcessor *processor;
};

U_NAMESPACE_END

#endif
