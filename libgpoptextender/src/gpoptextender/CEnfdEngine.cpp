#include "gpoptextender/CEnfdEngine.hpp"
// #include "gpopt/operators/CExpressionHandle.h"



namespace gpopt {

CEnfdEngine::CEnfdEngine(CEngineSpec *pes, EEngineMatching erm)
    : m_pes(pes), m_erm(erm)
{
    GPOS_ASSERT(NULL != pes);
}

ULONG CEnfdEngine::HashValue() const
{
    return gpos::CombineHashes(m_erm + 1, m_pes->HashValue());
}

CEnfdEngine::EPropEnforcingType CEnfdEngine::Epet(CExpressionHandle &exprhdl, CPhysical *popPhysical, BOOL fEngineReqd) const
{
    // Get engine delivered by the physical node
    // CEngineSpec *pes = CDrvdPropPlan::Pdpplan(exprhdl.Pdp())->Pes();
    
    //     if (FCompatible(pes))
    //     {
    //         // Required engine is already provided
    //         return CEnfdProp::EpetUnnecessary;
    //     }
        
    // Required engine will be enforced
    return CEnfdProp::EpetRequired;
}

}