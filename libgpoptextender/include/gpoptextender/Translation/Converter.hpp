#pragma once

namespace orcaextender {

  class Converter {
    public:
      Converter() = default;
      virtual ~Converter() = default;

      virtual void RemoveTranslator(ULONG translatorId) = 0;
      virtual void RemoveScalarTranslator(ULONG translatorId) = 0;
  };
}