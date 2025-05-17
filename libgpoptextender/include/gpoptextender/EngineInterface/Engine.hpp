#include "gpoptextender/DynamicRegistry/DynamicRegistry.hpp"

namespace orcaextender {
class Engine {
  protected:
    std::string m_engineName;
    CEngineSpec::EEngineType m_engineType;
    CMemoryPool *m_mp;
    CMDAccessor *m_mda;
  public:
    Engine(std::string engineName, CMemoryPool *mp, CMDAccessor *mda) : m_engineName(engineName), m_mp(mp), m_mda(mda) {}
    virtual ~Engine() {}
    
    virtual void UpdateEngineType() { 
      if (DynamicRegistry::GetInstance()->GetEngineType(m_engineName) == CEngineSpec::EEngineType::EetSentinel) {
        DynamicRegistry::GetInstance()->RegisterEngine(m_engineName); 
      }
    }

    virtual void RegisterOperators() = 0; // register cost constants.
    virtual void RegisterTransforms() = 0; // register transforms. Note these are technically allowed to be cross-engine.
    virtual void RegisterTranslators() = 0; // orca to boss physical translators must also be dynamically extensible.

    virtual void RemoveTranslators() = 0;
    virtual void RemoveOperators() = 0; // remove cost models
    virtual void RemoveTransforms() = 0; // remove transforms

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