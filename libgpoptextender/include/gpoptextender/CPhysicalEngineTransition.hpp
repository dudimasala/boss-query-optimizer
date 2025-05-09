#pragma once

#include "gpos/base.h"

#include "gpopt/operators/CPhysical.h"
#include "gpoptextender/CEngineSpec.hpp"
#include "gpoptextender/CEnfdEngine.hpp"

namespace gpopt {
using namespace gpos;
using namespace gpmd;

// Operator to handle engine transitions when needed
class CPhysicalEngineTransition : public CPhysical
{
private:
    // Target engine
    CEngineSpec *m_pes_target;

public:
    // Ctor
    CPhysicalEngineTransition(CMemoryPool *mp, CEngineSpec *pes_target)
        : CPhysical(mp),
          m_pes_target(pes_target)
    {
        GPOS_ASSERT(NULL != pes_target);
        m_pes_target->AddRef();
    }

    // Dtor
    virtual ~CPhysicalEngineTransition()
    {
        CRefCount::SafeRelease(m_pes_target);
    }

    // Engine property enforcing function
    virtual CEnfdProp::EPropEnforcingType
    EpetEngine(CExpressionHandle &exprhdl, const CEnfdEngine *pee) const;

    // Engine spec accessor
    CEngineSpec *PesSpec() const
    {
        return m_pes_target;
    }

    // Derive engine spec
    CEngineSpec *PesDerive(CMemoryPool *, CExpressionHandle &) const
    {
        m_pes_target->AddRef();
        return m_pes_target;
    }
};

}