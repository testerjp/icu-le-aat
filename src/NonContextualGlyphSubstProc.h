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

class NonContextualGlyphSubstitutionProcessor : public SubtableProcessor
{
public:
    virtual void process(LEGlyphStorage &glyphStorage, LEErrorCode &success) = 0;

    static SubtableProcessor *createInstance(le_int16 format, const LEReferenceTo<LookupTable> &lookupTable, LEErrorCode &success);

protected:
    NonContextualGlyphSubstitutionProcessor();

    virtual ~NonContextualGlyphSubstitutionProcessor();

private:
    NonContextualGlyphSubstitutionProcessor(const NonContextualGlyphSubstitutionProcessor &other); // forbid copying of this class
    NonContextualGlyphSubstitutionProcessor &operator=(const NonContextualGlyphSubstitutionProcessor &other); // forbid copying of this class
};

U_NAMESPACE_END

#endif
