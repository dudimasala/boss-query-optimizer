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
    std::vector<gpopt::COperator*> GetRelevantOperatorsForTransform(CXform::EXformId transformId, void* opaqueArgs) override {
        return impl->GetRelevantOperatorsForTransform(transformId, opaqueArgs);
    }
};

// Export the C-linkage factory.
extern "C" IDynamicRegistry *CreateDynamicRegistry() {
    return new RegistryAdapter();
}

