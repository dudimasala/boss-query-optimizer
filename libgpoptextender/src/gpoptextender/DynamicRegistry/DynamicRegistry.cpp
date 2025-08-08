#include "gpoptextender/DynamicRegistry/DynamicRegistry.hpp"
#include "BOSSToCExpressionDefault.hpp"
#include "CExpressionToBOSSDefault.hpp"

using namespace orcaextender;

DynamicRegistry* DynamicRegistry::s_pInstance = nullptr;

DynamicRegistry::DynamicRegistry(BOSSCostModel* costModel) : costModel(costModel) {
  currentOperatorId = COperator::EopDynamicStart;
  currentTransformId = CXform::ExfDynamicStart;
  currentEngineType = EEngineType::EetDynamicStart;
  currentMDIdType = IMDId::EmdidDynamicStart;
  boss2cexpressionConverters[DefaultTranslatorName] = std::make_unique<bosstocexpression::BOSSToCExpressionDefaultConverter>();
  cexpression2bossConverters[DefaultTranslatorName] = std::make_unique<cexpressiontoboss::CExpressionToBOSSDefaultConverter>();
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


void DynamicRegistry::RegisterPhysicalOperator(const std::string& opName, EEngineType engine, FnCost costFunc) {
  currentOperatorId = (COperator::EOperatorId) (currentOperatorId + 1);
  costModel->RegisterCostFunction(currentOperatorId, costFunc);
  opEngineAndNameToOperatorId[std::make_pair(engine, opName)] = currentOperatorId;
  engineToPhysicalOps[engine].push_back(opName);
}

void DynamicRegistry::RegisterLogicalOperator(const std::string& opName, EEngineType engine, bool isAProject) {
  currentOperatorId = (COperator::EOperatorId) (currentOperatorId + 1);
  opEngineAndNameToOperatorId[std::make_pair(engine, opName)] = currentOperatorId;
  if (isAProject) {
    projectOperators.insert(currentOperatorId);
  }
  engineToLogicalOps[engine].push_back(opName);
}

void DynamicRegistry::HookOpToTransform(CXform::EXformId transformId, COperator::EOperatorId opId, FnOperatorFactory opFactory) {
  opFactories[transformId].push_back(std::make_pair(opId, opFactory));
}

void DynamicRegistry::HookTransformToOp(COperator::EOperatorId opId, CXform::EXformId transformId) {
  relevantTransforms[opId].push_back(transformId);
}

std::vector<COperator*> DynamicRegistry::GetRelevantOperatorsForTransform(CXform::EXformId transformId, DynamicOperatorArgs& args) {
  std::vector<COperator*> operators;
  for (auto& factory : opFactories[transformId]) {
    operators.push_back(factory.second(args));
  }
  return operators;
}

// probably shoudln't need engine to be passed in here.
COperator::EOperatorId DynamicRegistry::GetOperatorId(EEngineType engine, const std::string& opName, bool throwError) {
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

void DynamicRegistry::RegisterTransform(const std::string& transformName, EEngineType engine, CXform* transform) {
  currentTransformId = (CXform::EXformId) (currentTransformId + 1);
  transformNameToTransformId[transformName] = currentTransformId;
  CXformFactory::Pxff()->Add(transform);
  engineToTransforms[engine].push_back(transformName);
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
  currentEngineType = (EEngineType) (currentEngineType + 1);
  engineNameToEngineType[engineName] = currentEngineType;
  engineTypeToEngineName[currentEngineType] = engineName;
  currentMDIdType = (IMDId::EMDIdType) (currentMDIdType + 1);
  engineToMDIdType[currentEngineType] = std::make_pair(currentMDIdType, engineName); // no need to store engineName. Can be retrieved from engineType to engineName map.
}

EEngineType DynamicRegistry::GetEngineType(const std::string& engineName, bool throwError) {
  if (engineNameToEngineType.find(engineName) == engineNameToEngineType.end()) {
    if (throwError) {
      throw std::runtime_error("Engine not found");
    } else {
      return EEngineType::EetSentinel;
    }
  }
  return engineNameToEngineType[engineName];
}

void DynamicRegistry::AddTransformsToXFormSet(COperator::EOperatorId opId, CXformSet* xformSet) {
  for (auto& transformId : relevantTransforms[opId]) {
    xformSet->ExchangeSet(transformId);
  }
}


// using PreserveMap  = std::unordered_map<std::pair<EEngineType, EEngineType>, bool, EnginePairHash>;
void DynamicRegistry::removeEngineFromMap(PreserveMap &m, EEngineType engineId) {
    for (auto it = m.begin(); it != m.end(); ) {
        const std::pair<EEngineType, EEngineType> &key = it->first;
        if (key.first == engineId || key.second == engineId) {
            it = m.erase(it);
        } else {
            ++it;
        }
    }
}



void DynamicRegistry::RemoveEngine(EEngineType engine) {
  // remove Engine related 
  engineTypeToEngineName.erase(engine);
  engineToMDIdType.erase(engine);

  // engine preserve order.
  removeEngineFromMap(enginePreserveOrder, engine);

  // engine preserve dist.
  removeEngineFromMap(enginePreserveDistribution, engine);

  // engine preserve rewind
  removeEngineFromMap(enginePreserveRewindability, engine);


  // remove transform / cost model params for engine
  costModel->RemoveEngineTransform(engine);
  costModel->RemoveCostModelParams(engine);

  // remove engine-related ops (includes removing relevant costs)
  for (auto& op : engineToPhysicalOps[engine]) {
    auto p = std::make_pair(engine, op);
    COperator::EOperatorId id = opEngineAndNameToOperatorId[p];
    RemovePhysicalOperator(id);
    opEngineAndNameToOperatorId.erase(p);
    for (auto & [xformId, factories] : opFactories) {
      factories.erase(
          std::remove_if(
              factories.begin(),
              factories.end(),
              [id](const std::pair<COperator::EOperatorId, FnOperatorFactory> &entry) {
                  return entry.first == id;
              }
          ),
          factories.end()
      );
    }
  }

  engineToPhysicalOps.erase(engine);

  for (auto& op : engineToLogicalOps[engine]) {
    auto p = std::make_pair(engine, op);
    COperator::EOperatorId id = opEngineAndNameToOperatorId[p];
    RemoveLogicalOperator(id);
    opEngineAndNameToOperatorId.erase(p);
    relevantTransforms.erase(id);
    for (auto & [xformId, factories] : opFactories) {
        factories.erase(
            std::remove_if(
                factories.begin(),
                factories.end(),
                [id](const std::pair<COperator::EOperatorId, FnOperatorFactory> &entry) {
                    return entry.first == id;
                }
            ),
            factories.end()
        );
    }
  }
  engineToLogicalOps.erase(engine);

  // remove engine-related transforms
  for (auto& transform : engineToTransforms[engine]) {
    CXform::EXformId id = transformNameToTransformId[transform];
    RemoveTransform(id);
    transformNameToTransformId.erase(transform);
    opFactories.erase(id);

    // remove from relevant transforms
    for (auto & [opId, vecOfTransforms] : relevantTransforms) {
        // vecOfTransforms is a std::vector<CXform::EXformId>&
        vecOfTransforms.erase(
            std::remove_if(
                vecOfTransforms.begin(),
                vecOfTransforms.end(),
                [id](const CXform::EXformId &transformId) {
                    return transformId == id;
                }
            ),
            vecOfTransforms.end()
        );
    }

  }
  engineToTransforms.erase(engine);

  // remove engine-related preprocessing rules
  for (auto& rule : engineToPreprocessingRules[engine]) {
    RemovePreprocessingRule(rule);
  }

  engineToPreprocessingRules.erase(engine);

  // remove engine-related translators
  for (auto [converter, translator] : engineToB2CTranslators[engine]) {
    boss2cexpressionConverters[converter]->RemoveTranslator(translator);
  }

  for (auto [converter, translator] : engineToC2BTranslators[engine]) {
    cexpression2bossConverters[converter]->RemoveTranslator(translator);
  }

   for (auto [converter, translator] : engineToB2CScalarTranslators[engine]) {
    boss2cexpressionConverters[converter]->RemoveScalarTranslator(translator);
  }

  for (auto [converter, translator] : engineToC2BScalarTranslators[engine]) {
    cexpression2bossConverters[converter]->RemoveScalarTranslator(translator);
  }

  engineToB2CTranslators.erase(engine);
  engineToC2BTranslators.erase(engine);
  engineToB2CScalarTranslators.erase(engine);
  engineToC2BScalarTranslators.erase(engine);

  // remove engine-related converters (is there a point of deleting this?)
  // for (auto& converter : engineTob2cConverters[engine]) {
  //   boss2cexpressionConverters.erase(converter);
  // }

  // engineTob2cConverters.erase(engine);

  // for (auto& converter : engineToc2bConverters[engine]) {
  //   cexpression2bossConverters.erase(converter);
  // }

  // engineToc2bConverters.erase(engine);

  
}
