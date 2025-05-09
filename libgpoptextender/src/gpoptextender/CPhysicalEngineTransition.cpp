#include "gpoptextender/CPhysicalEngineTransition.hpp"

using namespace gpopt;
CEnfdProp::EPropEnforcingType
    CPhysicalEngineTransition::EpetEngine(CExpressionHandle &exprhdl, const CEnfdEngine *pee) const
    {
        GPOS_ASSERT(NULL != pee);
        
        if (pee->FCompatible(m_pes_target))
        {
            // This transition provides the required engine
            return CEnfdProp::EpetUnnecessary;
        }
        
        // Cannot provide required engine
        return CEnfdProp::EpetProhibited;
    }