#pragma once

#include "gpoptextender/CostModel/BOSSCostModel.hpp"
#include "gpopt/xforms/CXform.h"
#include "gpopt/xforms/CXformFactory.h"
#include "gpoptextender/DynamicRegistry/DynamicOperatorArgs.hpp"
#include "gpoptextender/Translation/Converter.hpp"
#include "gpopt/operators/CNormalizer.h"
#include "BOSSToCExpression.hpp"
#include "CExpressionToBOSS.hpp"
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

    DynamicRegistry(BOSSCostModel* costModel);
    static DynamicRegistry* s_pInstance;

    COperator::EOperatorId currentOperatorId;
    CXform::EXformId currentTransformId;
    BOSSCostModel* costModel;
    CEngineSpec::EEngineType currentEngineType;


    std::map<std::string, std::unique_ptr<Converter>> boss2cexpressionConverters;
    std::map<std::string, std::unique_ptr<Converter>> cexpression2bossConverters;

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
    std::unordered_set<COperator::EOperatorId> projectOperators = {};

    std::unordered_map<COperator::EOperatorId, std::vector<CXform::EXformId>> relevantTransforms = {};
    std::unordered_map<CXform::EXformId, std::vector<FnOperatorFactory>> opFactories = {};
    std::unordered_map<std::string, std::vector<std::string>> engineToOperatorNames = {}; // for querying.
  public:
    static DynamicRegistry* GetInstance();
    static void Init(CMemoryPool* mp, BOSSCostModel* costModel);
    inline static const std::string& DefaultTranslatorName = "DEFAULT";

    ~DynamicRegistry();

    void RegisterPhysicalOperator(const std::string& opName, CEngineSpec::EEngineType engine, FnCost costFunc);
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
    std::vector<COperator*> GetRelevantOperatorsForTransform(CXform::EXformId transformId, DynamicOperatorArgs& args);
    std::vector<CXform::EXformId> GetRelevantTransformsForOperator(COperator::EOperatorId opId);
    CXform::EXformId GetTransformId(const std::string& transformName, bool throwError = true);
    CEngineSpec::EEngineType GetEngineType(const std::string& engineName, bool throwError = true);
    std::unordered_map<std::string, std::vector<std::string>>& GetAllOperators() { return engineToOperatorNames; };
    std::unordered_set<COperator::EOperatorId> GetProjectOperators() { return projectOperators; };
    ICostModelParams* GetCostModelParams(CEngineSpec::EEngineType engine) { return costModel->GetCostModelParams(engine); };

    void AddTransformsToXFormSet(COperator::EOperatorId opId, CXformSet* xformSet);

    template <typename RetAuxType, typename InpAuxType, typename RetScalarAuxType, typename InpScalarAuxType>
    void RegisterBOSS2CExpressionConverter(const std::string& converterName, bosstocexpression::BOSSToCExpressionConverter<RetAuxType, InpAuxType, RetScalarAuxType, InpScalarAuxType>& converter) { 
      if (converterName == DefaultTranslatorName) {
        throw std::invalid_argument("Default translator name cannot be used for registering a converter.");
      }
      boss2cexpressionConverters[converterName] = converter; 
    };

    template <typename RetAuxType, typename InpAuxType, typename RetScalarAuxType, typename InpScalarAuxType>
    void RegisterCExpression2BOSSConverter(const std::string& converterName, cexpressiontoboss::CExpressionToBOSSConverter<RetAuxType, InpAuxType, RetScalarAuxType, InpScalarAuxType>& converter) { 
      if (converterName == DefaultTranslatorName) {
        throw std::invalid_argument("Default translator name cannot be used for registering a converter.");
      }
      cexpression2bossConverters[converterName] = converter; 
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

    template <typename RetAuxType, typename InpAuxType, typename RetScalarAuxType, typename InpScalarAuxType>
    void RegisterBOSS2CExpressionTranslator(std::unique_ptr<bosstocexpression::TranslatorBase<RetAuxType, InpAuxType, RetScalarAuxType, InpScalarAuxType>> translator, const std::string& converterName = DefaultTranslatorName) {
      bosstocexpression::BOSSToCExpressionConverter<RetAuxType, InpAuxType, RetScalarAuxType, InpScalarAuxType>* typedConverter = GetBOSS2CExpressionConverter<RetAuxType, InpAuxType, RetScalarAuxType, InpScalarAuxType>(converterName);
      typedConverter->RegisterTranslator(std::move(translator));
    };

    template <typename RetAuxType, typename InpAuxType, typename RetScalarAuxType, typename InpScalarAuxType>
    void RegisterBOSS2CExpressionScalarTranslator(std::unique_ptr<bosstocexpression::ScalarTranslatorBase<RetAuxType, InpAuxType, RetScalarAuxType, InpScalarAuxType>> translator, const std::string& converterName = DefaultTranslatorName) {
      bosstocexpression::BOSSToCExpressionConverter<RetAuxType, InpAuxType, RetScalarAuxType, InpScalarAuxType>* typedConverter = GetBOSS2CExpressionConverter<RetAuxType, InpAuxType, RetScalarAuxType, InpScalarAuxType>(converterName);
      typedConverter->RegisterScalarTranslator(std::move(translator));
    };

    template <typename RetAuxType, typename InpAuxType, typename RetScalarAuxType, typename InpScalarAuxType>
    void RegisterCExpression2BOSSTranslator(std::unique_ptr<cexpressiontoboss::translation::TranslatorBase<RetAuxType, InpAuxType, RetScalarAuxType, InpScalarAuxType>> translator, const std::string& converterName = DefaultTranslatorName) {
      cexpressiontoboss::CExpressionToBOSSConverter<RetAuxType, InpAuxType, RetScalarAuxType, InpScalarAuxType>* typedConverter = GetCExpression2BOSSConverter<RetAuxType, InpAuxType, RetScalarAuxType, InpScalarAuxType>(converterName);
      typedConverter->RegisterTranslator(std::move(translator));
    };

    template <typename RetAuxType, typename InpAuxType, typename RetScalarAuxType, typename InpScalarAuxType>
    void RegisterCExpression2BOSSScalarTranslator(std::unique_ptr<cexpressiontoboss::translation::ScalarTranslatorBase<RetAuxType, InpAuxType, RetScalarAuxType, InpScalarAuxType>> translator, const std::string& converterName = DefaultTranslatorName) {
      cexpressiontoboss::CExpressionToBOSSConverter<RetAuxType, InpAuxType, RetScalarAuxType, InpScalarAuxType>* typedConverter = GetCExpression2BOSSConverter<RetAuxType, InpAuxType, RetScalarAuxType, InpScalarAuxType>(converterName);
      typedConverter->RegisterScalarTranslator(std::move(translator));
    };

    void RegisterNormalizerRule(COperator::EOperatorId opId, CNormalizer::FnPushThru pushThruFn) {
      CNormalizer::SPushThru pushThru(opId, pushThruFn);
      CNormalizer::AddPushThru(pushThru);
    };

};
}  // namespace orcaextender