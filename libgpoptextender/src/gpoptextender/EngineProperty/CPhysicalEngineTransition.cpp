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

CColRefSet *CPhysicalEngineTransition::PcrsRequired(CMemoryPool *mp, CExpressionHandle &exprhdl,
							CColRefSet *pcrsRequired, ULONG child_index,
							CDrvdPropArray *,  // pdrgpdpCtxt
							ULONG			   // ulOptReq
)
{
	GPOS_ASSERT(
		0 == child_index &&
		"Required properties can only be computed on the relational child");

	return PcrsChildReqd(mp, exprhdl, pcrsRequired, child_index,
						 gpos::ulong_max /*ulScalarIndex*/);
}

COrderSpec* CPhysicalEngineTransition::PosRequired(CMemoryPool *mp, CExpressionHandle &exprhdl,
									COrderSpec *posRequired, ULONG child_index,
									CDrvdPropArray *pdrgpdpCtxt,
									ULONG ulOptReq) const
{
	return PosPassThru(mp, exprhdl, posRequired, child_index);
}

CDistributionSpec *CPhysicalEngineTransition::PdsRequired(CMemoryPool *mp,
										   CExpressionHandle &exprhdl,
										   CDistributionSpec *pdsRequired,
										   ULONG child_index,
										   CDrvdPropArray *pdrgpdpCtxt,
										   ULONG ulOptReq) const
{
	GPOS_ASSERT(0 == child_index);

	return PdsPassThru(mp, exprhdl, pdsRequired, child_index);
}

CRewindabilitySpec *
CPhysicalEngineTransition::PrsRequired(CMemoryPool *mp, CExpressionHandle &exprhdl,
						   CRewindabilitySpec *prsRequired,  //prsRequired,
						   ULONG child_index,
						   CDrvdPropArray *,  // pdrgpdpCtxt
						   ULONG			  // ulOptReq
) const
{
    GPOS_ASSERT(0 == child_index);
		return PrsPassThru(mp, exprhdl, prsRequired, child_index);
    if (exprhdl.HasOuterRefs(0))
    {
        return GPOS_NEW(mp) CRewindabilitySpec(CRewindabilitySpec::ErtRescannable, CRewindabilitySpec::EmhtNoMotion);
    }
    else
    {
        return GPOS_NEW(mp) CRewindabilitySpec(CRewindabilitySpec::ErtNone, CRewindabilitySpec::EmhtNoMotion);
    }
}

CPartitionPropagationSpec *
CPhysicalEngineTransition::PppsRequired(CMemoryPool *mp, CExpressionHandle &exprhdl,
						   CPartitionPropagationSpec *pppsRequired,  //ppRequired,
						   ULONG
#ifdef GPOS_DEBUG
							   child_index
#endif	// GPOPS_DEBUG
						   ,
						   CDrvdPropArray *,  // pdrgpdpCtxt
						   ULONG)			  // ulOptReq) const    
{
	GPOS_ASSERT(0 == child_index);
	GPOS_ASSERT(NULL != pppsRequired);

	return CPhysical::PppsRequiredPushThruUnresolvedUnary(
		mp, exprhdl, pppsRequired, CPhysical::EppcAllowed, NULL);
}

CCTEReq *CPhysicalEngineTransition::PcteRequired(CMemoryPool *mp, CExpressionHandle &exprhdl,
							CCTEReq *pcter, ULONG child_index,
							CDrvdPropArray *pdrgpdpCtxt,
							ULONG ulOptReq) const
{
	GPOS_ASSERT(0 == child_index);
	return PcterPushThru(pcter);
}

BOOL CPhysicalEngineTransition::FProvidesReqdCols(CExpressionHandle &exprhdl,
												  CColRefSet *pcrsRequired,
												  ULONG ulOptReq) const
{
	return FUnaryProvidesReqdCols(exprhdl, pcrsRequired);
}

COrderSpec *CPhysicalEngineTransition::PosDerive(CMemoryPool *mp,
												CExpressionHandle &exprhdl) const
{
    return PosDerivePassThruOuter(exprhdl);
}

CDistributionSpec *CPhysicalEngineTransition::PdsDerive(CMemoryPool *mp,
												CExpressionHandle &exprhdl) const
{
    return PdsDerivePassThruOuter(exprhdl);
}

CRewindabilitySpec *CPhysicalEngineTransition::PrsDerive(CMemoryPool *mp,
												CExpressionHandle &exprhdl) const
{
    return PrsDerivePassThruOuter(mp, exprhdl);
}

CEnfdProp::EPropEnforcingType CPhysicalEngineTransition::EpetOrder(CExpressionHandle &exprhdl, const CEnfdOrder *peo) const
{
    return CEnfdProp::EpetUnnecessary;
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

CEnfdProp::EPropEnforcingType CPhysicalEngineTransition::EpetRewindability(CExpressionHandle &exprhdl, const CEnfdRewindability *per) const
{
    return CEnfdProp::EpetUnnecessary;
}

CEnfdProp::EPropEnforcingType CPhysicalEngineTransition::EpetDistribution(CExpressionHandle &exprhdl, const CEnfdDistribution *ped) const
{
    return CEnfdProp::EpetUnnecessary;
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
















