#include "gpoptextender/DynamicOperatorRegistry.hpp"
using namespace orcaextender;

DynamicOperatorRegistry* DynamicOperatorRegistry::s_pInstance = nullptr;

DynamicOperatorRegistry::DynamicOperatorRegistry(BOSSCostModel* costModel) : costModel(costModel) {
  currentOperatorId = COperator::EopDynamicStart;
  currentTransformId = CXform::ExfDynamicStart;
}

DynamicOperatorRegistry::~DynamicOperatorRegistry() {
  costModel->Release();
}

DynamicOperatorRegistry* DynamicOperatorRegistry::GetInstance() {
  return s_pInstance;
}

DynamicOperatorRegistry* DynamicOperatorRegistry::Init(CMemoryPool* mp, BOSSCostModel* costModel) {
  if (s_pInstance == nullptr) {
    s_pInstance = GPOS_NEW(mp) DynamicOperatorRegistry(costModel);
  }
  return s_pInstance;
}


void DynamicOperatorRegistry::RegisterOperator(const std::string& opName, FnCost costFunc) {
  currentOperatorId = (COperator::EOperatorId) (currentOperatorId + 1);
  costModel->RegisterCostFunction(currentOperatorId, costFunc);
  opNameToOperatorId[opName] = currentOperatorId;
}

COperator::EOperatorId DynamicOperatorRegistry::GetOperatorId(const std::string& opName) {
  return opNameToOperatorId[opName];
}

void DynamicOperatorRegistry::RegisterTransform(const std::string& transformName, const std::string& transformFrom, CXform* transform) {
  currentTransformId = (CXform::EXformId) (currentTransformId + 1);
  transformNameToTransformId[transformName] = currentTransformId;
  relevantTransforms[transformFrom].push_back(currentTransformId);
  CXformFactory::Pxff()->Add(transform);
}

CXform::EXformId DynamicOperatorRegistry::GetTransformId(const std::string& transformName) {
  return transformNameToTransformId[transformName];
}

std::vector<CXform::EXformId> DynamicOperatorRegistry::GetRelevantTransforms(const std::string& transformFrom) {
  return relevantTransforms[transformFrom];
}
