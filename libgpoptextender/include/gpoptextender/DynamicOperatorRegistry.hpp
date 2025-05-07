#pragma once

#include "gpoptextender/BOSSCostModel.hpp"
#include "gpopt/xforms/CXform.h"
#include "gpopt/xforms/CXformFactory.h"


namespace orcaextender {
using namespace gpos;
using namespace gpopt;
using namespace gpmd;

class DynamicOperatorRegistry {   
  private:
    typedef std::function<CCost(CMemoryPool*, CExpressionHandle&, const BOSSCostModel*, const ICostModel::SCostingInfo*)> FnCost;
    typedef std::function<COperator*(void*)> FnOperatorFactory;

    DynamicOperatorRegistry(BOSSCostModel* costModel);
    static DynamicOperatorRegistry* s_pInstance;

    COperator::EOperatorId currentOperatorId;
    CXform::EXformId currentTransformId;
    BOSSCostModel* costModel;

    std::unordered_map<std::string, COperator::EOperatorId> opNameToOperatorId;
    std::unordered_map<std::string, CXform::EXformId> transformNameToTransformId;
    std::unordered_map<std::string, std::vector<CXform::EXformId>> relevantTransforms;
    std::unordered_map<std::string, std::vector<FnOperatorFactory>> opFactories;
  public:
    static DynamicOperatorRegistry* GetInstance();
    static DynamicOperatorRegistry* Init(CMemoryPool* mp, BOSSCostModel* costModel);

    ~DynamicOperatorRegistry();

    void RegisterOperator(const std::string& opName, FnCost costFunc, const std::string& similarOpName, FnOperatorFactory opFactory);
    std::vector<COperator*> GetOperators(const std::string& opName, void* args);
    COperator::EOperatorId GetOperatorId(const std::string& opName);

    void RegisterTransform(const std::string& transformName, const std::string& transformFrom, CXform* transform);
    CXform::EXformId GetTransformId(const std::string& transformName);
    std::vector<CXform::EXformId> GetRelevantTransforms(const std::string& transformFrom);
};
}  // namespace orcaextender