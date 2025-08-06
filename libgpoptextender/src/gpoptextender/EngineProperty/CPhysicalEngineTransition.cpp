#include "gpoptextender/EngineProperty/CPhysicalEngineTransition.hpp"
#include "gpopt/base/CDistributionSpecAny.h"
#include "gpopt/operators/CExpressionHandle.h"
using namespace gpopt;

BOOL CPhysicalEngineTransition::Matches(COperator *pop) const
{
    if (Eopid() != pop->Eopid())
    {
        return false;
    }

	CPhysicalEngineTransition *popEngineTransition = CPhysicalEngineTransition::PopConvert(pop);
	return Eet() == popEngineTransition->Eet();
}

CEnfdProp::EPropEnforcingType CPhysicalEngineTransition::EpetEngine(CExpressionHandle &exprhdl, const CEnfdEngine *pee) const
{
	GPOS_ASSERT(NULL != pee);

	if (pee->FCompatible(PesSpec()))
	{
		return CEnfdProp::EpetUnnecessary;
	}
	return CEnfdProp::EpetProhibited;
}

//---------------------------------------------------------------------------
//	@function:
//		CPhysicalSort::OsPrint
//
//	@doc:
//		Debug print
//
//---------------------------------------------------------------------------
IOstream &
CPhysicalEngineTransition::OsPrint(IOstream &os) const
{
	os << SzId() << "  ";
	return os << Eet();
}


CEngineSpec* CPhysicalEngineTransition::PesDerive(CMemoryPool *mp, CExpressionHandle &exprhdl) const
{
	CEngineSpec *pes = PesSpec();
	pes->AddRef();
	return pes;
}

CEngineSpec* CPhysicalEngineTransition::PesRequired(CMemoryPool *mp, CExpressionHandle &exprhdl, CEngineSpec *pesRequired, ULONG child_index, CDrvdPropArray *pdrgpdpCtxt, ULONG ulOptReq) const
{
	return GPOS_NEW(mp) CEngineSpec(EetAny);
}


CEnfdProp::EPropEnforcingType CPhysicalEngineTransition::EpetOrder(CExpressionHandle &exprhdl, const CEnfdOrder *peo) const
{
	// get order delivered by the limit node
	COrderSpec *pos = CDrvdPropPlan::Pdpplan(exprhdl.Pdp())->Pos();

	if (peo->FCompatible(pos))
	{
		return CEnfdProp::EpetUnnecessary;
	}

	return CEnfdProp::EpetRequired;
}

CEnfdProp::EPropEnforcingType CPhysicalEngineTransition::EpetDistribution(CExpressionHandle &exprhdl, const CEnfdDistribution *ped) const
{
		// get distribution delivered by the limit node
	CDistributionSpec *pds = CDrvdPropPlan::Pdpplan(exprhdl.Pdp())->Pds();

	if (ped->FCompatible(pds))
	{
		return CEnfdProp::EpetUnnecessary;
	}

	return CEnfdProp::EpetRequired;
}


CRewindabilitySpec* CPhysicalEngineTransition::PrsDerive(CMemoryPool *mp, CExpressionHandle &exprhdl) const
{
	// Return singleton distribution to indicate this is a property-breaking operator
	if (m_preserve_rewindability) {
		return PrsDerivePassThruOuter(mp, exprhdl);
	}
	CRewindabilitySpec *prs = exprhdl.Pdpplan(0 /*child_index*/)->Prs();
	return GPOS_NEW(mp) CRewindabilitySpec(CRewindabilitySpec::ErtNone, prs->Emht());
}














