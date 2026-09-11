#include "../include/IdentificationEngine.hpp"

namespace myzone {

void IdentificationEngine::addModule(std::unique_ptr<IdentificationModule> module) {
    modules_.push_back(std::move(module));
}

DeviceProfile IdentificationEngine::identify(const DeviceProfile& input) const {
    DeviceProfile profile = input;

    for (const auto& module : modules_) {
        if (module->canEnrich(profile)) {
            module->enrich(profile);
        }
    }

    return profile;
}

} // namespace myzone
