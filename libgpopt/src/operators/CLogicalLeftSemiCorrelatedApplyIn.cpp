//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2014 Pivotal Inc.
//
//	@filename:
//		CLogicalLeftSemiCorrelatedApplyIn.cpp
//
//	@doc:
//		Implementation of left semi correlated apply with IN semantics
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Copyright 2025 Malhar Rajpal
// Modifications to this file: Added AddTransformsToXFormSet() in 
// PxfsCandidates() to enable dynamic addition of transforms .
//---------------------------------------------------------------------------

#include "gpopt/operators/CLogicalLeftSemiCorrelatedApplyIn.h"
#include "gpoptextender/DynamicRegistry/IDynamicRegistry.hpp"

#include "gpos/base.h"

using namespace gpopt;

//---------------------------------------------------------------------------
//	@function:
//		CLogicalLeftSemiCorrelatedApplyIn::CLogicalLeftSemiCorrelatedApplyIn
//
//	@doc:
//		Ctor - for patterns
//
//---------------------------------------------------------------------------
CLogicalLeftSemiCorrelatedApplyIn::CLogicalLeftSemiCorrelatedApplyIn(
	CMemoryPool *mp)
	: CLogicalLeftSemiApplyIn(mp)
{
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalLeftSemiCorrelatedApplyIn::CLogicalLeftSemiCorrelatedApplyIn
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CLogicalLeftSemiCorrelatedApplyIn::CLogicalLeftSemiCorrelatedApplyIn(
	CMemoryPool *mp, CColRefArray *pdrgpcrInner, EOperatorId eopidOriginSubq)
	: CLogicalLeftSemiApplyIn(mp, pdrgpcrInner, eopidOriginSubq)
{
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalLeftSemiCorrelatedApplyIn::PxfsCandidates
//
//	@doc:
//		Get candidate xforms
//
//---------------------------------------------------------------------------
CXformSet *
CLogicalLeftSemiCorrelatedApplyIn::PxfsCandidates(CMemoryPool *mp) const
{
	CXformSet *xform_set = GPOS_NEW(mp) CXformSet(mp);
	(void) xform_set->ExchangeSet(
		CXform::ExfImplementLeftSemiCorrelatedApplyIn);

	orcaextender::IDynamicRegistry *registry = orcaextender::CreateDynamicRegistry();
	registry->AddTransformsToXFormSet(Eopid(), xform_set);

	return xform_set;
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalLeftSemiCorrelatedApplyIn::PopCopyWithRemappedColumns
//
//	@doc:
//		Return a copy of the operator with remapped columns
//
//---------------------------------------------------------------------------
COperator *
CLogicalLeftSemiCorrelatedApplyIn::PopCopyWithRemappedColumns(
	CMemoryPool *mp, UlongToColRefMap *colref_mapping, BOOL must_exist)
{
	CColRefArray *pdrgpcrInner =
		CUtils::PdrgpcrRemap(mp, m_pdrgpcrInner, colref_mapping, must_exist);

	return GPOS_NEW(mp)
		CLogicalLeftSemiCorrelatedApplyIn(mp, pdrgpcrInner, m_eopidOriginSubq);
}


// EOF
