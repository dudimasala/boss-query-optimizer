#pragma once

#include "gpos/base.h"
#include "gpos/utils.h"

namespace gpopt {
using namespace gpos;

extern bool useMaxCosting;

enum EEngineType
  {
      EetGP,
      EetAny,  
      EetSentinel,
      EetDynamicStart = EetSentinel,
      EetDynamicEnd = 10000
  };

// Static hash function for EEngineType - for use in hash maps
static ULONG EngineTypeHashValue(const EEngineType *eet)
{
    GPOS_ASSERT(NULL != eet);
    ULONG ulEet = (ULONG) *eet;
    return gpos::HashValue<ULONG>(&ulEet);
}

// Static equality function for EEngineType - for use in hash maps
static BOOL EngineTypeEquals(const EEngineType *eet1, const EEngineType *eet2)
{
    GPOS_ASSERT(NULL != eet1);
    GPOS_ASSERT(NULL != eet2);
    return *eet1 == *eet2;
}
}