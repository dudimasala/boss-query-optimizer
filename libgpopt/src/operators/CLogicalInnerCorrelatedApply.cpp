//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		CLogicalInnerCorrelatedApply.cpp
//
//	@doc:
//		Implementation of inner correlated apply operator
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Copyright 2025 Malhar Rajpal
// Modifications to this file: Added AddTransformsToXFormSet() in 
// PxfsCandidates() to enable dynamic addition of transforms .
//---------------------------------------------------------------------------

#include "gpopt/operators/CLogicalInnerCorrelatedApply.h"
#include "gpoptextender/DynamicRegistry/IDynamicRegistry.hpp"

#include "gpos/base.h"

using namespace gpopt;

//---------------------------------------------------------------------------
//	@function:
//		CLogicalInnerCorrelatedApply::CLogicalInnerCorrelatedApply
//
//	@doc:
//		Ctor - for patterns
//
//---------------------------------------------------------------------------
CLogicalInnerCorrelatedApply::CLogicalInnerCorrelatedApply(CMemoryPool *mp)
	: CLogicalInnerApply(mp)
{
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalInnerCorrelatedApply::CLogicalInnerCorrelatedApply
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CLogicalInnerCorrelatedApply::CLogicalInnerCorrelatedApply(
	CMemoryPool *mp, CColRefArray *pdrgpcrInner, EOperatorId eopidOriginSubq)
	: CLogicalInnerApply(mp, pdrgpcrInner, eopidOriginSubq)
{
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalInnerCorrelatedApply::PxfsCandidates
//
//	@doc:
//		Get candidate xforms
//
//---------------------------------------------------------------------------
CXformSet *
CLogicalInnerCorrelatedApply::PxfsCandidates(CMemoryPool *mp) const
{
	CXformSet *xform_set = GPOS_NEW(mp) CXformSet(mp);
	(void) xform_set->ExchangeSet(CXform::ExfImplementInnerCorrelatedApply);

	orcaextender::IDynamicRegistry *registry = orcaextender::CreateDynamicRegistry();
	registry->AddTransformsToXFormSet(Eopid(), xform_set);

	return xform_set;
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalInnerCorrelatedApply::Matches
//
//	@doc:
//		Match function
//
//---------------------------------------------------------------------------
BOOL
CLogicalInnerCorrelatedApply::Matches(COperator *pop) const
{
	if (pop->Eopid() == Eopid())
	{
		return m_pdrgpcrInner->Equals(
			CLogicalInnerCorrelatedApply::PopConvert(pop)->PdrgPcrInner());
	}

	return false;
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalInnerCorrelatedApply::PopCopyWithRemappedColumns
//
//	@doc:
//		Return a copy of the operator with remapped columns
//
//---------------------------------------------------------------------------
COperator *
CLogicalInnerCorrelatedApply::PopCopyWithRemappedColumns(
	CMemoryPool *mp, UlongToColRefMap *colref_mapping, BOOL must_exist)
{
	CColRefArray *pdrgpcrInner =
		CUtils::PdrgpcrRemap(mp, m_pdrgpcrInner, colref_mapping, must_exist);

	return GPOS_NEW(mp)
		CLogicalInnerCorrelatedApply(mp, pdrgpcrInner, m_eopidOriginSubq);
}

// EOF
