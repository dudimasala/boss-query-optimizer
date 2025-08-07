#pragma once

#include <vector>
#include <string>
#include "gpopt/xforms/CXform.h"    // for CXform::EXformId
#include "gpoptextender/DynamicRegistry/DynamicOperatorArgs.hpp"

namespace orcaextender {
using namespace gpos;
using namespace gpopt;
using namespace gpmd;

typedef CExpression *(*PreprocessingRule)(CMemoryPool *, CExpression *);

// Only exposes the one method you need, in C++98-compatible form:
struct IDynamicRegistry {
    virtual ~IDynamicRegistry() {}
    virtual std::vector<CXform::EXformId>
        GetRelevantTransformsForOperator(COperator::EOperatorId opId) = 0;
    virtual std::vector<gpopt::COperator*> GetRelevantOperatorsForTransform(CXform::EXformId transformId, DynamicOperatorArgs& args) = 0;
    virtual std::vector<COperator::EOperatorId> GetProjectOperators() = 0;
    virtual void AddTransformsToXFormSet(COperator::EOperatorId opId, CXformSet* xformSet) = 0;
    virtual bool GetEnginePreserveOrder(EEngineType from, EEngineType to) = 0;
    virtual bool GetEnginePreserveDistribution(EEngineType from, EEngineType to) = 0;
    virtual bool GetEnginePreserveRewindability(EEngineType from, EEngineType to) = 0;
    virtual std::vector<PreprocessingRule> GetPreprocessingRules() = 0;
    virtual EEngineType GetEngineForDefaultOp(COperator::EOperatorId opId) = 0;
};

// Factory function – C linkage keeps name-mangling simple.
extern "C" IDynamicRegistry *CreateDynamicRegistry();

} // namespace orcaextender