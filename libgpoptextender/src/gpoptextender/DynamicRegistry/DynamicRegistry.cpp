#include "gpoptextender/DynamicRegistry/DynamicRegistry.hpp"


using namespace orcaextender;

DynamicRegistry* DynamicRegistry::s_pInstance = nullptr;

DynamicRegistry::DynamicRegistry(BOSSCostModel* costModel) : costModel(costModel) {
  currentOperatorId = COperator::EopDynamicStart;
  currentTransformId = CXform::ExfDynamicStart;
  currentEngineType = CEngineSpec::EetDynamicStart;
  boss2cexpressionConverters[DefaultTranslatorName] = std::make_unique<bosstocexpression::BOSSToCExpressionConverter<bosstocexpression::EmptyStruct, bosstocexpression::EmptyStruct, bosstocexpression::EmptyStruct, bosstocexpression::ColRefMap>>();
  cexpression2bossConverters[DefaultTranslatorName] = std::make_unique<cexpressiontoboss::CExpressionToBOSSConverter<cexpressiontoboss::translation::EmptyStruct, cexpressiontoboss::translation::ProjectInfo, cexpressiontoboss::translation::ColSet, cexpressiontoboss::translation::EmptyStruct>>();
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


void DynamicRegistry::RegisterPhysicalOperator(const std::string& opName, CEngineSpec::EEngineType engine, FnCost costFunc) {
  currentOperatorId = (COperator::EOperatorId) (currentOperatorId + 1);
  costModel->RegisterCostFunction(currentOperatorId, costFunc);
  opEngineAndNameToOperatorId[std::make_pair(engine, opName)] = currentOperatorId;
}

void DynamicRegistry::RegisterLogicalOperator(const std::string& opName, CEngineSpec::EEngineType engine, bool isAProject) {
  currentOperatorId = (COperator::EOperatorId) (currentOperatorId + 1);
  opEngineAndNameToOperatorId[std::make_pair(engine, opName)] = currentOperatorId;
  if (isAProject) {
    projectOperators.insert(currentOperatorId);
  }
}

void DynamicRegistry::HookOpToTransform(CXform::EXformId transformId, FnOperatorFactory opFactory) {
  opFactories[transformId].push_back(opFactory);
}

void DynamicRegistry::HookTransformToOp(COperator::EOperatorId opId, CXform::EXformId transformId) {
  relevantTransforms[opId].push_back(transformId);
}

std::vector<COperator*> DynamicRegistry::GetRelevantOperatorsForTransform(CXform::EXformId transformId, DynamicOperatorArgs& args) {
  std::vector<COperator*> operators;
  for (auto& factory : opFactories[transformId]) {
    operators.push_back(factory(args));
  }
  return operators;
}

COperator::EOperatorId DynamicRegistry::GetOperatorId(CEngineSpec::EEngineType engine, const std::string& opName, bool throwError) {
  if (opEngineAndNameToOperatorId.find(std::make_pair(engine, opName)) == opEngineAndNameToOperatorId.end()) {
    if (throwError) {
      std::cerr << "Operator " << opName << " not found" << std::endl;
      throw std::runtime_error("Operator not found");
    } else {
      return COperator::EopSentinel;
    }
  }
  return opEngineAndNameToOperatorId[std::make_pair(engine, opName)];
}

void DynamicRegistry::RegisterTransform(const std::string& transformName, CXform* transform) {
  currentTransformId = (CXform::EXformId) (currentTransformId + 1);
  transformNameToTransformId[transformName] = currentTransformId;
  CXformFactory::Pxff()->Add(transform);
}

CXform::EXformId DynamicRegistry::GetTransformId(const std::string& transformName, bool throwError) {
  if (transformNameToTransformId.find(transformName) == transformNameToTransformId.end()) {
    if (throwError) {
      std::cerr << "Transform " << transformName << " not found" << std::endl;
      throw std::runtime_error("Transform not found");
    } else {
      return CXform::ExfSentinel;
    }
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

CEngineSpec::EEngineType DynamicRegistry::GetEngineType(const std::string& engineName, bool throwError) {
  if (engineNameToEngineType.find(engineName) == engineNameToEngineType.end()) {
    if (throwError) {
      std::cerr << "Engine " << engineName << " not found" << std::endl;
      throw std::runtime_error("Engine not found");
    } else {
      return CEngineSpec::EetSentinel;
    }
  }
  return engineNameToEngineType[engineName];
}

void DynamicRegistry::AddTransformsToXFormSet(COperator::EOperatorId opId, CXformSet* xformSet) {
  for (auto& transformId : relevantTransforms[opId]) {
    xformSet->ExchangeSet(transformId);
  }
}
