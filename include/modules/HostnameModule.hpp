#ifndef MYZONE_HOSTNAME_MODULE_HPP
#define MYZONE_HOSTNAME_MODULE_HPP

#include "../IdentificationModule.hpp"

namespace myzone {

class HostnameModule : public IdentificationModule {
public:
    std::string name() const override;
    bool canEnrich(const DeviceProfile& profile) const override;
    void enrich(DeviceProfile& profile) const override;
};

} // namespace myzone

#endif // MYZONE_HOSTNAME_MODULE_HPP
