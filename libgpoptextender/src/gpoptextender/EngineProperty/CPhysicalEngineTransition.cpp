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
	return GPOS_NEW(mp) CEngineSpec(CEngineSpec::EetAny);
}
















