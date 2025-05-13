#pragma once

#include "gpoptextender/CostModel/BOSSCostModel.hpp"
#include "gpopt/xforms/CXform.h"
#include "gpopt/xforms/CXformFactory.h"


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

    std::unordered_map<COperator::EOperatorId, std::vector<CXform::EXformId>> relevantTransforms = {};
    std::unordered_map<CXform::EXformId, std::vector<FnOperatorFactory>> opFactories = {};
    std::unordered_map<std::string, std::vector<std::string>> engineToOperatorNames = {}; // for querying.
  public:
    static DynamicRegistry* GetInstance();
    static void Init(CMemoryPool* mp, BOSSCostModel* costModel);

    ~DynamicRegistry();

    void RegisterOperator(const std::string& opName, CEngineSpec::EEngineType engine, FnCost costFunc, std::vector<CXform::EXformId>& relevantTransforms, FnOperatorFactory opFactory);
    void RegisterTransform(const std::string& transformName, std::vector<COperator::EOperatorId>& relevantOperators, CXform* transform);
    void RegisterEngine(const std::string& engineName);

    void RegisterCostModelParams(CEngineSpec::EEngineType engine, ICostModelParams* pcp) {
      costModel->RegisterCostModelParams(engine, pcp);
    };


    COperator::EOperatorId GetOperatorId(CEngineSpec::EEngineType engine, const std::string& opName);
    std::vector<COperator*> GetRelevantOperatorsForTransform(CXform::EXformId transformId, void* args);
    std::vector<CXform::EXformId> GetRelevantTransformsForOperator(COperator::EOperatorId opId);
    CXform::EXformId GetTransformId(const std::string& transformName);
    CEngineSpec::EEngineType GetEngineType(const std::string& engineName);
    std::unordered_map<std::string, std::vector<std::string>>& GetAllOperators() { return engineToOperatorNames; };

};
}  // namespace orcaextender