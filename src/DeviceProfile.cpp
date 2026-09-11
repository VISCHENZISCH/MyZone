#include "../include/DeviceProfile.hpp"

namespace myzone {

void DeviceProfile::addResult(const std::string& moduleName,
                              const std::string& detail) {
    results.push_back(ModuleResult{moduleName, detail});
}

} // namespace myzone
