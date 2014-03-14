/*
 *
 * (C) Copyright IBM Corp. and others 1998-2013 - All Rights Reserved
 *
 */

#ifndef __CONTEXTUALGLYPHSUBSTITUTION_H
#define __CONTEXTUALGLYPHSUBSTITUTION_H

/**
 * \file
 * \internal
 */

#include "LETypes.h"
#include "LayoutTables.h"
#include "StateTables.h"

U_NAMESPACE_BEGIN

struct ContextualGlyphSubstitutionHeader : StateTableHeader
{
    le_uint16 substitutionTableOffset;
};

struct ContextualGlyphSubstitutionHeader2 : StateTableHeader2
{
    le_uint32 substitutionTableOffset;
};

enum ContextualGlyphSubstitutionFlags
{
    cgsSetMark      = 0x8000,
    cgsDontAdvance  = 0x4000,
    cgsReserved     = 0x3FFF
};

struct ContextualGlyphSubstitutionStateEntry : StateEntry
{
    le_uint16 markOffset;
    le_uint16 currOffset;
};

struct ContextualGlyphSubstitutionStateEntry2 : StateEntry2
{
    le_uint16 markIndex;
    le_uint16 currIndex;
};

U_NAMESPACE_END

#endif
