/*
 *
 * (C) Copyright IBM Corp. 1998-2013 - All Rights Reserved
 *
 */

#include "LETypes.h"
#include "LayoutEngine.h"
#include "GXLayoutEngine.h"
#include "LEGlyphStorage.h"
#include "KernTables.h"
#include "MorphTables.h"

U_NAMESPACE_BEGIN

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(GXLayoutEngine)

GXLayoutEngine::GXLayoutEngine(const LEFontInstance *fontInstance, le_int32 scriptCode, le_int32 languageCode, const LEReferenceTo<MorphTableHeader> &morphTable, LEErrorCode &success)
    : LayoutEngine(fontInstance, scriptCode, languageCode, 0, success)
    , fMorphTable(morphTable)
{
    fFilterZeroWidth = FALSE;

    // nothing else to do?
}

GXLayoutEngine::~GXLayoutEngine()
{
    reset();
}

le_int32
GXLayoutEngine::computeGlyphs(const LEUnicode chars[], le_int32 offset, le_int32 count, le_int32 max, le_bool rightToLeft, LEGlyphStorage &glyphStorage, LEErrorCode &success)
{
    if (LE_FAILURE(success))
        return 0;

    if (chars == NULL || offset < 0 || count < 0 || max < 0 || offset >= max || offset + count > max) {
        success = LE_ILLEGAL_ARGUMENT_ERROR;
        return 0;
    }

    mapCharsToGlyphs(chars, offset, count, FALSE, rightToLeft, glyphStorage, success);

    if (LE_FAILURE(success))
        return 0;

    fMorphTable->process(fMorphTable, glyphStorage, success);

    return count;
}

void
GXLayoutEngine::adjustGlyphPositions(const LEUnicode chars[], le_int32 offset, le_int32 count, le_bool /* reverse */, LEGlyphStorage &glyphStorage, LEErrorCode &success)
{
    if (LE_FAILURE(success))
        return;

    if (chars == NULL || offset < 0 || count < 0) {
        success = LE_ILLEGAL_ARGUMENT_ERROR;
        return;
    }

    if (fTypoFlags & LE_Kerning_FEATURE_FLAG) {
        LETableReference kernTable(fFontInstance, LE_KERN_TABLE_TAG, success);
        if (kernTable.isValid()) {
            KernTable kt(kernTable, success);
            kt.process(glyphStorage, success);
        }
    }

    return;
}

U_NAMESPACE_END
