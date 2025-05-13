#pragma once

#include "gpos/base.h"

#include "gpopt/base/CEnfdProp.h"
#include "gpoptextender/EngineProperty/CEngineSpec.hpp"
#include "gpopt/base/CDrvdPropPlan.h"

namespace gpopt {
using namespace gpos;
using namespace gpmd;

// Enforceable engine property
class CEnfdEngine : public CEnfdProp
{
public:
    // type of order matching function
    enum EEngineMatching
    {
      EemSatisfy = 0,

      EemSentinel
    };
private:
    // Required engine
    CEngineSpec *m_pes;
    EEngineMatching m_eem;

public:
    // Ctor
    CEnfdEngine(CEngineSpec *pes, EEngineMatching eem);

    // Dtor
    virtual ~CEnfdEngine()
    {
        CRefCount::SafeRelease(m_pes);
    }

    // Required engine accessor
    CEngineSpec *
    PesRequired() const
    {
        return m_pes;
    }

    // Property spec accessor
    virtual CPropSpec *
    Pps() const
    {
        return m_pes;
    }

    // hash function
	virtual ULONG HashValue() const;

    // Check compatibility
    BOOL FCompatible(CEngineSpec *pes) const
    {
        return pes->FSatisfies(m_pes);
    }

    // Get engine enforcing type for the given operator
    EPropEnforcingType Epet(CExpressionHandle &exprhdl, CPhysical *popPhysical, BOOL fEngineReqd) const;

	// matching type accessor
	EEngineMatching
	Eem() const
	{
		return m_eem;
	}

	// matching function
	BOOL
	Matches(CEnfdEngine *pee)
	{
		GPOS_ASSERT(NULL != pee);

		return m_eem == pee->Eem() && m_pes->Equals(pee->PesRequired());
	}
};

}