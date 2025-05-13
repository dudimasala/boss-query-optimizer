#include "gpoptextender/IDynamicOperatorRegistry.hpp"
#include "gpoptextender/DynamicOperatorRegistry.hpp"  // your full C++17 class

using namespace orcaextender;

class RegistryAdapter : public IDynamicOperatorRegistry {
    DynamicOperatorRegistry *impl;
public:
    RegistryAdapter() {
        // however you normally set it up; e.g. single-instance
        impl = DynamicOperatorRegistry::GetInstance();
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
extern "C" IDynamicOperatorRegistry *CreateDynamicOperatorRegistry() {
    return new RegistryAdapter();
}

