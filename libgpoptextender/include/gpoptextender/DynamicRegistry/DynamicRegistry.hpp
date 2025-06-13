#pragma once

#include "gpoptextender/CostModel/BOSSCostModel.hpp"
#include "gpopt/xforms/CXform.h"
#include "gpopt/xforms/CXformFactory.h"
#include "gpoptextender/DynamicRegistry/DynamicOperatorArgs.hpp"
#include "gpoptextender/Translation/B2CConverter.hpp"
#include "gpoptextender/Translation/C2BConverter.hpp"
#include "BOSSToCExpression.hpp"
#include "CExpressionToBOSS.hpp"
#include "gpopt/operators/CNormalizer.h"
#include "b2cDefaultTypes.hpp"
#include "c2bDefaultTypes.hpp"
#include <unordered_set>

namespace orcaextender {
using namespace gpos;
using namespace gpopt;
using namespace gpmd;

class DynamicRegistry {   
  private:
    typedef std::function<CCost(CMemoryPool*, CExpressionHandle&, const BOSSCostModel*, const ICostModel::SCostingInfo*)> FnCost;
    typedef std::function<COperator*(DynamicOperatorArgs&)> FnOperatorFactory;
    typedef CExpression *(*PreprocessingRule)(CMemoryPool *, CExpression *);

    DynamicRegistry(BOSSCostModel* costModel);
    static DynamicRegistry* s_pInstance;

    COperator::EOperatorId currentOperatorId;
    CXform::EXformId currentTransformId;
    BOSSCostModel* costModel;
    CEngineSpec::EEngineType currentEngineType;
    IMDId::EMDIdType currentMDIdType;


    std::map<std::string, std::unique_ptr<B2CConverter>> boss2cexpressionConverters;
    std::map<std::string, std::unique_ptr<C2BConverter>> cexpression2bossConverters;

    std::string defaultB2CConverterName = DefaultTranslatorName;
    std::string defaultC2BConverterName = DefaultTranslatorName;

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

  struct EnginePairHash {
    std::size_t operator()(const std::pair<CEngineSpec::EEngineType, CEngineSpec::EEngineType>& p) const {
      return std::hash<int>{}(static_cast<int>(p.first)) ^ (std::hash<int>{}(static_cast<int>(p.second)) << 1);
    }
  };

  using PreserveMap  = std::unordered_map<std::pair<CEngineSpec::EEngineType, CEngineSpec::EEngineType>, bool, EnginePairHash>;


    std::unordered_map<std::pair<CEngineSpec::EEngineType, std::string>, COperator::EOperatorId, EngineStringPairHash> opEngineAndNameToOperatorId = {};
    std::unordered_map<std::string, CXform::EXformId> transformNameToTransformId = {};
    std::unordered_map<std::string, CEngineSpec::EEngineType> engineNameToEngineType = {};
    std::unordered_map<CEngineSpec::EEngineType, std::string> engineTypeToEngineName = {};
    std::unordered_map<std::string, PreprocessingRule> preprocessingRules = {};
    std::unordered_set<COperator::EOperatorId> projectOperators = {};

    std::unordered_map<COperator::EOperatorId, std::vector<CXform::EXformId>> relevantTransforms = {};
    std::unordered_map<CXform::EXformId, std::vector<std::pair<COperator::EOperatorId, FnOperatorFactory>>> opFactories = {};
    std::unordered_map<std::pair<CEngineSpec::EEngineType, CEngineSpec::EEngineType>, bool, EnginePairHash> enginePreserveOrder = {};
    std::unordered_map<std::pair<CEngineSpec::EEngineType, CEngineSpec::EEngineType>, bool, EnginePairHash> enginePreserveDistribution = {};
    std::unordered_map<std::pair<CEngineSpec::EEngineType, CEngineSpec::EEngineType>, bool, EnginePairHash> enginePreserveRewindability = {};
    std::unordered_map<CEngineSpec::EEngineType, std::pair<IMDId::EMDIdType, std::string>> engineToMDIdType = {};

    std::unordered_map<CEngineSpec::EEngineType, std::vector<std::string>> engineToPhysicalOps;
    std::unordered_map<CEngineSpec::EEngineType, std::vector<std::string>> engineToLogicalOps; 
    std::unordered_map<CEngineSpec::EEngineType, std::vector<std::string>> engineToTransforms;
    // std::unordered_map<CEngineSpec::EEngineType, std::vector<std::string>> engineTob2cConverters;
    // std::unordered_map<CEngineSpec::EEngineType, std::vector<std::string>> engineToc2bConverters;
    std::unordered_map<CEngineSpec::EEngineType, std::vector<std::string>> engineToPreprocessingRules;
    std::unordered_map<CEngineSpec::EEngineType, std::vector<std::pair<std::string, ULONG>>> engineToB2CTranslators;
    std::unordered_map<CEngineSpec::EEngineType, std::vector<std::pair<std::string, ULONG>>> engineToC2BTranslators;
    std::unordered_map<CEngineSpec::EEngineType, std::vector<std::pair<std::string, ULONG>>> engineToB2CScalarTranslators;
    std::unordered_map<CEngineSpec::EEngineType, std::vector<std::pair<std::string, ULONG>>> engineToC2BScalarTranslators;
    


    void RemovePhysicalOperator(COperator::EOperatorId opId) {
      costModel->RemoveCostFunction(opId);
    }

    void RemoveLogicalOperator(COperator::EOperatorId opId) {
      projectOperators.erase(opId);
    }

    void RemoveEngine(CEngineSpec::EEngineType);
    void removeEngineFromMap(PreserveMap &m, CEngineSpec::EEngineType engineId);


  public:
    static DynamicRegistry* GetInstance();
    static void Init(CMemoryPool* mp, BOSSCostModel* costModel);
    inline static const std::string& DefaultTranslatorName = "DEFAULT";

    ~DynamicRegistry();

    void RegisterPhysicalOperator(const std::string& opName, CEngineSpec::EEngineType engine, FnCost costFunc);
    void RegisterLogicalOperator(const std::string& opName, CEngineSpec::EEngineType engine, bool isAProject = false);
    void RegisterTransform(const std::string& transformName, CEngineSpec::EEngineType engine, CXform* transform);
    void RemoveTransform(CXform::EXformId xformId) {
      GPOPT_DISABLE_XFORM(xformId);
    }

    void RegisterEngine(const std::string& engineName);
    
    void HookOpToTransform(CXform::EXformId transformId, COperator::EOperatorId opId, FnOperatorFactory opFactory);
    void HookTransformToOp(COperator::EOperatorId opId, CXform::EXformId transformId);

    void RegisterCostModelParams(CEngineSpec::EEngineType engine, ICostModelParams* pcp) {
      costModel->RegisterCostModelParams(engine, pcp);
    };

    void RegisterEngineTransform(CEngineSpec::EEngineType from, CEngineSpec::EEngineType to, FnCost fn_cost, bool preserve_order = false, bool preserve_distribution = false, bool preserve_rewindability = false) {
      costModel->RegisterEngineTransform(from, to, fn_cost);
      enginePreserveOrder[std::make_pair(from, to)] = preserve_order;
      enginePreserveDistribution[std::make_pair(from, to)] = preserve_distribution;
      enginePreserveRewindability[std::make_pair(from, to)] = preserve_distribution;
    };

    bool GetEnginePreserveOrder(CEngineSpec::EEngineType from, CEngineSpec::EEngineType to) {
      return enginePreserveOrder[std::make_pair(from, to)];
    }

    bool GetEnginePreserveDistribution(CEngineSpec::EEngineType from, CEngineSpec::EEngineType to) {
      return enginePreserveDistribution[std::make_pair(from, to)];
    }

    bool GetEnginePreserveRewindability(CEngineSpec::EEngineType from, CEngineSpec::EEngineType to) {
      return enginePreserveRewindability[std::make_pair(from, to)];
    }

    COperator::EOperatorId GetOperatorId(CEngineSpec::EEngineType engine, const std::string& opName, bool throwError = true);
    std::vector<COperator*> GetRelevantOperatorsForTransform(CXform::EXformId transformId, DynamicOperatorArgs& args);
    std::vector<CXform::EXformId> GetRelevantTransformsForOperator(COperator::EOperatorId opId);
    CXform::EXformId GetTransformId(const std::string& transformName, bool throwError = true);
    CEngineSpec::EEngineType GetEngineType(const std::string& engineName, bool throwError = true);
    std::unordered_map<std::string, std::vector<std::string>> GetAllOperators() { 
      std::unordered_map<std::string, std::vector<std::string>> engineNameToOperatorNames;
      for (auto& [engine, opNames] : engineToPhysicalOps) {
        engineNameToOperatorNames[engineTypeToEngineName[engine]] = opNames;
      }
      return engineNameToOperatorNames; 
    };
    std::unordered_set<COperator::EOperatorId> GetProjectOperators() { return projectOperators; };
    ICostModelParams* GetCostModelParams(CEngineSpec::EEngineType engine) { return costModel->GetCostModelParams(engine); };
    BOSSCostModel* GetCostModel() { return costModel; };

    void AddTransformsToXFormSet(COperator::EOperatorId opId, CXformSet* xformSet);

    template <typename RetAuxType, typename InpAuxType, typename RetScalarAuxType, typename InpScalarAuxType>
    void RegisterBOSS2CExpressionConverter(const std::string& converterName, bosstocexpression::BOSSToCExpressionConverter<RetAuxType, InpAuxType, RetScalarAuxType, InpScalarAuxType>& converter, bool makeDefault=false) { 
      if (converterName == DefaultTranslatorName) {
        throw std::invalid_argument("Default translator name cannot be used for registering a converter.");
      }
      boss2cexpressionConverters[converterName] = converter; 
      if (makeDefault) {
        defaultB2CConverterName = converterName;
      }
    };

    template <typename RetAuxType, typename InpAuxType, typename RetScalarAuxType, typename InpScalarAuxType>
    void RegisterCExpression2BOSSConverter(const std::string& converterName, cexpressiontoboss::CExpressionToBOSSConverter<RetAuxType, InpAuxType, RetScalarAuxType, InpScalarAuxType>& converter, bool makeDefault=false) { 
      if (converterName == DefaultTranslatorName) {
        throw std::invalid_argument("Default translator name cannot be used for registering a converter.");
      }
      cexpression2bossConverters[converterName] = converter; 
      if (makeDefault) {
        defaultC2BConverterName = converterName;
      }
    };

    template <typename RetAuxType, typename InpAuxType, typename RetScalarAuxType, typename InpScalarAuxType>
    cexpressiontoboss::CExpressionToBOSSConverter<RetAuxType, InpAuxType, RetScalarAuxType, InpScalarAuxType>* GetCExpression2BOSSConverter(const std::string& converterName = DefaultTranslatorName) { 
      cexpressiontoboss::CExpressionToBOSSConverter<RetAuxType, InpAuxType, RetScalarAuxType, InpScalarAuxType>* typedConverter = dynamic_cast<cexpressiontoboss::CExpressionToBOSSConverter<RetAuxType, InpAuxType, RetScalarAuxType, InpScalarAuxType>*>(cexpression2bossConverters[converterName].get());
      if (typedConverter == nullptr) {
        std::cerr << "typedConverter is nullptr" << std::endl;
        throw std::invalid_argument("CExpression2BOSS converter is not of the correct type for converter name: " + converterName);
      }
      return typedConverter;
    };

    template <typename RetAuxType, typename InpAuxType, typename RetScalarAuxType, typename InpScalarAuxType>
    bosstocexpression::BOSSToCExpressionConverter<RetAuxType, InpAuxType, RetScalarAuxType, InpScalarAuxType>* GetBOSS2CExpressionConverter(const std::string& converterName = DefaultTranslatorName) { 
      bosstocexpression::BOSSToCExpressionConverter<RetAuxType, InpAuxType, RetScalarAuxType, InpScalarAuxType>* typedConverter = dynamic_cast<bosstocexpression::BOSSToCExpressionConverter<RetAuxType, InpAuxType, RetScalarAuxType, InpScalarAuxType>*>(boss2cexpressionConverters[converterName].get());
      if (typedConverter == nullptr) {
        std::cerr << "typedConverter is nullptr" << std::endl;
        throw std::invalid_argument("BOSS2CExpression converter is not of the correct type for converter name: " + converterName);
      }
      return typedConverter;
    };

    cexpressiontoboss::CExpressionToBOSSConverter<cexpressiontoboss::translation::EmptyStruct, cexpressiontoboss::translation::ProjectInfo, cexpressiontoboss::translation::ColSet, cexpressiontoboss::translation::EmptyStruct>* GetDefaultCExpression2BOSSConverter() { 
      return GetCExpression2BOSSConverter<cexpressiontoboss::translation::EmptyStruct, cexpressiontoboss::translation::ProjectInfo, cexpressiontoboss::translation::ColSet, cexpressiontoboss::translation::EmptyStruct>(DefaultTranslatorName);
    };

    bosstocexpression::BOSSToCExpressionConverter<bosstocexpression::EmptyStruct, bosstocexpression::EmptyStruct, bosstocexpression::EmptyStruct, bosstocexpression::ColRefMap>* GetDefaultBOSS2CExpressionConverter() { 
      return GetBOSS2CExpressionConverter<bosstocexpression::EmptyStruct, bosstocexpression::EmptyStruct, bosstocexpression::EmptyStruct, bosstocexpression::ColRefMap>(DefaultTranslatorName);
    };


    // C2BConverter* GetErasedCExpression2BOSSConverter(const std::string& converterName = DefaultTranslatorName) {
    //   return cexpression2bossConverters[converterName].get();
    // };

    // B2CConverter* GetErasedBOSS2CExpressionConverter(const std::string& converterName = DefaultTranslatorName) {
    //   return boss2cexpressionConverters[converterName].get();
    // };

    C2BConverter* GetDefaultErasedCExpression2BOSSConverter() {
      return cexpression2bossConverters[defaultC2BConverterName].get();
    }

    B2CConverter* GetDefaultErasedBOSS2CExpressionConverter() {
      return boss2cexpressionConverters[defaultB2CConverterName].get();
    }

    template <typename RetAuxType, typename InpAuxType, typename RetScalarAuxType, typename InpScalarAuxType>
    void RegisterBOSS2CExpressionTranslator(std::unique_ptr<bosstocexpression::TranslatorBase<RetAuxType, InpAuxType, RetScalarAuxType, InpScalarAuxType>> translator, CEngineSpec::EEngineType engineType, const std::string& converterName = DefaultTranslatorName) {
      bosstocexpression::BOSSToCExpressionConverter<RetAuxType, InpAuxType, RetScalarAuxType, InpScalarAuxType>* typedConverter = GetBOSS2CExpressionConverter<RetAuxType, InpAuxType, RetScalarAuxType, InpScalarAuxType>(converterName);
      ULONG id = typedConverter->RegisterTranslator(std::move(translator));
      engineToB2CTranslators[engineType].push_back(std::make_pair(converterName, id));
    };

    template <typename RetAuxType, typename InpAuxType, typename RetScalarAuxType, typename InpScalarAuxType>
    void RegisterBOSS2CExpressionScalarTranslator(std::unique_ptr<bosstocexpression::ScalarTranslatorBase<RetAuxType, InpAuxType, RetScalarAuxType, InpScalarAuxType>> translator, CEngineSpec::EEngineType engineType, const std::string& converterName = DefaultTranslatorName) {
      bosstocexpression::BOSSToCExpressionConverter<RetAuxType, InpAuxType, RetScalarAuxType, InpScalarAuxType>* typedConverter = GetBOSS2CExpressionConverter<RetAuxType, InpAuxType, RetScalarAuxType, InpScalarAuxType>(converterName);
      ULONG id = typedConverter->RegisterScalarTranslator(std::move(translator));
      engineToB2CScalarTranslators[engineType].push_back(std::make_pair(converterName, id));
    };

    template <typename RetAuxType, typename InpAuxType, typename RetScalarAuxType, typename InpScalarAuxType>
    void RegisterCExpression2BOSSTranslator(std::unique_ptr<cexpressiontoboss::translation::TranslatorBase<RetAuxType, InpAuxType, RetScalarAuxType, InpScalarAuxType>> translator, CEngineSpec::EEngineType engineType, const std::string& converterName = DefaultTranslatorName) {
      cexpressiontoboss::CExpressionToBOSSConverter<RetAuxType, InpAuxType, RetScalarAuxType, InpScalarAuxType>* typedConverter = GetCExpression2BOSSConverter<RetAuxType, InpAuxType, RetScalarAuxType, InpScalarAuxType>(converterName);
      ULONG id = typedConverter->RegisterTranslator(std::move(translator));
      engineToC2BTranslators[engineType].push_back(std::make_pair(converterName, id));
    };

    template <typename RetAuxType, typename InpAuxType, typename RetScalarAuxType, typename InpScalarAuxType>
    void RegisterCExpression2BOSSScalarTranslator(std::unique_ptr<cexpressiontoboss::translation::ScalarTranslatorBase<RetAuxType, InpAuxType, RetScalarAuxType, InpScalarAuxType>> translator, CEngineSpec::EEngineType engineType, const std::string& converterName = DefaultTranslatorName) {
      cexpressiontoboss::CExpressionToBOSSConverter<RetAuxType, InpAuxType, RetScalarAuxType, InpScalarAuxType>* typedConverter = GetCExpression2BOSSConverter<RetAuxType, InpAuxType, RetScalarAuxType, InpScalarAuxType>(converterName);
      ULONG id = typedConverter->RegisterScalarTranslator(std::move(translator));
      engineToC2BScalarTranslators[engineType].push_back(std::make_pair(converterName, id));
    };

    void RegisterPreprocessingRule(const std::string& ruleName, PreprocessingRule preprocessingRule, CEngineSpec::EEngineType engineType) {
      preprocessingRules[ruleName] = preprocessingRule;
      engineToPreprocessingRules[engineType].push_back(ruleName);
    };

    void RemovePreprocessingRule(const std::string& ruleName) {
      preprocessingRules.erase(ruleName);
    }

    std::vector<PreprocessingRule> GetPreprocessingRules() {
      std::vector<PreprocessingRule> rules;
      for (auto rule : preprocessingRules) {
        rules.push_back(rule.second);
      }
      return rules;
    }

    void RegisterMetadataProvider(CEngineSpec::EEngineType engine, IMDProvider* pmdp) {
      CMDAccessor *mda = COptCtxt::PoctxtFromTLS()->Pmda();
      IMDId::EMDIdType mdIdType = engineToMDIdType[engine].first;
      auto wstr = bosstocexpression::utils::StringToWString(engineToMDIdType[engine].second);
      CSystemId sysid(mdIdType, wstr.c_str(),wstr.length());
      mda->RegisterProvider(sysid, pmdp);
    }


    void RemoveEngine(std::string& engineName) {
      CEngineSpec::EEngineType engineType = engineNameToEngineType[engineName];
      RemoveEngine(engineType);
      engineNameToEngineType.erase(engineName);
    }
};
}  // namespace orcaextender