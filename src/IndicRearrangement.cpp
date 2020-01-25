/*
 *
 * (C) Copyright IBM Corp. 1998-2013 - All Rights Reserved
 *
 */

#include "LETypes.h"
#include "LEGlyphStorage.h"
#include "LESwaps.h"
#include "IndicRearrangement.h"

U_NAMESPACE_BEGIN

void
doRearrangementAction(LEGlyphStorage &glyphStorage, le_int32 firstGlyph, le_int32 lastGlyph, IndicRearrangementVerb verb, LEErrorCode &success)
{
    if (LE_FAILURE(success))
        return;

    LEGlyphID a, b, c, d;
    le_int32  ia, ib, ic, id, ix, x;

    switch(verb) {
    case irvNoAction:
        break;

    case irvxA:
        a = glyphStorage[firstGlyph];
        ia = glyphStorage.getCharIndex(firstGlyph, success);
        x = firstGlyph + 1;

        while (x <= lastGlyph) {
            glyphStorage[x - 1] = glyphStorage[x];
            ix = glyphStorage.getCharIndex(x, success);
            glyphStorage.setCharIndex(x - 1, ix, success);
            x += 1;
        }

        glyphStorage[lastGlyph] = a;
        glyphStorage.setCharIndex(lastGlyph, ia, success);
        break;

    case irvDx:
        d = glyphStorage[lastGlyph];
        id = glyphStorage.getCharIndex(lastGlyph, success);
        x = lastGlyph - 1;

        while (x >= firstGlyph) {
            glyphStorage[x + 1] = glyphStorage[x];
            ix = glyphStorage.getCharIndex(x, success);
            glyphStorage.setCharIndex(x + 1, ix, success);
            x -= 1;
        }

        glyphStorage[firstGlyph] = d;
        glyphStorage.setCharIndex(firstGlyph, id, success);
        break;

    case irvDxA:
        a = glyphStorage[firstGlyph];
        ia = glyphStorage.getCharIndex(firstGlyph, success);
        id = glyphStorage.getCharIndex(lastGlyph,  success);

        glyphStorage[firstGlyph] = glyphStorage[lastGlyph];
        glyphStorage[lastGlyph] = a;

        glyphStorage.setCharIndex(firstGlyph, id, success);
        glyphStorage.setCharIndex(lastGlyph,  ia, success);
        break;

    case irvxAB:
        a = glyphStorage[firstGlyph];
        b = glyphStorage[firstGlyph + 1];
        ia = glyphStorage.getCharIndex(firstGlyph, success);
        ib = glyphStorage.getCharIndex(firstGlyph + 1, success);
        x = firstGlyph + 2;

        while (x <= lastGlyph) {
            glyphStorage[x - 2] = glyphStorage[x];
            ix = glyphStorage.getCharIndex(x, success);
            glyphStorage.setCharIndex(x - 2, ix, success);
            x += 1;
        }

        glyphStorage[lastGlyph - 1] = a;
        glyphStorage[lastGlyph] = b;

        glyphStorage.setCharIndex(lastGlyph - 1, ia, success);
        glyphStorage.setCharIndex(lastGlyph, ib, success);
        break;

    case irvxBA:
        a = glyphStorage[firstGlyph];
        b = glyphStorage[firstGlyph + 1];
        ia = glyphStorage.getCharIndex(firstGlyph, success);
        ib = glyphStorage.getCharIndex(firstGlyph + 1, success);
        x = firstGlyph + 2;

        while (x <= lastGlyph) {
            glyphStorage[x - 2] = glyphStorage[x];
            ix = glyphStorage.getCharIndex(x, success);
            glyphStorage.setCharIndex(x - 2, ix, success);
            x += 1;
        }

        glyphStorage[lastGlyph - 1] = b;
        glyphStorage[lastGlyph] = a;

        glyphStorage.setCharIndex(lastGlyph - 1, ib, success);
        glyphStorage.setCharIndex(lastGlyph, ia, success);
        break;

    case irvCDx:
        c = glyphStorage[lastGlyph - 1];
        d = glyphStorage[lastGlyph];
        ic = glyphStorage.getCharIndex(lastGlyph - 1, success);
        id = glyphStorage.getCharIndex(lastGlyph, success);
        x = lastGlyph - 2;

        while (x >= firstGlyph) {
            glyphStorage[x + 2] = glyphStorage[x];
            ix = glyphStorage.getCharIndex(x, success);
            glyphStorage.setCharIndex(x + 2, ix, success);
            x -= 1;
        }

        glyphStorage[firstGlyph] = c;
        glyphStorage[firstGlyph + 1] = d;

        glyphStorage.setCharIndex(firstGlyph, ic, success);
        glyphStorage.setCharIndex(firstGlyph + 1, id, success);
        break;

    case irvDCx:
        c = glyphStorage[lastGlyph - 1];
        d = glyphStorage[lastGlyph];
        ic = glyphStorage.getCharIndex(lastGlyph - 1, success);
        id = glyphStorage.getCharIndex(lastGlyph, success);
        x = lastGlyph - 2;

        while (x >= firstGlyph) {
            glyphStorage[x + 2] = glyphStorage[x];
            ix = glyphStorage.getCharIndex(x, success);
            glyphStorage.setCharIndex(x + 2, ix, success);
            x -= 1;
        }

        glyphStorage[firstGlyph] = d;
        glyphStorage[firstGlyph + 1] = c;

        glyphStorage.setCharIndex(firstGlyph, id, success);
        glyphStorage.setCharIndex(firstGlyph + 1, ic, success);
        break;

    case irvCDxA:
        a = glyphStorage[firstGlyph];
        c = glyphStorage[lastGlyph - 1];
        d = glyphStorage[lastGlyph];
        ia = glyphStorage.getCharIndex(firstGlyph, success);
        ic = glyphStorage.getCharIndex(lastGlyph - 1, success);
        id = glyphStorage.getCharIndex(lastGlyph, success);
        x = lastGlyph - 2;

        while (x > firstGlyph) {
            glyphStorage[x + 1] = glyphStorage[x];
            ix = glyphStorage.getCharIndex(x, success);
            glyphStorage.setCharIndex(x + 1, ix, success);
            x -= 1;
        }

        glyphStorage[firstGlyph] = c;
        glyphStorage[firstGlyph + 1] = d;
        glyphStorage[lastGlyph] = a;

        glyphStorage.setCharIndex(firstGlyph, ic, success);
        glyphStorage.setCharIndex(firstGlyph + 1, id, success);
        glyphStorage.setCharIndex(lastGlyph, ia, success);
        break;

    case irvDCxA:
        a = glyphStorage[firstGlyph];
        c = glyphStorage[lastGlyph - 1];
        d = glyphStorage[lastGlyph];
        ia = glyphStorage.getCharIndex(firstGlyph, success);
        ic = glyphStorage.getCharIndex(lastGlyph - 1, success);
        id = glyphStorage.getCharIndex(lastGlyph, success);
        x = lastGlyph - 2;

        while (x > firstGlyph) {
            glyphStorage[x + 1] = glyphStorage[x];
            ix = glyphStorage.getCharIndex(x, success);
            glyphStorage.setCharIndex(x + 1, ix, success);
            x -= 1;
        }

        glyphStorage[firstGlyph] = d;
        glyphStorage[firstGlyph + 1] = c;
        glyphStorage[lastGlyph] = a;

        glyphStorage.setCharIndex(firstGlyph, id, success);
        glyphStorage.setCharIndex(firstGlyph + 1, ic, success);
        glyphStorage.setCharIndex(lastGlyph, ia, success);
        break;

    case irvDxAB:
        a = glyphStorage[firstGlyph];
        b = glyphStorage[firstGlyph + 1];
        d = glyphStorage[lastGlyph];
        ia = glyphStorage.getCharIndex(firstGlyph, success);
        ib = glyphStorage.getCharIndex(firstGlyph + 1, success);
        id = glyphStorage.getCharIndex(lastGlyph, success);
        x = firstGlyph + 2;

        while (x < lastGlyph) {
            glyphStorage[x - 2] = glyphStorage[x];
            ix = glyphStorage.getCharIndex(x, success);
            glyphStorage.setCharIndex(x - 2, ix, success);
            x += 1;
        }

        glyphStorage[firstGlyph] = d;
        glyphStorage[lastGlyph - 1] = a;
        glyphStorage[lastGlyph] = b;

        glyphStorage.setCharIndex(firstGlyph, id, success);
        glyphStorage.setCharIndex(lastGlyph - 1, ia, success);
        glyphStorage.setCharIndex(lastGlyph, ib, success);
        break;

    case irvDxBA:
        a = glyphStorage[firstGlyph];
        b = glyphStorage[firstGlyph + 1];
        d = glyphStorage[lastGlyph];
        ia = glyphStorage.getCharIndex(firstGlyph, success);
        ib = glyphStorage.getCharIndex(firstGlyph + 1, success);
        id = glyphStorage.getCharIndex(lastGlyph, success);
        x = firstGlyph + 2;

        while (x < lastGlyph) {
            glyphStorage[x - 2] = glyphStorage[x];
            ix = glyphStorage.getCharIndex(x, success);
            glyphStorage.setCharIndex(x - 2, ix, success);
            x += 1;
        }

        glyphStorage[firstGlyph] = d;
        glyphStorage[lastGlyph - 1] = b;
        glyphStorage[lastGlyph] = a;

        glyphStorage.setCharIndex(firstGlyph, id, success);
        glyphStorage.setCharIndex(lastGlyph - 1, ib, success);
        glyphStorage.setCharIndex(lastGlyph, ia, success);
        break;

    case irvCDxAB:
        a = glyphStorage[firstGlyph];
        b = glyphStorage[firstGlyph + 1];

        glyphStorage[firstGlyph] = glyphStorage[lastGlyph - 1];
        glyphStorage[firstGlyph + 1] = glyphStorage[lastGlyph];

        glyphStorage[lastGlyph - 1] = a;
        glyphStorage[lastGlyph] = b;

        ia = glyphStorage.getCharIndex(firstGlyph, success);
        ib = glyphStorage.getCharIndex(firstGlyph + 1, success);
        ic = glyphStorage.getCharIndex(lastGlyph - 1, success);
        id = glyphStorage.getCharIndex(lastGlyph, success);

        glyphStorage.setCharIndex(firstGlyph, ic, success);
        glyphStorage.setCharIndex(firstGlyph + 1, id, success);

        glyphStorage.setCharIndex(lastGlyph - 1, ia, success);
        glyphStorage.setCharIndex(lastGlyph, ib, success);
        break;

    case irvCDxBA:
        a = glyphStorage[firstGlyph];
        b = glyphStorage[firstGlyph + 1];

        glyphStorage[firstGlyph] = glyphStorage[lastGlyph - 1];
        glyphStorage[firstGlyph + 1] = glyphStorage[lastGlyph];

        glyphStorage[lastGlyph - 1] = b;
        glyphStorage[lastGlyph] = a;

        ia = glyphStorage.getCharIndex(firstGlyph, success);
        ib = glyphStorage.getCharIndex(firstGlyph + 1, success);
        ic = glyphStorage.getCharIndex(lastGlyph - 1, success);
        id = glyphStorage.getCharIndex(lastGlyph, success);

        glyphStorage.setCharIndex(firstGlyph, ic, success);
        glyphStorage.setCharIndex(firstGlyph + 1, id, success);

        glyphStorage.setCharIndex(lastGlyph - 1, ib, success);
        glyphStorage.setCharIndex(lastGlyph, ia, success);
        break;

    case irvDCxAB:
        a = glyphStorage[firstGlyph];
        b = glyphStorage[firstGlyph + 1];

        glyphStorage[firstGlyph] = glyphStorage[lastGlyph];
        glyphStorage[firstGlyph + 1] = glyphStorage[lastGlyph - 1];

        glyphStorage[lastGlyph - 1] = a;
        glyphStorage[lastGlyph] = b;

        ia = glyphStorage.getCharIndex(firstGlyph, success);
        ib = glyphStorage.getCharIndex(firstGlyph + 1, success);
        ic = glyphStorage.getCharIndex(lastGlyph - 1, success);
        id = glyphStorage.getCharIndex(lastGlyph, success);

        glyphStorage.setCharIndex(firstGlyph, id, success);
        glyphStorage.setCharIndex(firstGlyph + 1, ic, success);

        glyphStorage.setCharIndex(lastGlyph - 1, ia, success);
        glyphStorage.setCharIndex(lastGlyph, ib, success);
        break;

    case irvDCxBA:
        a = glyphStorage[firstGlyph];
        b = glyphStorage[firstGlyph + 1];

        glyphStorage[firstGlyph] = glyphStorage[lastGlyph];
        glyphStorage[firstGlyph + 1] = glyphStorage[lastGlyph - 1];

        glyphStorage[lastGlyph - 1] = b;
        glyphStorage[lastGlyph] = a;

        ia = glyphStorage.getCharIndex(firstGlyph, success);
        ib = glyphStorage.getCharIndex(firstGlyph + 1, success);
        ic = glyphStorage.getCharIndex(lastGlyph - 1, success);
        id = glyphStorage.getCharIndex(lastGlyph, success);

        glyphStorage.setCharIndex(firstGlyph, id, success);
        glyphStorage.setCharIndex(firstGlyph + 1, ic, success);

        glyphStorage.setCharIndex(lastGlyph - 1, ib, success);
        glyphStorage.setCharIndex(lastGlyph, ia, success);
        break;

    default:
        break;
    }
}

U_NAMESPACE_END
