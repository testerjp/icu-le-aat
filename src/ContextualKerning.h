#ifndef __CONTEXTUALKERNING_H
#define __CONTEXTUALKERNING_H

/**
 * \file
 * \internal
 */

#include "LETypes.h"
#include "LayoutTables.h"
#include "StateTables.h"

U_NAMESPACE_BEGIN

struct ContextualKerningHeader : StateTableHeader
{
    le_uint16 valueTable;
};

enum ContextualKerningFlags
{
    ckfPush            = 0x8000,
    ckfDontAdvance     = 0x4000,
    ckfValueOffsetMask = 0x3FFF,
};

struct ContextualKerningStateEntry : StateEntry
{
};

U_NAMESPACE_END

#endif // __CONTEXTUALKERNING_H
