#ifndef MYZONE_IDENTIFICATION_MODULE_HPP
#define MYZONE_IDENTIFICATION_MODULE_HPP

#include <string>
#include "DeviceProfile.hpp"

namespace myzone {

class IdentificationModule {
public:
    virtual ~IdentificationModule() = default;
    virtual std::string name() const = 0;
    virtual bool canEnrich(const DeviceProfile& profile) const = 0;
    virtual void enrich(DeviceProfile& profile) const = 0;
};

} // namespace myzone

#endif // MYZONE_IDENTIFICATION_MODULE_HPP
