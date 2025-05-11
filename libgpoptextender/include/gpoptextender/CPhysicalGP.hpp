#pragma once

#include "gpos/base.h"

#include "gpopt/operators/CPhysical.h"
#include "gpoptextender/CEngineSpecGP.hpp"

namespace gpopt {
using namespace gpos;
using namespace gpmd;

class CPhysicalGP : public CPhysical {
private: 
	// private copy ctor
	CPhysicalGP(const CPhysicalGP &);
public:
  CPhysicalGP(CMemoryPool *mp);

  virtual ~CPhysicalGP();

	virtual CEngineSpec *PesRequired(CMemoryPool *mp,
									  CExpressionHandle &exprhdl,
									  CEngineSpec *pesRequired,
									  ULONG child_index,
									  CDrvdPropArray *pdrgpdpCtxt,
									  ULONG ulOptReq) const;
};

}