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

CColRefSet *CEngineSpec::PcrsUsed(CMemoryPool *mp) const
{
  // we use all columns. Will implement this later
  return NULL;
}

} // namespace gpopt
