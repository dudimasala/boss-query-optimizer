//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CXformProject2ComputeScalar.cpp
//
//	@doc:
//		Implementation of transform
//---------------------------------------------------------------------------

#include "gpopt/xforms/CXformProject2ComputeScalar.h"

#include "gpos/base.h"

#include "gpopt/operators/ops.h"

#include "gpoptextender/DynamicRegistry/IDynamicRegistry.hpp"
#include "gpoptextender/DynamicRegistry/DynamicOperatorArgs.hpp"


using namespace gpopt;


//---------------------------------------------------------------------------
//	@function:
//		CXformProject2ComputeScalar::CXformProject2ComputeScalar
//
//	@doc:
//		Ctor
//
//---------------------------------------------------------------------------
CXformProject2ComputeScalar::CXformProject2ComputeScalar(CMemoryPool *mp)
	:  // pattern
	  CXformImplementation(GPOS_NEW(mp) CExpression(
		  mp, GPOS_NEW(mp) CLogicalProject(mp),
		  GPOS_NEW(mp) CExpression(
			  mp, GPOS_NEW(mp) CPatternLeaf(mp)),  // relational child
		  GPOS_NEW(mp)
			  CExpression(mp, GPOS_NEW(mp) CPatternLeaf(mp))  // scalar child
		  ))
{
}


//---------------------------------------------------------------------------
//	@function:
//		CXformProject2ComputeScalar::Transform
//
//	@doc:
//		Actual transformation
//
//---------------------------------------------------------------------------
void
CXformProject2ComputeScalar::Transform(CXformContext *pxfctxt,
									   CXformResult *pxfres,
									   CExpression *pexpr) const
{
	GPOS_ASSERT(NULL != pxfctxt);
	GPOS_ASSERT(FPromising(pxfctxt->Pmp(), this, pexpr));
	GPOS_ASSERT(FCheckPattern(pexpr));

	CMemoryPool *mp = pxfctxt->Pmp();

	// extract components
	CExpression *pexprRelational = (*pexpr)[0];
	CExpression *pexprScalar = (*pexpr)[1];

	// addref all children
	pexprRelational->AddRef();
	pexprScalar->AddRef();

	// assemble physical operator
	CExpression *pexprComputeScalar =
		GPOS_NEW(mp) CExpression(mp, GPOS_NEW(mp) CPhysicalComputeScalar(mp),
								 pexprRelational, pexprScalar);

	// add alternative to results
	pxfres->Add(pexprComputeScalar);

	// dynamic
	orcaextender::IDynamicRegistry *registry = orcaextender::CreateDynamicRegistry();
	orcaextender::CProjectArgs args = orcaextender::CProjectArgs(mp);
	
	std::vector<COperator*> dynOperators = registry->GetRelevantOperatorsForTransform(Exfid(), args);
	for (size_t i = 0; i < dynOperators.size(); i++) {
		pexprRelational->AddRef();
		pexprScalar->AddRef();

		CExpression *pexprAnotherAlt = GPOS_NEW(mp) CExpression(
			mp,
			dynOperators[i],
			pexprRelational, pexprScalar);

		// add alternative to transformation result
		pxfres->Add(pexprAnotherAlt);
	}
}


// EOF
