#pragma once

#include "gpos/base.h"

#include "gpopt/operators/CExpressionHandle.h"
#include "gpopt/operators/CLogicalUnary.h"
#include "gpopt/operators/CLogicalSelect.h"
#include "gpoptextender/DynamicRegistry/DynamicRegistry.hpp"

namespace orcaextender
{
  using namespace gpopt;

//---------------------------------------------------------------------------
//	@class:
//		CLogicalSelect
//
//	@doc:
//		Select operator
//
//---------------------------------------------------------------------------
class CBaseLogicalOp : public CLogical
{
private:
	// private copy ctor
	CBaseLogicalOp(const CBaseLogicalOp &);

public:
	// ctor
	explicit CBaseLogicalOp(CMemoryPool *mp);

	// dtor
	virtual ~CBaseLogicalOp();

  	// derive output columns
	virtual CColRefSet *DeriveOutputColumns(CMemoryPool *mp,
											CExpressionHandle &exprhdl);

  virtual CPropConstraint *DerivePropertyConstraint(CMemoryPool *mp, CExpressionHandle &exprhdl) const;

	virtual IStatistics *PstatsDerive(CMemoryPool *mp,
									  CExpressionHandle &exprhdl,
									  IStatisticsArray *stats_ctxt) const;

	virtual CPartInfo *DerivePartitionInfo(CMemoryPool *mp, CExpressionHandle &exprhdl) const;

	virtual CColRefSet *PcrsStat(CMemoryPool *mp, CExpressionHandle &exprhdl,
								 CColRefSet *pcrsInput,
								 ULONG child_index) const;


	BOOL Matches(COperator *pop) const {
		return pop->Eopid() == Eopid();
	}

	// return a copy of the operator with remapped columns
	virtual COperator *
	PopCopyWithRemappedColumns(CMemoryPool *,		//mp,
							   UlongToColRefMap *,	//colref_mapping,
							   BOOL					//must_exist
	)
	{
		return PopCopyDefault();
	}
};	// class CBaseLogicalOp

}  // namespace orcaextender
