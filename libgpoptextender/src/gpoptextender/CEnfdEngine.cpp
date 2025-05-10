#include "gpoptextender/CEnfdEngine.hpp"
// #include "gpopt/operators/CExpressionHandle.h"



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
        
        return CEnfdProp::EpetRequired;
    }

    return CEnfdProp::EpetUnnecessary;
}
}