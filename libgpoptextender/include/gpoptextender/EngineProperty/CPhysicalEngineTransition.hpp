#pragma once

#include "gpos/base.h"

#include "gpopt/operators/CPhysical.h"
#include "gpoptextender/GenericOps/CBasePhysicalUnaryOp.hpp"
#include "gpoptextender/EngineProperty/CEngineSpec.hpp"
#include "gpoptextender/EngineProperty/CEnfdEngine.hpp"
#include "gpopt/base/COrderSpec.h"
#include "gpopt/base/CDistributionSpec.h"
#include "gpopt/base/CDistributionSpecAny.h"
#include "gpopt/base/CDistributionSpecRandom.h"

namespace gpopt {
using namespace gpos;
using namespace gpmd;

// Operator to handle engine transitions when needed
class CPhysicalEngineTransition : public CBasePhysicalUnaryOp
{
private:
    // Target engine
    CEngineSpec *m_pes;
    bool m_preserve_order;
    bool m_preserve_distribution;

public:
    // Ctor
    CPhysicalEngineTransition(CMemoryPool *mp, CEngineSpec *pes, bool preserve_order, bool preserve_distribution)
        : CBasePhysicalUnaryOp(mp),
          m_pes(pes),
          m_preserve_order(preserve_order),
          m_preserve_distribution(preserve_distribution)
    {
    }

	// dtor
	virtual ~CPhysicalEngineTransition(){
		m_pes->Release();
	};

    // ident accessors
	virtual EOperatorId
	Eopid() const
	{
		return EopPhysicalEngineTransform;
	}

    	// return a string for operator name
	virtual const CHAR *
	SzId() const
	{
		return "CPhysicalEngineTransition";
	}

    // match function
	BOOL Matches(COperator *pop) const;

	CEngineSpec::EEngineType Eet() const {
		return m_pes->Eet();
	}

    // Engine spec accessor
    CEngineSpec *PesSpec() const
    {
			return m_pes;
    }


		BOOL FInputOrderSensitive() const {
			return false;
		}

	virtual CEnfdProp::EPropEnforcingType EpetEngine(
		CExpressionHandle &exprhdl, const CEnfdEngine *pee) const;
    
	// debug print
	virtual IOstream &OsPrint(IOstream &os) const;

	// conversion function
	static CPhysicalEngineTransition *
	PopConvert(COperator *pop)
	{
		GPOS_ASSERT(NULL != pop);
		GPOS_ASSERT(EopPhysicalEngineTransform == pop->Eopid());

		return dynamic_cast<CPhysicalEngineTransition *>(pop);
	}

	virtual CEngineSpec* PesDerive(CMemoryPool *mp, CExpressionHandle &exprhdl) const;

	virtual CEngineSpec* PesRequired(CMemoryPool *mp, CExpressionHandle &exprhdl, CEngineSpec *pesRequired, ULONG child_index, CDrvdPropArray *pdrgpdpCtxt, ULONG ulOptReq) const;  

	virtual COrderSpec* PosRequired(CMemoryPool *mp, CExpressionHandle &exprhdl,
									COrderSpec *posRequired, ULONG child_index,
									CDrvdPropArray *pdrgpdpCtxt,
									ULONG ulOptReq) const
{
	return GPOS_NEW(mp) COrderSpec(mp);
}

	virtual COrderSpec* PosDerive(CMemoryPool *mp, CExpressionHandle &exprhdl) const
{
	if (m_preserve_order) {
		return PosDerivePassThruOuter(exprhdl);
	}
	// Return empty order spec to indicate no order is preserved
	return GPOS_NEW(mp) COrderSpec(mp);
}

virtual CDistributionSpec* PdsDerive(CMemoryPool *mp, CExpressionHandle &exprhdl) const
{
	// Return singleton distribution to indicate this is a property-breaking operator
	if (m_preserve_distribution) {
		return PdsDerivePassThruOuter(exprhdl);
	}
	return GPOS_NEW(mp) CDistributionSpecSingleton();
}

virtual CDistributionSpec* PdsRequired(CMemoryPool *mp, CExpressionHandle &exprhdl, CDistributionSpec *pdsRequired, ULONG child_index, CDrvdPropArray *pdrgpdpCtxt, ULONG ulOptReq) const
{
	// Return ANY distribution since we don't care what distribution the child has
	return GPOS_NEW(mp) CDistributionSpecAny(Eopid());
}

virtual CEnfdProp::EPropEnforcingType EpetDistribution(CExpressionHandle &exprhdl, const CEnfdDistribution *ped) const;

virtual CEnfdProp::EPropEnforcingType EpetOrder(CExpressionHandle &exprhdl, const CEnfdOrder *peo) const;

};

}