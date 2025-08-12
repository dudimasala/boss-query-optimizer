//---------------------------------------------------------------------------
// Copyright 2025 Malhar Rajpal
// Interface for BOSS to Orca converter.
//---------------------------------------------------------------------------


#pragma once
#include "utils.hpp"

namespace orcaextender {

  class B2CConverter {
    public:
      B2CConverter() = default;
      virtual ~B2CConverter() = default;

      virtual void RemoveTranslator(ULONG translatorId) = 0;
      virtual void RemoveScalarTranslator(ULONG translatorId) = 0;

      virtual std::pair<CExpression *, bool> ConvertExpr(Expression&& bossExpr) = 0;
  };
}