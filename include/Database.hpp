#ifndef MYZONE_DATABASE_HPP
#define MYZONE_DATABASE_HPP

#include <string>
#include <unordered_map>
#include "MacAddress.hpp"
#include "DeviceCategory.hpp"

namespace myzone {

	struct DeviceInfo {
		std::string companyName;     // Le nom du fabricant 
		DeviceCategory category;     // La catégorie
	};

	class Database {
	public:
		// Le constructeur prendra le chemin vers le fichier "lookup.csv"
		explicit Database(const std::string& csvFilePath);

		// Cherche une adresse MAC dans notre base de données.
		// Retourne 'true' si trouvée et remplit 'outInfo', retourne 'false' sinon.
		bool lookup(const MacAddress& mac, DeviceInfo& outInfo) const;

	private:
		// - La clé (std::string) sera l'OUI (les 3 premiers octets, ex: "C4:A0:52")
		// - La valeur (DeviceInfo) contiendra le fabricant et la catégorie
		std::unordered_map<std::string, DeviceInfo> data_;
	};

} // namespace myzone

#endif // MYZONE_DATABASE_HPP
