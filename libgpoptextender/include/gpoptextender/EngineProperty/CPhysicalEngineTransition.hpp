#pragma once

#include "gpos/base.h"

#include "gpopt/operators/CPhysical.h"
#include "gpoptextender/EngineProperty/CEngineSpec.hpp"
#include "gpoptextender/EngineProperty/CEnfdEngine.hpp"

namespace gpopt {
using namespace gpos;
using namespace gpmd;

// Operator to handle engine transitions when needed
class CPhysicalEngineTransition : public CPhysical
{
private:
    // Target engine
    CEngineSpec *m_pes;

public:
    // Ctor
    CPhysicalEngineTransition(CMemoryPool *mp, CEngineSpec *pes)
        : CPhysical(mp),
          m_pes(pes)
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

    // // Derive engine spec
    // CEngineSpec *PesDerive(CMemoryPool *, CExpressionHandle &) const
    // {
    //     m_pes_target->AddRef();
    //     return m_pes_target;
    // }

		BOOL FInputOrderSensitive() const {
			return false;
		}

    //-------------------------------------------------------------------------------------
	// Required Plan Properties
	//-------------------------------------------------------------------------------------

    virtual CColRefSet *PcrsRequired(
	CMemoryPool *mp, CExpressionHandle &exprhdl, CColRefSet *pcrsRequired,
	ULONG child_index, CDrvdPropArray *pdrgpdpCtxt, ULONG ulOptReq);

    virtual CCTEReq *PcteRequired(CMemoryPool *mp, CExpressionHandle &exprhdl,
								  CCTEReq *pcter, ULONG child_index,
								  CDrvdPropArray *pdrgpdpCtxt,
								  ULONG ulOptReq) const;

                                  	// compute required sort order of the n-th child
	virtual COrderSpec *PosRequired(CMemoryPool *mp, CExpressionHandle &exprhdl,
									COrderSpec *posRequired, ULONG child_index,
									CDrvdPropArray *pdrgpdpCtxt,
									ULONG ulOptReq) const;

                                    	// compute required distribution of the n-th child
	virtual CDistributionSpec *PdsRequired(CMemoryPool *mp,
										   CExpressionHandle &exprhdl,
										   CDistributionSpec *pdsRequired,
										   ULONG child_index,
										   CDrvdPropArray *pdrgpdpCtxt,
										   ULONG ulOptReq) const;

	// compute required rewindability of the n-th child
	virtual CRewindabilitySpec *PrsRequired(CMemoryPool *mp,
											CExpressionHandle &exprhdl,
											CRewindabilitySpec *prsRequired,
											ULONG child_index,
											CDrvdPropArray *pdrgpdpCtxt,
											ULONG ulOptReq) const;

                                            	// compute required partition propagation of the n-th child
	virtual CPartitionPropagationSpec *PppsRequired(
		CMemoryPool *mp, CExpressionHandle &exprhdl,
		CPartitionPropagationSpec *pppsRequired, ULONG child_index,
		CDrvdPropArray *pdrgpdpCtxt, ULONG ulOptReq);

    // required properties: check if required columns are included in output columns
	virtual BOOL FProvidesReqdCols(CExpressionHandle &exprhdl,
								   CColRefSet *pcrsRequired,
								   ULONG ulOptReq) const;

                                   	// derive sort order
	virtual COrderSpec *PosDerive(CMemoryPool *mp,
								  CExpressionHandle &exprhdl) const;

                                  	// dderive distribution
	virtual CDistributionSpec *PdsDerive(CMemoryPool *mp,
										 CExpressionHandle &exprhdl) const;

	// derived properties: derive rewindability
	virtual CRewindabilitySpec *PrsDerive(CMemoryPool *mp,
										  CExpressionHandle &exprhdl) const;

	// derive partition index map
	virtual CPartIndexMap *
	PpimDerive(CMemoryPool *,  // mp
			   CExpressionHandle &exprhdl,
			   CDrvdPropCtxt *	//pdpctxt
	) const
	{
		return PpimPassThruOuter(exprhdl);
	}

	// derive partition filter map
	virtual CPartFilterMap *
	PpfmDerive(CMemoryPool *,  // mp
			   CExpressionHandle &exprhdl) const
	{
		return PpfmPassThruOuter(exprhdl);
	}

    // return order property enforcing type for this operator
	virtual CEnfdProp::EPropEnforcingType EpetOrder(
		CExpressionHandle &exprhdl, const CEnfdOrder *peo) const;

    // return rewindability property enforcing type for this operator
	virtual CEnfdProp::EPropEnforcingType EpetRewindability(
		CExpressionHandle &exprhdl, const CEnfdRewindability *per) const;

	virtual CEnfdProp::EPropEnforcingType EpetEngine(
		CExpressionHandle &exprhdl, const CEnfdEngine *pee) const;


	virtual CEnfdProp::EPropEnforcingType EpetDistribution(
		CExpressionHandle &exprhdl, const CEnfdDistribution *ped) const;

	// return true if operator passes through stats obtained from children,
	// this is used when computing stats during costing
	virtual BOOL
	FPassThruStats() const
	{
		return true;
	}

    
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


    
};

}