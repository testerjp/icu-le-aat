#ifndef __KERNSTATETABLES_H
#define __KERNSTATETABLES_H

/**
 * \file
 * \internal
 */

#include "LETypes.h"
#include "KernTables.h"
#include "StateTables.h"

U_NAMESPACE_BEGIN

struct KernStateTableHeader : KernSubtableHeader2
{
    StateTableHeader stHeader;
};

U_NAMESPACE_END

#endif // __KERNSTATETABLES_H
