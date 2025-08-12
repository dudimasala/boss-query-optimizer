//---------------------------------------------------------------------------
// Copyright 2025 Malhar Rajpal
// Description of engine order;
// Can be used as required or derived property;
//---------------------------------------------------------------------------


#include "gpoptextender/EngineProperty/CEngineSpec.hpp"
#include "gpoptextender/EngineProperty/CEnfdEngine.hpp"
#include "gpoptextender/EngineProperty/CPhysicalEngineTransition.hpp"
#include "gpoptextender/DynamicRegistry/IDynamicRegistry.hpp"

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

  CEngineSpec *pes = CPhysical::PopConvert(pexpr->Pop())->PesDerive(mp, exprhdl);
  orcaextender::IDynamicRegistry *registry = orcaextender::CreateDynamicRegistry();
  bool preserve_order = registry->GetEnginePreserveOrder(pes->Eet(), Eet());
  bool preserve_distribution = registry->GetEnginePreserveDistribution(pes->Eet(), Eet());
  bool preserve_rewindability = registry->GetEnginePreserveRewindability(pes->Eet(), Eet());
  pes->Release();

  CExpression *pexprEngineTransform = GPOS_NEW(mp) CExpression(
    mp, GPOS_NEW(mp) CPhysicalEngineTransition(mp, this, preserve_order, preserve_distribution, preserve_rewindability), pexpr);
  pdrgpexpr->Append(pexprEngineTransform);
} 

IOstream &CEngineSpec::OsPrint(IOstream &os) const
{
	return os << "ENGINE (" << Eet() << ")";
}

} // namespace gpopt
