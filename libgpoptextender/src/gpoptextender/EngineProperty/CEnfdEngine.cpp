#include "gpoptextender/EngineProperty/CEnfdEngine.hpp"
// #include "gpopt/operators/CExpressionHandle.h"
#include "gpopt/operators/CPhysical.h"



namespace gpopt {

CEnfdEngine::CEnfdEngine(CEngineSpec *pes, EEngineMatching eem)
    : m_pes(pes), m_eem(eem)
{
    
    GPOS_ASSERT(NULL != pes);
}

ULONG CEnfdEngine::HashValue() const
{
    return gpos::CombineHashes(m_eem + 1, m_pes->HashValue());
}

CEnfdEngine::EPropEnforcingType CEnfdEngine::Epet(CExpressionHandle &exprhdl, CPhysical *popPhysical, BOOL fEngineReqd) const
{
    if (fEngineReqd)
    {
		return popPhysical->EpetEngine(exprhdl, this);
    }

    return CEnfdProp::EpetUnnecessary;
}

}