#pragma once

#include <vector>
#include <string>
#include "gpopt/xforms/CXform.h"    // for CXform::EXformId

namespace orcaextender {
using namespace gpos;
using namespace gpopt;
using namespace gpmd;

// Only exposes the one method you need, in C++98-compatible form:
struct IDynamicRegistry {
    virtual ~IDynamicRegistry() {}
    virtual std::vector<CXform::EXformId>
        GetRelevantTransformsForOperator(COperator::EOperatorId opId) = 0;
    virtual std::vector<gpopt::COperator*> GetRelevantOperatorsForTransform(CXform::EXformId transformId, void* opaqueArgs) = 0;
    virtual std::vector<COperator::EOperatorId> GetProjectOperators() = 0;
};

// Factory function – C linkage keeps name-mangling simple.
extern "C" IDynamicRegistry *CreateDynamicRegistry();

} // namespace orcaextender