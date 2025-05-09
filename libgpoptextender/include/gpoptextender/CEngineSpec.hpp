#pragma once

#include "gpos/base.h"

#include "gpopt/base/CPropSpec.h"

namespace gpopt {
using namespace gpos;
using namespace gpmd;
// Engine specification base class
class CEngineSpec : public CPropSpec
{
private:
    // Private copy ctor
    CEngineSpec(const CEngineSpec &);

public:
    // Engine types
    enum EEngineType
    {
        EetCPU,
        EetAny,   // Can be executed on any engine
        EetSentinel
    };

    // Ctor
    CEngineSpec()
    {
    }

    // Dtor
    virtual ~CEngineSpec()
    {
    }


    virtual EEngineType Eet() const = 0;

    // append enforcers to dynamic array for the given plan properties
    virtual void AppendEnforcers(CMemoryPool *mp, CExpressionHandle &exprhdl,
                                 CReqdPropPlan *prpp,
                                 CExpressionArray *pdrgpexpr,
                                 CExpression *pexpr);

    virtual BOOL FSatisfies(const CEngineSpec *pes) const;

    // hash function
	virtual ULONG
	HashValue() const
	{
		ULONG ulEet = (ULONG) Eet();
		return gpos::HashValue<ULONG>(&ulEet);
	}

    // property type
    virtual EPropSpecType Epst() const
    {
        return EpstEngine;
    }

    // extract columns used by the property
	virtual CColRefSet *PcrsUsed(CMemoryPool *mp) const;

    


};

}