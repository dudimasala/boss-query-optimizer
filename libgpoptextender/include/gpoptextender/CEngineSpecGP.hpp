#pragma once

#include "gpos/base.h"

#include "gpoptextender/CEngineSpec.hpp"

namespace gpopt {
using namespace gpos;
using namespace gpmd;

class CEngineSpecGP : public CEngineSpec
{
public:
    // Ctor
    CEngineSpecGP()
    {
    }

    // Dtor
    virtual ~CEngineSpecGP()
    {
    }

    // Engine type accessor
    virtual EEngineType Eet() const
    {
        return EetCPU;
    }

    // Identifier
    virtual const CHAR *SzId() const
    {
        return "GP";
    }
};

}