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
		bool m_preserve_rewindability;

public:
    // Ctor
    CPhysicalEngineTransition(CMemoryPool *mp, CEngineSpec *pes, bool preserve_order, bool preserve_distribution, bool preserve_rewindability)
        : CBasePhysicalUnaryOp(mp),
          m_pes(pes),
          m_preserve_order(preserve_order),
          m_preserve_distribution(preserve_distribution),
					m_preserve_rewindability(preserve_rewindability)
    {
    }

	// dtor
	virtual ~CPhysicalEngineTransition(){
		m_pes->Release();
	};

    // ident accessors
	virtual EOperatorId
	Eopid() const override
	{
		return EopPhysicalEngineTransform;
	}

    	// return a string for operator name
	virtual const CHAR *
	SzId() const override
	{
		return "CPhysicalEngineTransition";
	}

    // match function
	BOOL Matches(COperator *pop) const override;

	CEngineSpec::EEngineType Eet() const {
		return m_pes->Eet();
	}

    // Engine spec accessor
    CEngineSpec *PesSpec() const
    {
			return m_pes;
    }


		BOOL FInputOrderSensitive() const override {
			return false;
		}

	virtual CEnfdProp::EPropEnforcingType EpetEngine(
		CExpressionHandle &exprhdl, const CEnfdEngine *pee) const override;
    
	// debug print
	virtual IOstream &OsPrint(IOstream &os) const override;

	// conversion function
	static CPhysicalEngineTransition *
	PopConvert(COperator *pop)
	{
		GPOS_ASSERT(NULL != pop);
		GPOS_ASSERT(EopPhysicalEngineTransform == pop->Eopid());

		return dynamic_cast<CPhysicalEngineTransition *>(pop);
	}

	virtual CEngineSpec* PesDerive(CMemoryPool *mp, CExpressionHandle &exprhdl) const override;

	virtual CEngineSpec* PesRequired(CMemoryPool *mp, CExpressionHandle &exprhdl, CEngineSpec *pesRequired, ULONG child_index, CDrvdPropArray *pdrgpdpCtxt, ULONG ulOptReq) const override;  

	virtual COrderSpec* PosRequired(CMemoryPool *mp, CExpressionHandle &exprhdl,
									COrderSpec *posRequired, ULONG child_index,
									CDrvdPropArray *pdrgpdpCtxt,
									ULONG ulOptReq) const override
{
	return GPOS_NEW(mp) COrderSpec(mp);
}

	virtual COrderSpec* PosDerive(CMemoryPool *mp, CExpressionHandle &exprhdl) const override
{
	if (m_preserve_order) {
		return PosDerivePassThruOuter(exprhdl);
	}
	// Return empty order spec to indicate no order is preserved
	return GPOS_NEW(mp) COrderSpec(mp);
}

virtual CDistributionSpec* PdsDerive(CMemoryPool *mp, CExpressionHandle &exprhdl) const override
{
	// Return singleton distribution to indicate this is a property-breaking operator
	if (m_preserve_distribution) {
		return PdsDerivePassThruOuter(exprhdl);
	}
	return GPOS_NEW(mp) CDistributionSpecSingleton();
}

virtual CRewindabilitySpec* PrsDerive(CMemoryPool *mp, CExpressionHandle &exprhdl) const override;



virtual CDistributionSpec* PdsRequired(CMemoryPool *mp, CExpressionHandle &exprhdl, CDistributionSpec *pdsRequired, ULONG child_index, CDrvdPropArray *pdrgpdpCtxt, ULONG ulOptReq) const override
{
	// Return ANY distribution since we don't care what distribution the child has
	return GPOS_NEW(mp) CDistributionSpecAny(Eopid());
}

virtual CEnfdProp::EPropEnforcingType EpetDistribution(CExpressionHandle &exprhdl, const CEnfdDistribution *ped) const override;

virtual CEnfdProp::EPropEnforcingType EpetOrder(CExpressionHandle &exprhdl, const CEnfdOrder *peo) const override;

};

}