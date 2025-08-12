//---------------------------------------------------------------------------
// Copyright 2025 Malhar Rajpal
// Base logical operator with standard properties to make it easy to create
// a new logical operator
//---------------------------------------------------------------------------


#include "gpoptextender/GenericOps/CBaseLogicalOp.hpp"

namespace orcaextender {
  using namespace gpopt;
  
  CBaseLogicalOp::CBaseLogicalOp(CMemoryPool *mp) : CLogical(mp) {
  }

  CBaseLogicalOp::~CBaseLogicalOp() {
  }

  CColRefSet *CBaseLogicalOp::DeriveOutputColumns(CMemoryPool *mp, CExpressionHandle &exprhdl) {
    return PcrsDeriveOutputPassThru(exprhdl);
  }

  CPropConstraint *CBaseLogicalOp::DerivePropertyConstraint(CMemoryPool *mp, CExpressionHandle &exprhdl) const {
    return PpcDeriveConstraintPassThru(exprhdl, 0);
  }

  IStatistics *CBaseLogicalOp::PstatsDerive(CMemoryPool *mp, CExpressionHandle &exprhdl, IStatisticsArray *stats_ctxt) const {
    return PstatsPassThruOuter(exprhdl);
  }

  CPartInfo *CBaseLogicalOp::DerivePartitionInfo(CMemoryPool *mp, CExpressionHandle &exprhdl) const {
    return PpartinfoPassThruOuter(exprhdl);
  }

  CColRefSet *CBaseLogicalOp::PcrsStat(CMemoryPool *mp, CExpressionHandle &exprhdl, CColRefSet *pcrsInput, ULONG child_index) const {
		return PcrsStatsPassThru(pcrsInput);
  }

}
