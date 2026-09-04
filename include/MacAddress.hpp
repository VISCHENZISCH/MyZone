#ifndef MYZONE_MAC_ADDRESS_HPP
#define MYZONE_MAC_ADDRESS_HPP

#include <string>
#include <array>

namespace myzone {

/**
 * Représente une adresse MAC (48 bits / 6 octets).
 * Sait se parser depuis différents formats texte et
 * exposer son OUI (les 3 premiers octets = identifiant fabricant).
 */
class MacAddress {
public:
    MacAddress() = default;

    // Parse une chaîne du type "AA:BB:CC:DD:EE:FF", "AA-BB-CC-DD-EE-FF"
    // ou "AABBCCDDEEFF". Lève std::invalid_argument si le format est invalide.
    explicit MacAddress(const std::string& raw);

    // Retourne l'OUI normalisé (6 caractères hexa majuscules, sans séparateur)
    // ex: "D83AAD" pour l'adresse D8:3A:AD:12:34:56
    std::string oui() const;

    // Retourne l'adresse complète normalisée avec séparateurs ':'
    // ex: "D8:3A:AD:12:34:56"
    std::string toString() const;

    // Retourne l'adresse brute sous forme de 6 octets
    const std::array<unsigned char, 6>& bytes() const { return bytes_; }

    bool isValid() const { return valid_; }

private:
    std::array<unsigned char, 6> bytes_{};
    bool valid_ = false;

    // Nettoie la chaîne d'entrée (retire séparateurs, met en majuscules)
    static std::string normalizeInput(const std::string& raw);
};

} // namespace myzone

#endif // MYZONE_MAC_ADDRESS_HPP
