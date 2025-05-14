#include "gpoptextender/DynamicRegistry/DynamicRegistry.hpp"
using namespace orcaextender;

DynamicRegistry* DynamicRegistry::s_pInstance = nullptr;

DynamicRegistry::DynamicRegistry(BOSSCostModel* costModel) : costModel(costModel) {
  currentOperatorId = COperator::EopDynamicStart;
  currentTransformId = CXform::ExfDynamicStart;
  currentEngineType = CEngineSpec::EetDynamicStart;
  // TODO: populate with default (orca) operators and transforms.
}

DynamicRegistry::~DynamicRegistry() {
  costModel->Release();
}

DynamicRegistry* DynamicRegistry::GetInstance() {
  return s_pInstance;
}

void DynamicRegistry::Init(CMemoryPool* mp, BOSSCostModel* costModel) {
  s_pInstance = GPOS_NEW(mp) DynamicRegistry(costModel);
}


void DynamicRegistry::RegisterOperator(const std::string& opName, CEngineSpec::EEngineType engine, FnCost costFunc, std::vector<CXform::EXformId>& relevantTransforms, FnOperatorFactory opFactory) {
  currentOperatorId = (COperator::EOperatorId) (currentOperatorId + 1);
  costModel->RegisterCostFunction(currentOperatorId, costFunc);
  opEngineAndNameToOperatorId[std::make_pair(engine, opName)] = currentOperatorId;
  for (auto& transformId : relevantTransforms) {
    opFactories[transformId].push_back(opFactory);
  }
}

std::vector<COperator*> DynamicRegistry::GetRelevantOperatorsForTransform(CXform::EXformId transformId, void* opaqueArgs) {
  std::vector<COperator*> operators;
  for (auto& factory : opFactories[transformId]) {
    operators.push_back(factory(opaqueArgs));
  }
  return operators;
}

COperator::EOperatorId DynamicRegistry::GetOperatorId(CEngineSpec::EEngineType engine, const std::string& opName) {
  if (opEngineAndNameToOperatorId.find(std::make_pair(engine, opName)) == opEngineAndNameToOperatorId.end()) {
    return COperator::EopSentinel;
  }
  return opEngineAndNameToOperatorId[std::make_pair(engine, opName)];
}

void DynamicRegistry::RegisterTransform(const std::string& transformName, std::vector<COperator::EOperatorId>& relevantOperators, CXform* transform) {
  currentTransformId = (CXform::EXformId) (currentTransformId + 1);
  transformNameToTransformId[transformName] = currentTransformId;
  for (auto& operatorId : relevantOperators) {
    relevantTransforms[operatorId].push_back(currentTransformId);
  }

  CXformFactory::Pxff()->Add(transform);
}

CXform::EXformId DynamicRegistry::GetTransformId(const std::string& transformName) {
  if (transformNameToTransformId.find(transformName) == transformNameToTransformId.end()) {
    return CXform::ExfSentinel;
  }
  return transformNameToTransformId[transformName];
}

std::vector<CXform::EXformId> DynamicRegistry::GetRelevantTransformsForOperator(COperator::EOperatorId opId) {
  return relevantTransforms[opId];
}

void DynamicRegistry::RegisterEngine(const std::string& engineName) {
  currentEngineType = (CEngineSpec::EEngineType) (currentEngineType + 1);
  engineToOperatorNames[engineName] = std::vector<std::string>();
  engineNameToEngineType[engineName] = currentEngineType;
}

CEngineSpec::EEngineType DynamicRegistry::GetEngineType(const std::string& engineName) {
  if (engineNameToEngineType.find(engineName) == engineNameToEngineType.end()) {
    return CEngineSpec::EEngineType::EetSentinel;
  }
  return engineNameToEngineType[engineName];
}
