//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2013 EMC Corp.
//
//	@filename:
//		CLogicalLeftAntiSemiJoinNotIn.cpp
//
//	@doc:
//		Implementation of left anti semi join operator
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Copyright 2025 Malhar Rajpal
// Modifications to this file: Added AddTransformsToXFormSet() in 
// PxfsCandidates() to enable dynamic addition of transforms .
//---------------------------------------------------------------------------

#include "gpopt/operators/CLogicalLeftAntiSemiJoinNotIn.h"

#include "gpos/base.h"

#include "gpopt/base/CColRefSet.h"
#include "gpopt/operators/CExpression.h"
#include "gpopt/operators/CExpressionHandle.h"

#include "gpoptextender/DynamicRegistry/IDynamicRegistry.hpp"

using namespace gpopt;


//---------------------------------------------------------------------------
//	@function:
//		CLogicalLeftAntiSemiJoinNotIn::CLogicalLeftAntiSemiJoinNotIn
//
//	@doc:
//		ctor
//
//---------------------------------------------------------------------------
CLogicalLeftAntiSemiJoinNotIn::CLogicalLeftAntiSemiJoinNotIn(CMemoryPool *mp)
	: CLogicalLeftAntiSemiJoin(mp)
{
	GPOS_ASSERT(NULL != mp);
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalLeftAntiSemiJoinNotIn::PxfsCandidates
//
//	@doc:
//		Get candidate xforms
//
//---------------------------------------------------------------------------
CXformSet *
CLogicalLeftAntiSemiJoinNotIn::PxfsCandidates(CMemoryPool *mp) const
{
	CXformSet *xform_set = GPOS_NEW(mp) CXformSet(mp);

	(void) xform_set->ExchangeSet(
		CXform::ExfAntiSemiJoinNotInAntiSemiJoinNotInSwap);
	(void) xform_set->ExchangeSet(CXform::ExfAntiSemiJoinNotInAntiSemiJoinSwap);
	(void) xform_set->ExchangeSet(CXform::ExfAntiSemiJoinNotInSemiJoinSwap);
	(void) xform_set->ExchangeSet(CXform::ExfAntiSemiJoinNotInInnerJoinSwap);
	(void) xform_set->ExchangeSet(
		CXform::ExfLeftAntiSemiJoinNotIn2CrossProduct);
	(void) xform_set->ExchangeSet(CXform::ExfLeftAntiSemiJoinNotIn2NLJoinNotIn);
	(void) xform_set->ExchangeSet(
		CXform::ExfLeftAntiSemiJoinNotIn2HashJoinNotIn);

	orcaextender::IDynamicRegistry *registry = orcaextender::CreateDynamicRegistry();
	registry->AddTransformsToXFormSet(Eopid(), xform_set);

	return xform_set;
}

// EOF
