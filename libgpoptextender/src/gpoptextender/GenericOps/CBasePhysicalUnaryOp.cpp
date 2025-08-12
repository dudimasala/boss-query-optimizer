//---------------------------------------------------------------------------
// Copyright 2025 Malhar Rajpal
// Base physical operator with standard properties to make it easy to create
// a new physical operator. Passes thru properties.
//---------------------------------------------------------------------------


#include "gpoptextender/GenericOps/CBasePhysicalUnaryOp.hpp"
#include "gpopt/base/CDistributionSpecAny.h"
#include "gpopt/operators/CExpressionHandle.h"
using namespace gpopt;

CColRefSet *CBasePhysicalUnaryOp::PcrsRequired(CMemoryPool *mp, CExpressionHandle &exprhdl,
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

COrderSpec* CBasePhysicalUnaryOp::PosRequired(CMemoryPool *mp, CExpressionHandle &exprhdl,
									COrderSpec *posRequired, ULONG child_index,
									CDrvdPropArray *pdrgpdpCtxt,
									ULONG ulOptReq) const
{
	return PosPassThru(mp, exprhdl, posRequired, child_index);
}

CDistributionSpec *CBasePhysicalUnaryOp::PdsRequired(CMemoryPool *mp,
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
CBasePhysicalUnaryOp::PrsRequired(CMemoryPool *mp, CExpressionHandle &exprhdl,
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
CBasePhysicalUnaryOp::PppsRequired(CMemoryPool *mp, CExpressionHandle &exprhdl,
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

CCTEReq *CBasePhysicalUnaryOp::PcteRequired(CMemoryPool *mp, CExpressionHandle &exprhdl,
							CCTEReq *pcter, ULONG child_index,
							CDrvdPropArray *pdrgpdpCtxt,
							ULONG ulOptReq) const
{
	GPOS_ASSERT(0 == child_index);
	return PcterPushThru(pcter);
}

BOOL CBasePhysicalUnaryOp::FProvidesReqdCols(CExpressionHandle &exprhdl,
												  CColRefSet *pcrsRequired,
												  ULONG ulOptReq) const
{
	return FUnaryProvidesReqdCols(exprhdl, pcrsRequired);
}

COrderSpec *CBasePhysicalUnaryOp::PosDerive(CMemoryPool *mp,
												CExpressionHandle &exprhdl) const
{
    return PosDerivePassThruOuter(exprhdl);
}

CDistributionSpec *CBasePhysicalUnaryOp::PdsDerive(CMemoryPool *mp,
												CExpressionHandle &exprhdl) const
{
    return PdsDerivePassThruOuter(exprhdl);
}

CRewindabilitySpec *CBasePhysicalUnaryOp::PrsDerive(CMemoryPool *mp,
												CExpressionHandle &exprhdl) const
{
    return PrsDerivePassThruOuter(mp, exprhdl);
}

CEnfdProp::EPropEnforcingType CBasePhysicalUnaryOp::EpetOrder(CExpressionHandle &exprhdl, const CEnfdOrder *peo) const
{
    return CEnfdProp::EpetUnnecessary;
}

CEnfdProp::EPropEnforcingType CBasePhysicalUnaryOp::EpetRewindability(CExpressionHandle &exprhdl, const CEnfdRewindability *per) const
{
    return CEnfdProp::EpetUnnecessary;
}

CEnfdProp::EPropEnforcingType CBasePhysicalUnaryOp::EpetDistribution(CExpressionHandle &exprhdl, const CEnfdDistribution *ped) const
{
    return CEnfdProp::EpetUnnecessary;
}
