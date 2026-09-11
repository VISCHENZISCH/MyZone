#ifndef MYZONE_IDENTIFICATION_ENGINE_HPP
#define MYZONE_IDENTIFICATION_ENGINE_HPP

#include <memory>
#include <vector>
#include "DeviceProfile.hpp"
#include "IdentificationModule.hpp"

namespace myzone {

class IdentificationEngine {
public:
    void addModule(std::unique_ptr<IdentificationModule> module);
    DeviceProfile identify(const DeviceProfile& input) const;
    std::size_t moduleCount() const { return modules_.size(); }

private:
    std::vector<std::unique_ptr<IdentificationModule>> modules_;
};

} // namespace myzone

#endif // MYZONE_IDENTIFICATION_ENGINE_HPP
