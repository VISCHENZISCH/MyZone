#ifndef MYZONE_FINGERBANK_MODULE_HPP
#define MYZONE_FINGERBANK_MODULE_HPP

#include "../IdentificationModule.hpp"
#include "../Database.hpp"

namespace myzone {

class FingerBankModule : public IdentificationModule {
public:
    explicit FingerBankModule(const Database& database);

    std::string name() const override;
    bool canEnrich(const DeviceProfile& profile) const override;
    void enrich(DeviceProfile& profile) const override;

private:
    const Database& database_;
};

} // namespace myzone

#endif // MYZONE_FINGERBANK_MODULE_HPP
