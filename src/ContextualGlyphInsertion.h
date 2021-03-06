/**
 *
 * (C) Copyright IBM Corp. and Others 1998-2013 - All Rights Reserved
 *
 */

#ifndef __CONTEXTUALGLYPHINSERTION_H
#define __CONTEXTUALGLYPHINSERTION_H

/**
 * \file
 * \internal
 */

#include "LETypes.h"
#include "LayoutTables.h"
#include "StateTables.h"

U_NAMESPACE_BEGIN

struct ContextualGlyphInsertionHeader : StateTableHeader
{
};

struct ContextualGlyphInsertionHeader2 : StateTableHeader2
{
    le_uint32 insertionActionOffset;
};

enum ContextualGlyphInsertionFlags
{
    cgiSetMark                  = 0x8000,
    cgiDontAdvance              = 0x4000,
    cgiCurrentIsKashidaLike     = 0x2000,
    cgiMarkedIsKashidaLike      = 0x1000,
    cgiCurrentInsertBefore      = 0x0800,
    cgiMarkInsertBefore         = 0x0400,
    cgiCurrentInsertCountMask   = 0x03E0,
    cgiMarkedInsertCountMask    = 0x001F,
    cgiCurrentInsertCountShift  = 5,
    cgiMarkedInsertCountShift   = 0,
};

struct ContextualGlyphInsertionStateEntry : StateEntry
{
    le_uint16 currentInsertListOffset;
    le_uint16 markedInsertListOffset;
};

struct ContextualGlyphInsertionStateEntry2 : StateEntry2
{
    le_uint16 currentInsertIndex;
    le_uint16 markedInsertIndex;
};

U_NAMESPACE_END

#endif
