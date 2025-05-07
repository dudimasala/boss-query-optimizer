#pragma once

#include <vector>
#include <string>
#include "gpopt/xforms/CXform.h"    // for CXform::EXformId

namespace orcaextender {
using namespace gpos;
using namespace gpopt;
using namespace gpmd;

// Only exposes the one method you need, in C++98-compatible form:
struct IDynamicOperatorRegistry {
    virtual ~IDynamicOperatorRegistry() {}
    virtual std::vector<CXform::EXformId>
        GetRelevantTransforms(const std::string &transformFrom) = 0;
};

// Factory function – C linkage keeps name-mangling simple.
extern "C" IDynamicOperatorRegistry *CreateDynamicOperatorRegistry();

} // namespace orcaextender