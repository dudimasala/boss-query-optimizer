//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2013 Pivotal, Inc.
//
//	@filename:
//		CLogicalExternalGet.cpp
//
//	@doc:
//		Implementation of external get
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Copyright 2025 Malhar Rajpal
// Modifications to this file: Added AddTransformsToXFormSet() in 
// PxfsCandidates() to enable dynamic addition of transforms .
//---------------------------------------------------------------------------

#include "gpopt/operators/CLogicalExternalGet.h"

#include "gpos/base.h"

#include "gpopt/base/CColRefSet.h"
#include "gpopt/base/CColRefSetIter.h"
#include "gpopt/base/CColRefTable.h"
#include "gpopt/base/COptCtxt.h"
#include "gpopt/base/CUtils.h"
#include "gpopt/metadata/CName.h"
#include "gpopt/metadata/CTableDescriptor.h"
#include "gpoptextender/DynamicRegistry/IDynamicRegistry.hpp"


using namespace gpopt;


//---------------------------------------------------------------------------
//	@function:
//		CLogicalExternalGet::CLogicalExternalGet
//
//	@doc:
//		Ctor - for pattern
//
//---------------------------------------------------------------------------
CLogicalExternalGet::CLogicalExternalGet(CMemoryPool *mp) : CLogicalGet(mp)
{
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalExternalGet::CLogicalExternalGet
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CLogicalExternalGet::CLogicalExternalGet(CMemoryPool *mp,
										 const CName *pnameAlias,
										 CTableDescriptor *ptabdesc)
	: CLogicalGet(mp, pnameAlias, ptabdesc)
{
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalExternalGet::CLogicalExternalGet
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CLogicalExternalGet::CLogicalExternalGet(CMemoryPool *mp,
										 const CName *pnameAlias,
										 CTableDescriptor *ptabdesc,
										 CColRefArray *pdrgpcrOutput)
	: CLogicalGet(mp, pnameAlias, ptabdesc, pdrgpcrOutput)
{
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalExternalGet::Matches
//
//	@doc:
//		Match function on operator level
//
//---------------------------------------------------------------------------
BOOL
CLogicalExternalGet::Matches(COperator *pop) const
{
	if (pop->Eopid() != Eopid())
	{
		return false;
	}
	CLogicalExternalGet *popGet = CLogicalExternalGet::PopConvert(pop);

	return Ptabdesc() == popGet->Ptabdesc() &&
		   PdrgpcrOutput()->Equals(popGet->PdrgpcrOutput());
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalExternalGet::PopCopyWithRemappedColumns
//
//	@doc:
//		Return a copy of the operator with remapped columns
//
//---------------------------------------------------------------------------
COperator *
CLogicalExternalGet::PopCopyWithRemappedColumns(
	CMemoryPool *mp, UlongToColRefMap *colref_mapping, BOOL must_exist)
{
	CColRefArray *pdrgpcrOutput = NULL;
	if (must_exist)
	{
		pdrgpcrOutput =
			CUtils::PdrgpcrRemapAndCreate(mp, PdrgpcrOutput(), colref_mapping);
	}
	else
	{
		pdrgpcrOutput = CUtils::PdrgpcrRemap(mp, PdrgpcrOutput(),
											 colref_mapping, must_exist);
	}
	CName *pnameAlias = GPOS_NEW(mp) CName(mp, Name());

	CTableDescriptor *ptabdesc = Ptabdesc();
	ptabdesc->AddRef();

	return GPOS_NEW(mp)
		CLogicalExternalGet(mp, pnameAlias, ptabdesc, pdrgpcrOutput);
}

//---------------------------------------------------------------------------
//	@function:
//		CLogicalExternalGet::PxfsCandidates
//
//	@doc:
//		Get candidate xforms
//
//---------------------------------------------------------------------------
CXformSet *
CLogicalExternalGet::PxfsCandidates(CMemoryPool *mp) const
{
	CXformSet *xform_set = GPOS_NEW(mp) CXformSet(mp);
	(void) xform_set->ExchangeSet(CXform::ExfExternalGet2ExternalScan);

	orcaextender::IDynamicRegistry *registry = orcaextender::CreateDynamicRegistry();
	registry->AddTransformsToXFormSet(Eopid(), xform_set);

	return xform_set;
}

// EOF
