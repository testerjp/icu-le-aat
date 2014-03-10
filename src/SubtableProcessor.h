/*
 *
 * (C) Copyright IBM Corp. 1998-2013 - All Rights Reserved
 *
 */

#ifndef __SUBTABLEPROCESSOR_H
#define __SUBTABLEPROCESSOR_H

/**
 * \file
 * \internal
 */

#include "LETypes.h"

U_NAMESPACE_BEGIN

class LEGlyphStorage;

class SubtableProcessor : public UMemory {
public:
    virtual void process(LEGlyphStorage &glyphStorage, LEErrorCode &success) = 0;
    virtual ~SubtableProcessor();

protected:
    SubtableProcessor();

private:
    SubtableProcessor(const SubtableProcessor &other); // forbid copying of this class
    SubtableProcessor &operator=(const SubtableProcessor &other); // forbid copying of this class
};

U_NAMESPACE_END

#endif
