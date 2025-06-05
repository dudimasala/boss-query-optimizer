#pragma once

#include "gpoptextender/DynamicRegistry/DynamicRegistry.hpp"

namespace orcaextender {
class Engine {
  protected:
    std::string m_engineName;
    CEngineSpec::EEngineType m_engineType;
    CMemoryPool *m_mp;
    CMDAccessor *m_mda;
  public:
    Engine(std::string engineName, CMemoryPool *mp, CMDAccessor *mda) : m_engineName(engineName), m_engineType(CEngineSpec::EEngineType::EetSentinel), m_mp(mp), m_mda(mda) {}
    virtual ~Engine() {}
    
    virtual void UpdateEngineType() { 
      if (DynamicRegistry::GetInstance()->GetEngineType(m_engineName, false) == CEngineSpec::EEngineType::EetSentinel) {
        DynamicRegistry::GetInstance()->RegisterEngine(m_engineName); 
      }
      m_engineType = DynamicRegistry::GetInstance()->GetEngineType(m_engineName, true);
    }

    virtual void RegisterNewBOSS2CExpressionConverter() {};
    virtual void RegisterNewCExpression2BOSSConverter() {};


    virtual void RegisterCostModelParams() = 0; // register cost constants.
    virtual void RegisterOperators() = 0; // register cost ops.
    virtual void RegisterTransforms() = 0; // register transforms. Note these are technically allowed to be cross-engine.
    virtual void RegisterTranslators() = 0; // orca to boss physical translators must also be dynamically extensible.

    virtual void RemoveTransforms() = 0; // remove transforms

    virtual void RegisterEngineTransforms() = 0;
    virtual void RegisterMetadataFilePath() = 0;

    // helpers
    virtual void RegisterDefaultB2CTranslator(std::unique_ptr<bosstocexpression::TranslatorBase<bosstocexpression::EmptyStruct, bosstocexpression::EmptyStruct, bosstocexpression::EmptyStruct, bosstocexpression::ColRefMap>> translator) {
      DynamicRegistry::GetInstance()->RegisterBOSS2CExpressionTranslator(std::move(translator), m_engineType);
    };

    virtual void RegisterDefaultB2CScalarTranslator(std::unique_ptr<bosstocexpression::ScalarTranslatorBase<bosstocexpression::EmptyStruct, bosstocexpression::EmptyStruct, bosstocexpression::EmptyStruct, bosstocexpression::ColRefMap>> translator) {
      DynamicRegistry::GetInstance()->RegisterBOSS2CExpressionScalarTranslator(std::move(translator), m_engineType);
    };

    virtual void RegisterDefaultC2BTranslator(std::unique_ptr<cexpressiontoboss::translation::TranslatorBase<cexpressiontoboss::translation::EmptyStruct, cexpressiontoboss::translation::ProjectInfo, cexpressiontoboss::translation::ColSet, cexpressiontoboss::translation::EmptyStruct>> translator) {
      DynamicRegistry::GetInstance()->RegisterCExpression2BOSSTranslator(std::move(translator), m_engineType);
    };

    virtual void RegisterDefaultC2BScalarTranslator(std::unique_ptr<cexpressiontoboss::translation::ScalarTranslatorBase<cexpressiontoboss::translation::EmptyStruct, cexpressiontoboss::translation::ProjectInfo, cexpressiontoboss::translation::ColSet, cexpressiontoboss::translation::EmptyStruct>> translator) {
      DynamicRegistry::GetInstance()->RegisterCExpression2BOSSScalarTranslator(std::move(translator), m_engineType);
    };

    virtual std::string GetEngineName() {
      return m_engineName;
    }


    virtual void Register() {
      UpdateEngineType();
      RegisterMetadataFilePath();
      RegisterNewBOSS2CExpressionConverter();
      RegisterNewCExpression2BOSSConverter();
      RegisterCostModelParams();
      RemoveTransforms();
      RegisterOperators();
      RegisterTransforms();
      RegisterTranslators();
      RegisterEngineTransforms();
    }

};
}