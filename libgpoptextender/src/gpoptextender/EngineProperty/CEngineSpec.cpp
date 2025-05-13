#include "gpoptextender/EngineProperty/CEngineSpec.hpp"
#include "gpoptextender/EngineProperty/CEnfdEngine.hpp"
#include "gpoptextender/EngineProperty/CPhysicalEngineTransition.hpp"

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
  GPOS_ASSERT(NULL != mp);
	GPOS_ASSERT(NULL != prpp);
	GPOS_ASSERT(NULL != pdrgpexpr);
	GPOS_ASSERT(NULL != pexpr);
	GPOS_ASSERT(!GPOS_FTRACE(EopttraceDisableEngineEnforcement));
	GPOS_ASSERT(
		this == prpp->Pee()->PesRequired() &&
		"required plan properties don't match enforced engine spec");

  AddRef();
  pexpr->AddRef();
  CExpression *pexprEngineTransform = GPOS_NEW(mp) CExpression(
    mp, GPOS_NEW(mp) CPhysicalEngineTransition(mp, this), pexpr);
  pdrgpexpr->Append(pexprEngineTransform);
} 

IOstream &CEngineSpec::OsPrint(IOstream &os) const
{
	return os << "ENGINE (" << Eet() << ")";
}

} // namespace gpopt
