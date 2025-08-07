#pragma once

#include "gpos/base.h"

#include "gpoptextender/EngineProperty/EEngineType.hpp"
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
    EEngineType m_eet;

public:
    // ctor
    CEngineSpec(EEngineType eet) : m_eet(eet) {}

    // Dtor
    virtual ~CEngineSpec()
    {
    }

    // Engine type accessor
    virtual EEngineType Eet() const
    {
        return m_eet;
    }

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

    virtual CColRefSet *PcrsUsed(CMemoryPool *mp) const
    {
        return GPOS_NEW(mp) CColRefSet(mp);
    }

	// default match function for distribution specs
	virtual BOOL
	Matches(const CEngineSpec *pes) const
	{
		return Eet() == pes->Eet();
	}

	// default implementation for all the classes inheriting from
	// CEngineSpec, if any class requires special Equals
	// handling, they should override it.
	virtual BOOL
	Equals(const CEngineSpec *pes) const
	{
		return Matches(pes);
	}

    virtual IOstream &OsPrint(IOstream &os) const;
};

}