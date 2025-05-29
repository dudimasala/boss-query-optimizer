//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2011 EMC Corp.
//
//	@filename:
//		CXformInnerJoin2HashJoin.cpp
//
//	@doc:
//		Implementation of transform
//---------------------------------------------------------------------------

#include "gpopt/xforms/CXformInnerJoin2HashJoin.h"

#include "gpos/base.h"

#include "gpopt/operators/CPredicateUtils.h"
#include "gpopt/operators/ops.h"
#include "gpopt/xforms/CXformUtils.h"

#include "gpoptextender/DynamicRegistry/IDynamicRegistry.hpp"
#include "gpoptextender/DynamicRegistry/DynamicOperatorArgs.hpp"

using namespace gpopt;


//---------------------------------------------------------------------------
//	@function:
//		CXformInnerJoin2HashJoin::CXformInnerJoin2HashJoin
//
//	@doc:
//		ctor
//
//---------------------------------------------------------------------------
CXformInnerJoin2HashJoin::CXformInnerJoin2HashJoin(CMemoryPool *mp)
	:  // pattern
	  CXformImplementation(GPOS_NEW(mp) CExpression(
		  mp, GPOS_NEW(mp) CLogicalInnerJoin(mp),
		  GPOS_NEW(mp)
			  CExpression(mp, GPOS_NEW(mp) CPatternLeaf(mp)),  // left child
		  GPOS_NEW(mp)
			  CExpression(mp, GPOS_NEW(mp) CPatternLeaf(mp)),  // right child
		  GPOS_NEW(mp)
			  CExpression(mp, GPOS_NEW(mp) CPatternTree(mp))  // predicate
		  ))
{
}


//---------------------------------------------------------------------------
//	@function:
//		CXformInnerJoin2HashJoin::Exfp
//
//	@doc:
//		Compute xform promise for a given expression handle;
//
//---------------------------------------------------------------------------
CXform::EXformPromise
CXformInnerJoin2HashJoin::Exfp(CExpressionHandle &exprhdl) const
{
  // CExpression *pred = exprhdl.PexprScalarExactChild(2);
	// if (pred->Pop()->Eopid() == COperator::EopScalarBoolOp) {
	// 	if (pred->Arity() > 2) {
	// 		return CXform::ExfpNone;
	// 	} else {
	// 		CExpression *left = (*pred)[0];
	// 		CExpression *right = (*pred)[1];
	// 		if (left->Pop()->Eopid() == COperator::EopScalarCmp && right->Pop()->Eopid() == COperator::EopScalarCmp) {
	// 			CExpression *leftChild1 = (*left)[0];
	// 			CExpression *leftChild2 = (*left)[1];
	// 			CExpression *rightChild1 = (*right)[0];
	// 			CExpression *rightChild2 = (*right)[1];
	// 			if (leftChild1->Pop()->Eopid() == COperator::EopScalarIdent && leftChild2->Pop()->Eopid() == COperator::EopScalarIdent && rightChild1->Pop()->Eopid() == COperator::EopScalarIdent && rightChild2->Pop()->Eopid() == COperator::EopScalarIdent) {
	// 				std::cout << "here" << std::endl;
	// 				const CColRef *pcrLeft1 = CScalarIdent::PopConvert(leftChild1->Pop())->Pcr();
	// 				const CColRef *pcrLeft2 = CScalarIdent::PopConvert(leftChild2->Pop())->Pcr();
	// 				const CColRef *pcrRight1 = CScalarIdent::PopConvert(rightChild1->Pop())->Pcr();
	// 				const CColRef *pcrRight2 = CScalarIdent::PopConvert(rightChild2->Pop())->Pcr();
	// 				std::wcout << pcrLeft1->Name().Pstr()->GetBuffer() << std::endl;
	// 				std::wcout << pcrRight1->Name().Pstr()->GetBuffer() << std::endl;
	// 				std::wcout << pcrLeft2->Name().Pstr()->GetBuffer() << std::endl;
	// 				std::wcout << pcrRight2->Name().Pstr()->GetBuffer() << std::endl;

	// 				if (pcrLeft1->Name().Pstr()->GetBuffer()[0] != pcrRight1->Name().Pstr()->GetBuffer()[0] || pcrLeft2->Name().Pstr()->GetBuffer()[0] != pcrRight2->Name().Pstr()->GetBuffer()[0]) {
	// 					std::cout << "here2" << std::endl;
	// 					return CXform::ExfpNone;
	// 				}
	// 				std::cout << CXformUtils::ExfpLogicalJoin2PhysicalJoin(exprhdl) << std::endl;
	// 			}
	// 		}
	// 	}
	// }

	return CXformUtils::ExfpLogicalJoin2PhysicalJoin(exprhdl);
}


//---------------------------------------------------------------------------
//	@function:
//		CXformInnerJoin2HashJoin::Transform
//
//	@doc:
//		actual transformation
//
//---------------------------------------------------------------------------
void
CXformInnerJoin2HashJoin::Transform(CXformContext *pxfctxt,
									CXformResult *pxfres,
									CExpression *pexpr) const
{
	GPOS_ASSERT(NULL != pxfctxt);
	GPOS_ASSERT(FPromising(pxfctxt->Pmp(), this, pexpr));
	GPOS_ASSERT(FCheckPattern(pexpr));

	CXformUtils::ImplementHashJoin<CPhysicalInnerHashJoin>(pxfctxt, pxfres,
														   pexpr, Exfid());

}

// EOF
