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
    GetRelevantTransforms(const std::string &from) override {
        return impl->GetRelevantTransforms(from);
    }
    std::vector<gpopt::COperator*> GetOperators(const std::string& opName, void* opaqueArgs) override {
        return impl->GetOperators(opName, opaqueArgs);
    }
};

// Export the C-linkage factory.
extern "C" IDynamicOperatorRegistry *CreateDynamicOperatorRegistry() {
    return new RegistryAdapter();
}

