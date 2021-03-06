/*
 *
 * (C) Copyright IBM Corp. and Others 1998-2013 - All Rights Reserved
 *
 */

#ifndef __LIGATURESUBSTITUTION_H
#define __LIGATURESUBSTITUTION_H

/**
 * \file
 * \internal
 */

#include "LETypes.h"
#include "LayoutTables.h"
#include "StateTables.h"

U_NAMESPACE_BEGIN

struct LigatureSubstitutionHeader : StateTableHeader
{
    le_uint16 ligatureActionTableOffset;
    le_uint16 componentTableOffset;
    le_uint16 ligatureTableOffset;
};

struct LigatureSubstitutionHeader2 : StateTableHeader2
{
    le_uint32 ligActionOffset;
    le_uint32 componentOffset;
    le_uint32 ligatureOffset;
};

enum LigatureSubstitutionFlags
{
    lsfSetComponent     = 0x8000,
    lsfDontAdvance      = 0x4000,
    lsfActionOffsetMask = 0x3FFF, // N/A in morx
    lsfPerformAction    = 0x2000
};

struct LigatureSubstitutionStateEntry : StateEntry
{
};

struct LigatureSubstitutionStateEntry2 : StateEntry2
{
    le_uint16 ligActionIndex;
};

typedef le_uint32 LigatureActionEntry;

enum LigatureActionFlags
{
    lafLast                 = 0x80000000,
    lafStore                = 0x40000000,
    lafComponentOffsetMask  = 0x3FFFFFFF
};

U_NAMESPACE_END

#endif
