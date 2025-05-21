#pragma once

#include "gpoptextender/CostModel/BOSSCostModel.hpp"
#include "gpopt/xforms/CXform.h"
#include "gpopt/xforms/CXformFactory.h"
#include <unordered_set>

namespace orcaextender {
using namespace gpos;
using namespace gpopt;
using namespace gpmd;

class DynamicRegistry {   
  private:
    typedef std::function<CCost(CMemoryPool*, CExpressionHandle&, const BOSSCostModel*, const ICostModel::SCostingInfo*)> FnCost;
    typedef std::function<COperator*(void*)> FnOperatorFactory;

    DynamicRegistry(BOSSCostModel* costModel);
    static DynamicRegistry* s_pInstance;

    COperator::EOperatorId currentOperatorId;
    CXform::EXformId currentTransformId;
    BOSSCostModel* costModel;
    CEngineSpec::EEngineType currentEngineType;

	// First, create a hash struct for the pair
	struct EngineStringPairHash {
			std::size_t operator()(const std::pair<CEngineSpec::EEngineType, std::string>& p) const {
					// Combine the hashes of both enum values
					// This is a simple but effective way to hash a pair
					std::size_t h1 = std::hash<int>{}(static_cast<int>(p.first));
					std::size_t h2 = std::hash<std::string>{}(p.second);
					
					// Combine the hashes - a common technique is to shift one and XOR
					return h1 ^ (h2 << 1);
			}
	};


    std::unordered_map<std::pair<CEngineSpec::EEngineType, std::string>, COperator::EOperatorId, EngineStringPairHash> opEngineAndNameToOperatorId = {};
    std::unordered_map<std::string, CXform::EXformId> transformNameToTransformId = {};
    std::unordered_map<std::string, CEngineSpec::EEngineType> engineNameToEngineType = {};
    std::unordered_set<COperator::EOperatorId> passThroughOperators = {};
    std::unordered_set<COperator::EOperatorId> projectOperators = {};

    std::unordered_map<COperator::EOperatorId, std::vector<CXform::EXformId>> relevantTransforms = {};
    std::unordered_map<CXform::EXformId, std::vector<FnOperatorFactory>> opFactories = {};
    std::unordered_map<std::string, std::vector<std::string>> engineToOperatorNames = {}; // for querying.
  public:
    static DynamicRegistry* GetInstance();
    static void Init(CMemoryPool* mp, BOSSCostModel* costModel);

    ~DynamicRegistry();

    void RegisterPhysicalOperator(const std::string& opName, CEngineSpec::EEngineType engine, FnCost costFunc, bool isPassThrough = false);
    void RegisterLogicalOperator(const std::string& opName, CEngineSpec::EEngineType engine, bool isAProject = false);
    void RegisterTransform(const std::string& transformName, CXform* transform);
    void RegisterEngine(const std::string& engineName);
    
    void HookOpToTransform(CXform::EXformId transformId, FnOperatorFactory opFactory);
    void HookTransformToOp(COperator::EOperatorId opId, CXform::EXformId transformId);

    void RegisterCostModelParams(CEngineSpec::EEngineType engine, ICostModelParams* pcp) {
      costModel->RegisterCostModelParams(engine, pcp);
    };

    void RegisterEngineTransform(CEngineSpec::EEngineType from, CEngineSpec::EEngineType to, FnCost fn_cost) {
      costModel->RegisterEngineTransform(from, to, fn_cost);
    };


    COperator::EOperatorId GetOperatorId(CEngineSpec::EEngineType engine, const std::string& opName, bool throwError = true);
    std::vector<COperator*> GetRelevantOperatorsForTransform(CXform::EXformId transformId, void* args);
    std::vector<CXform::EXformId> GetRelevantTransformsForOperator(COperator::EOperatorId opId);
    CXform::EXformId GetTransformId(const std::string& transformName, bool throwError = true);
    CEngineSpec::EEngineType GetEngineType(const std::string& engineName, bool throwError = true);
    std::unordered_map<std::string, std::vector<std::string>>& GetAllOperators() { return engineToOperatorNames; };
    std::unordered_set<COperator::EOperatorId> GetPassThroughOperators() { return passThroughOperators; };
    std::unordered_set<COperator::EOperatorId> GetProjectOperators() { return projectOperators; };

};
}  // namespace orcaextender