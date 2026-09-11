#ifndef MYZONE_DHCP_MODULE_HPP
#define MYZONE_DHCP_MODULE_HPP

#include "../IdentificationModule.hpp"
#include "../Database.hpp"

namespace myzone {

class DhcpModule : public IdentificationModule {
public:
    explicit DhcpModule(const Database& database);

    std::string name() const override;
    bool canEnrich(const DeviceProfile& profile) const override;
    void enrich(DeviceProfile& profile) const override;

private:
    const Database& database_;
};

} // namespace myzone

#endif // MYZONE_DHCP_MODULE_HPP
