#include "gpoptextender/DynamicRegistry/IDynamicRegistry.hpp"
#include "gpoptextender/DynamicRegistry/DynamicRegistry.hpp"  // your full C++17 class

using namespace orcaextender;

class RegistryAdapter : public IDynamicRegistry {
    DynamicRegistry *impl;
public:
    RegistryAdapter() {
        // however you normally set it up; e.g. single-instance
        impl = DynamicRegistry::GetInstance();
    }
    std::vector<CXform::EXformId>
    GetRelevantTransformsForOperator(COperator::EOperatorId opId) override {
        return impl->GetRelevantTransformsForOperator(opId);
    }
    std::vector<gpopt::COperator*> GetRelevantOperatorsForTransform(CXform::EXformId transformId, DynamicOperatorArgs&
     args) override {
        return impl->GetRelevantOperatorsForTransform(transformId, args);
    }
    std::vector<COperator::EOperatorId> GetProjectOperators() override {
        std::vector<COperator::EOperatorId> projectOperators;
        for (auto opId : impl->GetProjectOperators()) {
            projectOperators.push_back(opId);
        }
        return projectOperators;
    }

    void AddTransformsToXFormSet(COperator::EOperatorId opId, CXformSet* xformSet) override {
        impl->AddTransformsToXFormSet(opId, xformSet);
    }

    bool GetEnginePreserveOrder(CEngineSpec::EEngineType from, CEngineSpec::EEngineType to) override {
        return impl->GetEnginePreserveOrder(from, to);
    }

    bool GetEnginePreserveDistribution(CEngineSpec::EEngineType from, CEngineSpec::EEngineType to) override {
        return impl->GetEnginePreserveDistribution(from, to);
    }

    bool GetEnginePreserveRewindability(CEngineSpec::EEngineType from, CEngineSpec::EEngineType to) override {
        return impl->GetEnginePreserveRewindability(from, to);
    }

    std::vector<PreprocessingRule> GetPreprocessingRules() override {
        return impl->GetPreprocessingRules();
    }

    
};

// Export the C-linkage factory.
extern "C" IDynamicRegistry *CreateDynamicRegistry() {
    return new RegistryAdapter();
}

