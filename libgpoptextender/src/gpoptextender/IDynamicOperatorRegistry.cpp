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
    ~RegistryAdapter() override {
        // if you own impl, delete it; else leave it
    }
    std::vector<CXform::EXformId>
    GetRelevantTransforms(const std::string &from) override {
        return impl->GetRelevantTransforms(from);
    }
};

// Export the C-linkage factory.
extern "C" IDynamicOperatorRegistry *CreateDynamicOperatorRegistry() {
    return new RegistryAdapter();
}

