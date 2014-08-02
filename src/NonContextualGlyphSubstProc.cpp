/*
 *
 * (C) Copyright IBM Corp. 1998-2013 - All Rights Reserved
 *
 */

#include "LETypes.h"
#include "LEGlyphStorage.h"
#include "LESwaps.h"
#include "LookupTableProcessor.h"
#include "NonContextualGlyphSubstProc.h"

U_NAMESPACE_BEGIN

NonContextualGlyphSubstitutionProcessor::NonContextualGlyphSubstitutionProcessor(const LEReferenceTo<LookupTable> &lookupTable, LEErrorCode &success)
    : processor(0)
{
    if (LE_FAILURE(success))
        return;

    processor = LookupTableProcessor::createInstance((LookupTableFormat)SWAPW(lookupTable->format), lookupTable, success);

    if (!processor)
        success = LE_INDEX_OUT_OF_BOUNDS_ERROR;
}

NonContextualGlyphSubstitutionProcessor::~NonContextualGlyphSubstitutionProcessor()
{
    if (processor)
        delete processor;
}

void NonContextualGlyphSubstitutionProcessor::process(LEGlyphStorage &glyphStorage, LEErrorCode &success)
{
    if (LE_FAILURE(success) || !processor)
        return;

    le_int32 glyphCount = glyphStorage.getGlyphCount();
    le_int32 glyph;

    for (glyph = 0; LE_SUCCESS(success) && glyph < glyphCount; glyph++) {
        LEGlyphID thisGlyph = glyphStorage[glyph];
        TTGlyphID newGlyph;

        if (processor->lookup(thisGlyph, newGlyph, success))
            glyphStorage[glyph] = LE_SET_GLYPH(thisGlyph, newGlyph);
    }
}

U_NAMESPACE_END
