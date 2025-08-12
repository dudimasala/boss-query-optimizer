//---------------------------------------------------------------------------
// Copyright 2025 Malhar Rajpal
// Interface for Orca to BOSS converter.
//---------------------------------------------------------------------------


#pragma once

#include "utils.hpp"

namespace orcaextender {

  class C2BConverter {
    public:
      C2BConverter() = default;
      virtual ~C2BConverter() = default;

      virtual void RemoveTranslator(ULONG translatorId) = 0;
      virtual void RemoveScalarTranslator(ULONG translatorId) = 0;

      virtual std::pair<Expression, bool> ConvertExpr(CExpression *cexpr) = 0;
  };
}