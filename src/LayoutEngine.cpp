/*
 * (C) Copyright IBM Corp. and others 1998-2013 - All Rights Reserved
 */

#include "LETypes.h"
#include "LEGlyphStorage.h"
#include "LELanguages.h"
#include "LEScripts.h"
#include "LESwaps.h"

#include "GXLayoutEngine.h"
#include "GXLayoutEngine2.h"
#include "LayoutEngine.h"

#include "KernTables.h"
#include "MorphTables.h"

#include "unicode/uchar.h"

U_NAMESPACE_BEGIN

class DefaultCharMapper : public LECharMapper
{
public:
    DefaultCharMapper(le_bool mirror);
    virtual ~DefaultCharMapper();
    LEUnicode32 mapChar(LEUnicode32 ch) const;

private:
    le_bool fMirror;
};

/* Leave this copyright notice here! It needs to go somewhere in this library. */
static const char copyright[] = U_COPYRIGHT_STRING;

const le_int32 LayoutEngine::kTypoFlagKern = LE_Kerning_FEATURE_FLAG;
const le_int32 LayoutEngine::kTypoFlagLiga = LE_Ligatures_FEATURE_FLAG;

DefaultCharMapper::DefaultCharMapper(le_bool mirror)
    : fMirror(mirror)
{
}

DefaultCharMapper::~DefaultCharMapper()
{
}

LEUnicode32
DefaultCharMapper::mapChar(LEUnicode32 ch) const
{
    if (fMirror)
        return u_charMirror(ch);
    else
        return ch;
}

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(LayoutEngine)

LayoutEngine::LayoutEngine(const LEFontInstance *fontInstance, le_int32 scriptCode, le_int32 languageCode, le_int32 typoFlags, LEErrorCode &success)
    : fGlyphStorage(NULL)
    , fFontInstance(fontInstance)
    , fScriptCode(scriptCode)
    , fLanguageCode(languageCode)
    , fTypoFlags(typoFlags)
    , fFilterZeroWidth(TRUE)
{
    (void)copyright;

    if (LE_FAILURE(success))
        return;

    fGlyphStorage = new LEGlyphStorage();
    if (fGlyphStorage == NULL) {
        success = LE_MEMORY_ALLOCATION_ERROR;
    }
}

le_int32
LayoutEngine::getGlyphCount() const
{
    return fGlyphStorage->getGlyphCount();
}

void
LayoutEngine::getCharIndices(le_int32 charIndices[], le_int32 indexBase, LEErrorCode &success) const
{
    fGlyphStorage->getCharIndices(charIndices, indexBase, success);
}

void
LayoutEngine::getCharIndices(le_int32 charIndices[], LEErrorCode &success) const
{
    fGlyphStorage->getCharIndices(charIndices, success);
}

// Copy the glyphs into caller's (32-bit) glyph array, OR in extraBits
void
LayoutEngine::getGlyphs(le_uint32 glyphs[], le_uint32 extraBits, LEErrorCode &success) const
{
    fGlyphStorage->getGlyphs(glyphs, extraBits, success);
}

void
LayoutEngine::getGlyphs(LEGlyphID glyphs[], LEErrorCode &success) const
{
    fGlyphStorage->getGlyphs(glyphs, success);
}

void
LayoutEngine::getGlyphPositions(float positions[], LEErrorCode &success) const
{
    fGlyphStorage->getGlyphPositions(positions, success);
}

void
LayoutEngine::getGlyphPosition(le_int32 glyphIndex, float &x, float &y, LEErrorCode &success) const
{
    fGlyphStorage->getGlyphPosition(glyphIndex, x, y, success);
}

le_int32
LayoutEngine::computeGlyphs(const LEUnicode chars[], le_int32 offset, le_int32 count, le_int32 max, le_bool rightToLeft, LEGlyphStorage &glyphStorage, LEErrorCode &success)
{
    if (LE_FAILURE(success))
        return 0;

    if (chars == NULL || offset < 0 || count < 0 || max < 0 || offset >= max || offset + count > max) {
        success = LE_ILLEGAL_ARGUMENT_ERROR;
        return 0;
    }

    mapCharsToGlyphs(chars, offset, count, rightToLeft, rightToLeft, glyphStorage, success);

    return glyphStorage.getGlyphCount();
}

void
LayoutEngine::positionGlyphs(LEGlyphStorage &glyphStorage, float x, float y, LEErrorCode &success)
{
    if (LE_FAILURE(success))
        return;

    glyphStorage.allocatePositions(success);

    if (LE_FAILURE(success))
        return;

    le_int32 i, glyphCount = glyphStorage.getGlyphCount();

    for (i = 0; i < glyphCount; i += 1) {
        LEPoint advance;

        glyphStorage.setPosition(i, x, y, success);

        fFontInstance->getGlyphAdvance(glyphStorage[i], advance);
        x += advance.fX;
        y += advance.fY;
    }

    glyphStorage.setPosition(glyphCount, x, y, success);
}

void
LayoutEngine::adjustGlyphPositions(const LEUnicode chars[], le_int32 offset, le_int32 count, le_bool /* reverse */, LEGlyphStorage &glyphStorage, LEErrorCode &success)
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

    // default is no adjustments
    return;
}

const void *
LayoutEngine::getFontTable(LETag tableTag, size_t &length) const
{
    return fFontInstance->getFontTable(tableTag, length);
}

void
LayoutEngine::mapCharsToGlyphs(const LEUnicode chars[], le_int32 offset, le_int32 count, le_bool reverse, le_bool mirror, LEGlyphStorage &glyphStorage, LEErrorCode &success)
{
    if (LE_FAILURE(success))
        return;

    glyphStorage.allocateGlyphArray(count, reverse, success);

    DefaultCharMapper charMapper(mirror);

    fFontInstance->mapCharsToGlyphs(chars, offset, count, reverse, &charMapper, fFilterZeroWidth, glyphStorage);
}

le_int32
LayoutEngine::layoutChars(const LEUnicode chars[], le_int32 offset, le_int32 count, le_int32 max, le_bool rightToLeft, float x, float y, LEErrorCode &success)
{
    if (LE_FAILURE(success))
        return 0;

    if (chars == NULL || offset < 0 || count < 0 || max < 0 || offset >= max || offset + count > max) {
        success = LE_ILLEGAL_ARGUMENT_ERROR;
        return 0;
    }

    le_int32 glyphCount;

    if (fGlyphStorage->getGlyphCount() > 0) {
        fGlyphStorage->reset();
    }

    glyphCount = computeGlyphs(chars, offset, count, max, rightToLeft, *fGlyphStorage, success);
    positionGlyphs(*fGlyphStorage, x, y, success);
    adjustGlyphPositions(chars, offset, count, rightToLeft, *fGlyphStorage, success);

    return glyphCount;
}

void
LayoutEngine::reset()
{
    if (fGlyphStorage != NULL)
        fGlyphStorage->reset();
}

LayoutEngine *
LayoutEngine::layoutEngineFactory(const LEFontInstance *fontInstance, le_int32 scriptCode, le_int32 languageCode, LEErrorCode &success)
{
    return LayoutEngine::layoutEngineFactory(fontInstance, scriptCode, languageCode, LE_DEFAULT_FEATURE_FLAG, success);
}

LayoutEngine *
LayoutEngine::layoutEngineFactory(const LEFontInstance *fontInstance, le_int32 scriptCode, le_int32 languageCode, le_int32 typoFlags, LEErrorCode &success)
{
    static const le_uint32 mortTableTag = LE_MORT_TABLE_TAG;
    static const le_uint32 morxTableTag = LE_MORX_TABLE_TAG;

    if (LE_FAILURE(success))
        return NULL;

    LayoutEngine *engine = NULL;
    LEErrorCode   err;

    {
        err = LE_NO_ERROR;
        LEReferenceTo<MorphTableHeader2> morxTable(fontInstance, morxTableTag, err);

        if (LE_SUCCESS(err) && morxTable.isValid() && (SWAPW(morxTable->version) == 2 || SWAPW(morxTable->version) == 3)) {
            engine = new GXLayoutEngine2(fontInstance, scriptCode, languageCode, morxTable, typoFlags, success);

            goto created;
        }
    }

    {
        err = LE_NO_ERROR;
        LEReferenceTo<MorphTableHeader> mortTable(fontInstance, mortTableTag, err);

        if (LE_SUCCESS(err) && mortTable.isValid() && SWAPL(mortTable->version) == 0x00010000) {
            engine = new GXLayoutEngine(fontInstance, scriptCode, languageCode, mortTable, success);

            goto created;
        }
    }

    {
        engine = new LayoutEngine(fontInstance, scriptCode, languageCode, typoFlags, success);
    }

created:

    if (engine && LE_FAILURE(success)) {
		delete engine;
		engine = NULL;
	}

    if (engine == NULL)
        success = LE_MEMORY_ALLOCATION_ERROR;

    return engine;
}

LayoutEngine::~LayoutEngine() {
    delete fGlyphStorage;
}

U_NAMESPACE_END
