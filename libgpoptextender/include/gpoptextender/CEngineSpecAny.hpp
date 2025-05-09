#pragma once

#include "gpos/base.h"

#include "gpoptextender/CEngineSpec.hpp"

namespace gpopt {
using namespace gpos;
using namespace gpmd;

class CEngineSpecAny : public CEngineSpec
{
public:
    // Ctor
    CEngineSpecAny()
    {
    }

    // Dtor
    virtual ~CEngineSpecAny()
    {
    }

    // Engine type accessor
    virtual EEngineType Eet() const
    {
        return EetAny;
    }

    // Identifier
    virtual const CHAR *SzId() const
    {
        return "ANY";
    }
};

}