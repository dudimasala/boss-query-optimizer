//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2014 Pivotal Inc.
//
//	@filename:
//		CLogicalLeftSemiCorrelatedApply.cpp
//
//	@doc:
//		Implementation of left semi correlated apply for EXISTS subqueries
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Copyright 2025 Malhar Rajpal
// Modifications to this file: Added AddTransformsToXFormSet() in 
// PxfsCandidates() to enable dynamic addition of transforms .
//---------------------------------------------------------------------------

#include "gpopt/operators/CLogicalLeftSemiCorrelatedApply.h"
#include "gpoptextender/DynamicRegistry/IDynamicRegistry.hpp"

#include "gpos/base.h"

using namespace gpopt;

//---------------------------------------------------------------------------
//	@function:
//		CLogicalLeftSemiCorrelatedApply::CLogicalLeftSemiCorrelatedApply
//
//	@doc:
//		Ctor - for patterns
//
//---------------------------------------------------------------------------
CLogicalLeftSemiCorrelatedApply::CLogicalLeftSemiCorrelatedApply(
	CMemoryPool *mp)
	: CLogicalLeftSemiApply(mp)
{
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalLeftSemiCorrelatedApply::CLogicalLeftSemiCorrelatedApply
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CLogicalLeftSemiCorrelatedApply::CLogicalLeftSemiCorrelatedApply(
	CMemoryPool *mp, CColRefArray *pdrgpcrInner, EOperatorId eopidOriginSubq)
	: CLogicalLeftSemiApply(mp, pdrgpcrInner, eopidOriginSubq)
{
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalLeftSemiCorrelatedApply::PxfsCandidates
//
//	@doc:
//		Get candidate xforms
//
//---------------------------------------------------------------------------
CXformSet *
CLogicalLeftSemiCorrelatedApply::PxfsCandidates(CMemoryPool *mp) const
{
	CXformSet *xform_set = GPOS_NEW(mp) CXformSet(mp);
	(void) xform_set->ExchangeSet(CXform::ExfImplementLeftSemiCorrelatedApply);

	orcaextender::IDynamicRegistry *registry = orcaextender::CreateDynamicRegistry();
	registry->AddTransformsToXFormSet(Eopid(), xform_set);

	return xform_set;
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalLeftSemiCorrelatedApply::PopCopyWithRemappedColumns
//
//	@doc:
//		Return a copy of the operator with remapped columns
//
//---------------------------------------------------------------------------
COperator *
CLogicalLeftSemiCorrelatedApply::PopCopyWithRemappedColumns(
	CMemoryPool *mp, UlongToColRefMap *colref_mapping, BOOL must_exist)
{
	CColRefArray *pdrgpcrInner =
		CUtils::PdrgpcrRemap(mp, m_pdrgpcrInner, colref_mapping, must_exist);

	return GPOS_NEW(mp)
		CLogicalLeftSemiCorrelatedApply(mp, pdrgpcrInner, m_eopidOriginSubq);
}


// EOF
