#ifndef MYZONE_MAC_ADDRESS_HPP
#define MYZONE_MAC_ADDRESS_HPP

#include <string>
#include <array>
#include <optional>
#include <functional>

namespace myzone {

    /// @brief Représente une adresse MAC (Media Access Control)
    class MacAddress {

    public:
        /// @brief Construit une adresse MAC vide ou invalide
        MacAddress() = default;
        /// @brief Construit une adresse MAC à partir d'une chaîne de caractères
        /// @param raw La chaîne brute représentant l'adresse (ex: "00:11:22:33:44:55")
        explicit MacAddress(const std::string& raw);
        
        /// @brief Opérateur d'égalité
        /// @param other L'autre adresse MAC à comparer
        /// @return true si les adresses sont identiques
        bool operator==(const MacAddress& other) const;
        /// @brief Opérateur d'inégalité
        /// @param other L'autre adresse MAC à comparer
        /// @return true si les adresses sont différentes
        bool operator!=(const MacAddress& other) const;
        /// @brief Opérateur de comparaison "inférieur à" (utile pour les tris/maps)
        /// @param other L'autre adresse MAC
        /// @return true si cette adresse est inférieure à l'autre
        bool operator<(const MacAddress& other) const;
        /// @brief Tente d'analyser une chaîne et de retourner une MacAddress valide
        /// @param raw La chaîne à analyser
        /// @return Un std::optional contenant l'adresse si valide, ou vide sinon
        static std::optional<MacAddress> tryParse(const std::string& raw) noexcept;

        /// @brief Renvoie la forme compacte de l'adresse (sans séparateurs)
        /// @return Chaîne contenant l'adresse compactée
        std::string compact() const;
        /// @brief Renvoie la forme standard (avec séparateurs) de l'adresse
        /// @return Chaîne lisible de l'adresse MAC
        std::string toString() const;
        /// @brief Renvoie le tableau interne d'octets de l'adresse MAC
        /// @return Référence vers un tableau de 6 octets
        const std::array<unsigned char, 6>& bytes() const { return bytes_;}
        /// @brief Vérifie si l'adresse est valide
        /// @return true si valide, false sinon
        bool isValid() const { return valid_;}

    private:
        std::array<unsigned char, 6> bytes_{};
        bool valid_ = false;
        static std::string normalizeInput(const std::string& raw);
    };

 }

namespace std {
    /// @brief Spécialisation de std::hash pour la classe MacAddress
    template<>
    struct hash<myzone::MacAddress> {
        /// @brief Calcule le hash de l'adresse MAC
        /// @param mac L'adresse MAC
        /// @return Valeur de hachage
        std::size_t operator()(const myzone::MacAddress& mac) const noexcept;
    };
}

#endif // MYZONE_MAC_ADDRESS_HPP
