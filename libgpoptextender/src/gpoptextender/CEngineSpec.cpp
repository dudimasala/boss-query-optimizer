#include "gpoptextender/CEngineSpec.hpp"

namespace gpopt {

BOOL CEngineSpec::FSatisfies(const CEngineSpec *pes) const
{
    if (Eet() == pes->Eet() || pes->Eet() == EetAny)
    {
      return true;
    }
    return false;
}

void CEngineSpec::AppendEnforcers(CMemoryPool *mp, CExpressionHandle &exprhdl,
                                  CReqdPropPlan *prpp,
                                  CExpressionArray *pdrgpexpr,
                                  CExpression *pexpr)
{
    // TODO: Implement
} 

} // namespace gpopt
