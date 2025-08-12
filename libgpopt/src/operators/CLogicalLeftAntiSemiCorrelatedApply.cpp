//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2014 Pivotal Inc.
//
//	@filename:
//		CLogicalLeftAntiSemiCorrelatedApply.cpp
//
//	@doc:
//		Implementation of left anti semi correlated apply for NOT EXISTS subqueries
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Copyright 2025 Malhar Rajpal
// Modifications to this file: Added AddTransformsToXFormSet() in 
// PxfsCandidates() to enable dynamic addition of transforms .
//---------------------------------------------------------------------------

#include "gpopt/operators/CLogicalLeftAntiSemiCorrelatedApply.h"
#include "gpoptextender/DynamicRegistry/IDynamicRegistry.hpp"

#include "gpos/base.h"

using namespace gpopt;

//---------------------------------------------------------------------------
//	@function:
//		CLogicalLeftAntiSemiCorrelatedApply::PxfsCandidates
//
//	@doc:
//		Get candidate xforms
//
//---------------------------------------------------------------------------
CXformSet *
CLogicalLeftAntiSemiCorrelatedApply::PxfsCandidates(CMemoryPool *mp) const
{
	CXformSet *xform_set = GPOS_NEW(mp) CXformSet(mp);
	(void) xform_set->ExchangeSet(
		CXform::ExfImplementLeftAntiSemiCorrelatedApply);

	orcaextender::IDynamicRegistry *registry = orcaextender::CreateDynamicRegistry();
	registry->AddTransformsToXFormSet(Eopid(), xform_set);

	return xform_set;
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalLeftAntiSemiCorrelatedApply::PopCopyWithRemappedColumns
//
//	@doc:
//		Return a copy of the operator with remapped columns
//
//---------------------------------------------------------------------------
COperator *
CLogicalLeftAntiSemiCorrelatedApply::PopCopyWithRemappedColumns(
	CMemoryPool *mp, UlongToColRefMap *colref_mapping, BOOL must_exist)
{
	CColRefArray *pdrgpcrInner =
		CUtils::PdrgpcrRemap(mp, m_pdrgpcrInner, colref_mapping, must_exist);

	return GPOS_NEW(mp) CLogicalLeftAntiSemiCorrelatedApply(mp, pdrgpcrInner,
															m_eopidOriginSubq);
}


// EOF
