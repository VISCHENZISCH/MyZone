#ifndef MYZONE_MAC_ADDRESS_HPP
#define MYZONE_MAC_ADDRESS_HPP

#include <string>
#include <array>

namespace myzone {

    class MacAddress {

    public:
        MacAddress() = default;
        explicit MacAddress(const std::string& raw);
        std::string compact() const;
        std::string toString() const;
        const std::array<unsigned char, 6>& bytes() const { return bytes_;}
        bool isValid() const { return valid_;}

    private:
        std::array<unsigned char, 6> bytes_{};
        bool valid_ = false;
        static std::string normalizeInput(const std::string& raw);
    };

 }

#endif // MYZONE_MAC_ADDRESS_HPP
