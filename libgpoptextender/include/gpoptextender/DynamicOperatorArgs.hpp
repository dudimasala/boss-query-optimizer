#pragma once

#include <map>
#include <string>

namespace orcaextender {

struct DynamicOperatorArgs {
    std::map<std::string, void*> raw;

    void set(const std::string& key, void* value) {
        raw[key] = value;
    }

    void* get(const std::string& key) const {
        std::map<std::string, void*>::const_iterator it = raw.find(key);
        return (it != raw.end()) ? it->second : NULL;
    }
};

} // namespace orcaextender