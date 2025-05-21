#pragma once

#include "gpos/base.h"

#include "gpopt/operators/CPhysical.h"
#include "gpoptextender/GenericOps/CBasePhysicalUnaryOp.hpp"
#include "gpoptextender/EngineProperty/CEngineSpec.hpp"
#include "gpoptextender/EngineProperty/CEnfdEngine.hpp"

namespace gpopt {
using namespace gpos;
using namespace gpmd;

// Operator to handle engine transitions when needed
class CPhysicalEngineTransition : public CBasePhysicalUnaryOp
{
private:
    // Target engine
    CEngineSpec *m_pes;

public:
    // Ctor
    CPhysicalEngineTransition(CMemoryPool *mp, CEngineSpec *pes)
        : CBasePhysicalUnaryOp(mp),
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
};

}