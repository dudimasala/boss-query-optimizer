#include "gpoptextender/CPhysicalGP.hpp"

using namespace gpopt;

CPhysicalGP::CPhysicalGP(CMemoryPool *mp) : CPhysical(mp) {}

CPhysicalGP::~CPhysicalGP() {}

CEngineSpec *CPhysicalGP::PesRequired(CMemoryPool *mp,
									  CExpressionHandle &exprhdl,
									  CEngineSpec *pesRequired,
									  ULONG child_index,
									  CDrvdPropArray *pdrgpdpCtxt,
									  ULONG ulOptReq) const
{
	return GPOS_NEW(mp) CEngineSpecGP();
}