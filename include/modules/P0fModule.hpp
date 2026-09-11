#ifndef MYZONE_P0F_MODULE_HPP
#define MYZONE_P0F_MODULE_HPP

#include "../IdentificationModule.hpp"
#include "../Database.hpp"

namespace myzone {

class P0fModule : public IdentificationModule {
public:
    explicit P0fModule(const Database& database);

    std::string name() const override;
    bool canEnrich(const DeviceProfile& profile) const override;
    void enrich(DeviceProfile& profile) const override;

private:
    const Database& database_;
};

} // namespace myzone

#endif // MYZONE_P0F_MODULE_HPP
