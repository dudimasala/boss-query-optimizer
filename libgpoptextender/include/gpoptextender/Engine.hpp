#include "gpoptextender/DynamicOperatorRegistry.hpp"

namespace orcaextender {
class Engine {

  private:
    std::string m_engineName;
    CEngineSpec::EEngineType m_engineType;

  public:
    Engine(std::string engineName) : m_engineName(engineName) {}
    virtual ~Engine() {}
    
    virtual void UpdateEngineType() { 
      if (DynamicOperatorRegistry::GetInstance()->GetEngineType(m_engineName) == CEngineSpec::EEngineType::EetSentinel) {
        DynamicOperatorRegistry::GetInstance()->RegisterEngine(m_engineName); 
      }
    }

    virtual void RegisterOperators(); // register cost constants.
    virtual void RegisterTransforms(); // register transforms. Note these are technically allowed to be cross-engine.
    virtual void RegisterTranslators(); // orca to boss physical translators must also be dynamically extensible.

    virtual void RemoveTranslators();
    virtual void RemoveOperators(); // remove cost models
    virtual void RemoveTransforms(); // remove transforms

    virtual void Enforce() {
      UpdateEngineType();
      RemoveOperators();
      RemoveTransforms();
      RemoveTranslators();
      RegisterOperators();
      RegisterTransforms();
      RegisterTranslators();
    }

};
}