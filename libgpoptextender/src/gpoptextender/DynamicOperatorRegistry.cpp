#include "gpoptextender/DynamicOperatorRegistry.hpp"
using namespace orcaextender;

DynamicOperatorRegistry* DynamicOperatorRegistry::s_pInstance = nullptr;

DynamicOperatorRegistry::DynamicOperatorRegistry(BOSSCostModel* costModel) : costModel(costModel) {
  currentOperatorId = COperator::EopDynamicStart;
  currentTransformId = CXform::ExfDynamicStart;
  currentEngineType = CEngineSpec::EetDynamicStart;
  // TODO: populate with default (orca) operators and transforms.
}

DynamicOperatorRegistry::~DynamicOperatorRegistry() {
  costModel->Release();
}

DynamicOperatorRegistry* DynamicOperatorRegistry::GetInstance() {
  return s_pInstance;
}

void DynamicOperatorRegistry::Init(CMemoryPool* mp, BOSSCostModel* costModel) {
  s_pInstance = GPOS_NEW(mp) DynamicOperatorRegistry(costModel);
}


void DynamicOperatorRegistry::RegisterOperator(const std::string& opName, CEngineSpec::EEngineType engine, FnCost costFunc, std::vector<CXform::EXformId>& relevantTransforms, FnOperatorFactory opFactory) {
  currentOperatorId = (COperator::EOperatorId) (currentOperatorId + 1);
  costModel->RegisterCostFunction(currentOperatorId, costFunc);
  opEngineAndNameToOperatorId[std::make_pair(engine, opName)] = currentOperatorId;
  for (auto& transformId : relevantTransforms) {
    opFactories[transformId].push_back(opFactory);
  }
}

std::vector<COperator*> DynamicOperatorRegistry::GetRelevantOperatorsForTransform(CXform::EXformId transformId, void* opaqueArgs) {
  std::vector<COperator*> operators;
  for (auto& factory : opFactories[transformId]) {
    operators.push_back(factory(opaqueArgs));
  }
  return operators;
}

COperator::EOperatorId DynamicOperatorRegistry::GetOperatorId(CEngineSpec::EEngineType engine, const std::string& opName) {
  if (opEngineAndNameToOperatorId.find(std::make_pair(engine, opName)) == opEngineAndNameToOperatorId.end()) {
    return COperator::EopSentinel;
  }
  return opEngineAndNameToOperatorId[std::make_pair(engine, opName)];
}

void DynamicOperatorRegistry::RegisterTransform(const std::string& transformName, std::vector<COperator::EOperatorId>& relevantOperators, CXform* transform) {
  currentTransformId = (CXform::EXformId) (currentTransformId + 1);
  transformNameToTransformId[transformName] = currentTransformId;
  for (auto& operatorId : relevantOperators) {
    relevantTransforms[operatorId].push_back(currentTransformId);
  }
  CXformFactory::Pxff()->Add(transform);
}

CXform::EXformId DynamicOperatorRegistry::GetTransformId(const std::string& transformName) {
  if (transformNameToTransformId.find(transformName) == transformNameToTransformId.end()) {
    return CXform::ExfSentinel;
  }
  return transformNameToTransformId[transformName];
}

std::vector<CXform::EXformId> DynamicOperatorRegistry::GetRelevantTransformsForOperator(COperator::EOperatorId opId) {
  return relevantTransforms[opId];
}

void DynamicOperatorRegistry::RegisterEngine(const std::string& engineName) {
  currentEngineType = (CEngineSpec::EEngineType) (currentEngineType + 1);
  engineToOperatorNames[engineName] = std::vector<std::string>();
  engineNameToEngineType[engineName] = currentEngineType;
}

CEngineSpec::EEngineType DynamicOperatorRegistry::GetEngineType(const std::string& engineName) {
  if (engineNameToEngineType.find(engineName) == engineNameToEngineType.end()) {
    return CEngineSpec::EEngineType::EetSentinel;
  }
  return engineNameToEngineType[engineName];
}
