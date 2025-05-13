#pragma once

#include "gpos/base.h"
#include "gpos/common/CDouble.h"

#include "gpdbcost/CCostModelParamsGPDB.h"
#include "gpopt/cost/CCost.h"
#include "gpopt/cost/ICostModel.h"
#include "gpopt/cost/ICostModelParams.h"
#include "gpoptextender/EngineProperty/CEngineSpec.hpp"

#include <unordered_map>
#include <functional>

namespace orcaextender
{
using namespace gpos;
using namespace gpopt;
using namespace gpmd;


//---------------------------------------------------------------------------
//	@class:
//		CCostModelGPDB
//
//	@doc:
//		GPDB cost model
//
//---------------------------------------------------------------------------
class BOSSCostModel : public ICostModel
{
private:
	using FnCost = std::function<CCost(CMemoryPool*, CExpressionHandle&, const BOSSCostModel*, const SCostingInfo*)>;

	// memory pool
	CMemoryPool *m_mp;

	// number of segments
	ULONG m_num_of_segments;

	// Maps engines to cost model params (so can change params at runtime).
	std::unordered_map<CEngineSpec::EEngineType, ICostModelParams*> m_cost_model_params_map;

	// array of mappings
	static std::unordered_map<COperator::EOperatorId, FnCost> m_cost_map;

	// First, create a hash struct for the pair
	struct EngineTransformPairHash {
			std::size_t operator()(const std::pair<CEngineSpec::EEngineType, CEngineSpec::EEngineType>& p) const {
					// Combine the hashes of both enum values
					// This is a simple but effective way to hash a pair
					std::size_t h1 = std::hash<int>{}(static_cast<int>(p.first));
					std::size_t h2 = std::hash<int>{}(static_cast<int>(p.second));
					
					// Combine the hashes - a common technique is to shift one and XOR
					return h1 ^ (h2 << 1);
			}
	};
	
	// array of engine transform mappings
	static std::unordered_map<std::pair<CEngineSpec::EEngineType, CEngineSpec::EEngineType>, FnCost, EngineTransformPairHash> m_engine_transform_map;

public:
	// ctor
	BOSSCostModel(CMemoryPool *mp, ULONG ulSegments,
				   CCostModelParamsGPDB *pcp = NULL);
	// dtor
	virtual ~BOSSCostModel();

	void RegisterCostFunction(COperator::EOperatorId op_id, FnCost fn_cost);

	void RegisterCostModelParams(CEngineSpec::EEngineType engine, ICostModelParams* pcp);

	// number of segments
	ULONG
	UlHosts() const
	{
		return m_num_of_segments;
	}

	// return number of rows per host
	virtual CDouble DRowsPerHost(CDouble dRowsTotal) const;

	// return cost model parameters (need this to fulfil class interface)
	virtual ICostModelParams *
	GetCostModelParams() const
	{
		return m_cost_model_params_map.at(CEngineSpec::EEngineType::EetGP);
	}

	// in use - real implementation
	ICostModelParams *GetCostModelParams(CEngineSpec::EEngineType engine) const
	{
		if (m_cost_model_params_map.find(engine) == m_cost_model_params_map.end())
		{
			std::cerr << "Cost model params not found for engine type" << std::endl;
			throw;
		}
		return m_cost_model_params_map.at(engine);
	}


	// main driver for cost computation
	virtual CCost Cost(CExpressionHandle &exprhdl,
					   const SCostingInfo *pci) const;

	// cost model type
	virtual ECostModelType
	Ecmt() const
	{
		return ICostModel::EcmtGPDBCalibrated;
	}

	// Want these to be public so new operators can use them (or variations of them) easily.
	// return cost of processing the given number of rows
	static CCost CostTupleProcessing(DOUBLE rows, DOUBLE width,
									 ICostModelParams *pcp);

	// helper function to return cost of producing output tuples from Scan operator
	static CCost CostScanOutput(CMemoryPool *mp, DOUBLE rows, DOUBLE width,
								DOUBLE num_rebinds, ICostModelParams *pcp);

	// helper function to return cost of a plan rooted by unary operator
	static CCost CostUnary(CMemoryPool *mp, CExpressionHandle &exprhdl,
						   const BOSSCostModel *pcmgpdb,
						   const SCostingInfo *pci);

	// cost of spooling
	static CCost CostSpooling(CMemoryPool *mp, CExpressionHandle &exprhdl,
							  const SCostingInfo *pci, ICostModelParams *pcp);

	// add up children cost
	static CCost CostChildren(CMemoryPool *mp, CExpressionHandle &exprhdl,
							  const SCostingInfo *pci, ICostModelParams *pcp);

	// returns cost of highest costed child
	static CCost CostMaxChild(CMemoryPool *mp, CExpressionHandle &exprhdl,
							  const SCostingInfo *pci, ICostModelParams *pcp);

	// check if given operator is unary
	static BOOL FUnary(COperator::EOperatorId op_id);

	// cost of scan
	static CCost CostScan(CMemoryPool *mp, CExpressionHandle &exprhdl,
						  const BOSSCostModel *pcmgpdb,
						  const SCostingInfo *pci);

	// cost of filter
	static CCost CostFilter(CMemoryPool *mp, CExpressionHandle &exprhdl,
							const BOSSCostModel *pcmgpdb,
							const SCostingInfo *pci);

	// cost of index scan
	static CCost CostIndexScan(CMemoryPool *mp, CExpressionHandle &exprhdl,
							   const BOSSCostModel *pcmgpdb,
							   const SCostingInfo *pci);

	// cost of bitmap table scan
	static CCost CostBitmapTableScan(CMemoryPool *mp,
									 CExpressionHandle &exprhdl,
									 const BOSSCostModel *pcmgpdb,
									 const SCostingInfo *pci);

	// cost of sequence project
	static CCost CostSequenceProject(CMemoryPool *mp,
									 CExpressionHandle &exprhdl,
									 const BOSSCostModel *pcmgpdb,
									 const SCostingInfo *pci);

	// cost of CTE producer
	static CCost CostCTEProducer(CMemoryPool *mp, CExpressionHandle &exprhdl,
								 const BOSSCostModel *pcmgpdb,
								 const SCostingInfo *pci);

	// cost of CTE consumer
	static CCost CostCTEConsumer(CMemoryPool *mp, CExpressionHandle &exprhdl,
								 const BOSSCostModel *pcmgpdb,
								 const SCostingInfo *pci);

	// cost of const table get
	static CCost CostConstTableGet(CMemoryPool *mp, CExpressionHandle &exprhdl,
								   const BOSSCostModel *pcmgpdb,
								   const SCostingInfo *pci);

	// cost of DML
	static CCost CostDML(CMemoryPool *mp, CExpressionHandle &exprhdl,
						 const BOSSCostModel *pcmgpdb,
						 const SCostingInfo *pci);

	// cost of hash agg
	static CCost CostHashAgg(CMemoryPool *mp, CExpressionHandle &exprhdl,
							 const BOSSCostModel *pcmgpdb,
							 const SCostingInfo *pci);


	// cost of scalar agg
	static CCost CostScalarAgg(CMemoryPool *mp, CExpressionHandle &exprhdl,
							   const BOSSCostModel *pcmgpdb,
							   const SCostingInfo *pci);

	// cost of stream agg
	static CCost CostStreamAgg(CMemoryPool *mp, CExpressionHandle &exprhdl,
							   const BOSSCostModel *pcmgpdb,
							   const SCostingInfo *pci);

	// cost of sequence
	static CCost CostSequence(CMemoryPool *mp, CExpressionHandle &exprhdl,
							  const BOSSCostModel *pcmgpdb,
							  const SCostingInfo *pci);

	// cost of sort
	static CCost CostSort(CMemoryPool *mp, CExpressionHandle &exprhdl,
						  const BOSSCostModel *pcmgpdb,
						  const SCostingInfo *pci);

	// cost of TVF
	static CCost CostTVF(CMemoryPool *mp, CExpressionHandle &exprhdl,
						 const BOSSCostModel *pcmgpdb,
						 const SCostingInfo *pci);

	// cost of UnionAll
	static CCost CostUnionAll(CMemoryPool *mp, CExpressionHandle &exprhdl,
							  const BOSSCostModel *pcmgpdb,
							  const SCostingInfo *pci);

	// cost of hash join
	static CCost CostHashJoin(CMemoryPool *mp, CExpressionHandle &exprhdl,
							  const BOSSCostModel *pcmgpdb,
							  const SCostingInfo *pci);

	// cost of merge join
	static CCost CostMergeJoin(CMemoryPool *mp, CExpressionHandle &exprhdl,
							   const BOSSCostModel *pcmgpdb,
							   const SCostingInfo *pci);

	// cost of nljoin
	static CCost CostNLJoin(CMemoryPool *mp, CExpressionHandle &exprhdl,
							const BOSSCostModel *pcmgpdb,
							const SCostingInfo *pci);

	// cost of inner or outer index-nljoin
	static CCost CostIndexNLJoin(CMemoryPool *mp, CExpressionHandle &exprhdl,
								 const BOSSCostModel *pcmgpdb,
								 const SCostingInfo *pci);

	// cost of motion
	static CCost CostMotion(CMemoryPool *mp, CExpressionHandle &exprhdl,
							const BOSSCostModel *pcmgpdb,
							const SCostingInfo *pci);

	// cost of bitmap scan when the NDV is small
	static CCost CostBitmapSmallNDV(const BOSSCostModel *pcmgpdb,
									const SCostingInfo *pci, CDouble dNDV, CEngineSpec::EEngineType engine);

	// cost of bitmap scan when the NDV is large
	static CCost CostBitmapLargeNDV(const BOSSCostModel *pcmgpdb,
									const SCostingInfo *pci, CDouble dNDV, CEngineSpec::EEngineType engine);

	// cost of engine transform
	static CCost CostEngineTransform(CMemoryPool *mp, CExpressionHandle &exprhdl,
									 const BOSSCostModel *pcmgpdb,
									 const SCostingInfo *pci);
	
	static CEngineSpec::EEngineType GetEngineType(CMemoryPool *mp,CExpressionHandle &exprhdl);


};	// class BOSSCostModel

}  // namespace gpdbcost


// EOF
