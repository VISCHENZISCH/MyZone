#ifndef MYZONE_DATABASE_HPP
#define MYZONE_DATABASE_HPP

#include <cstddef>
#include <string>
#include <unordered_map>
#include <vector>
#include "MacAddress.hpp"
#include "DeviceCategory.hpp"

namespace myzone {

	/// @brief Informations de base sur un appareil
	struct DeviceInfo {
		/// @brief Nom de l'entreprise ou fabricant
		std::string companyName;
		/// @brief Catégorie de l'appareil
		DeviceCategory category;
		/// @brief Source de l'information
		std::string source;
	};

	/// @brief Informations liées à une empreinte DHCP
	struct DhcpFingerprintInfo {
		/// @brief L'empreinte DHCP (ex: 1,3,6,15)
		std::string fingerprint;
		/// @brief Nom de l'appareil
		std::string deviceName;
		/// @brief Options demandées
		std::string requestedOptions;
		/// @brief Niveau de confiance de l'identification
		int confidence = 0;
	};

	/// @brief Informations liées à une signature P0f (TCP/IP)
	struct P0fSignatureInfo {
		/// @brief Signature P0f
		std::string signature;
		/// @brief Nom du système d'exploitation
		std::string osName;
	};

	
	/// @brief Informations provenant de FingerBank
	struct FingerBankInfo {
		/// @brief Options DHCP
		std::string options;
		/// @brief Nom du système d'exploitation
		std::string osName;
	};

	/// @brief Informations sur une source de données
	struct DataSourceInfo {
		/// @brief Nom du fichier de la source
		std::string fileName;
		/// @brief Étiquette descriptive de la source
		std::string label;
		/// @brief But ou utilisation de la source
		std::string purpose;
		/// @brief Nombre d'enregistrements chargés
		std::size_t records = 0;
		/// @brief Indique si la source est disponible
		bool available = false;
		/// @brief Indique si la source est indexée
		bool indexed = false;
	};

	/// @brief Classe principale gérant la base de données d'empreintes et constructeurs d'appareils
	class Database {
	public:
		/// @brief Construit une nouvelle instance de la base de données
		/// @param dataDirectory Le répertoire contenant les fichiers de données
		explicit Database(const std::string& dataDirectory);

		/// @brief Recherche des informations à partir d'une adresse MAC (OUI)
		/// @param mac L'adresse MAC à rechercher
		/// @param outInfo Référence pour stocker les informations trouvées
		/// @return true si trouvé, false sinon
		bool lookup(const MacAddress& mac, DeviceInfo& outInfo) const;
		/// @brief Recherche des informations à partir d'une empreinte DHCP
		/// @param fingerprint L'empreinte DHCP
		/// @param outInfo Référence pour stocker les informations trouvées
		/// @return true si trouvé, false sinon
		bool lookupDhcpFingerprint(const std::string& fingerprint,
		                           DhcpFingerprintInfo& outInfo) const;
		/// @brief Recherche des informations à partir d'une signature p0f
		/// @param signature La signature p0f
		/// @param outInfo Référence pour stocker les informations trouvées
		/// @return true si trouvé, false sinon
		bool lookupP0fSignature(const std::string& signature,
		                        P0fSignatureInfo& outInfo) const;
		/// @brief Recherche des informations à partir d'options Fingerbank
		/// @param options Les options DHCP pour Fingerbank
		/// @param outInfo Référence pour stocker les informations trouvées
		/// @return true si trouvé, false sinon
		bool lookupFingerBankOptions(const std::string& options,
		                             FingerBankInfo& outInfo) const;

		/// @brief Renvoie le nombre de fabricants (OUI) enregistrés
		/// @return Nombre d'enregistrements
		std::size_t vendorCount() const { return data_.size(); }
		/// @brief Renvoie le nombre d'empreintes DHCP enregistrées
		/// @return Nombre d'enregistrements
		std::size_t dhcpFingerprintCount() const { return dhcpData_.size(); }
		/// @brief Renvoie le nombre de signatures P0f enregistrées
		/// @return Nombre d'enregistrements
		std::size_t p0fSignatureCount() const { return p0fData_.size(); }
		/// @brief Renvoie le nombre d'options FingerBank enregistrées
		/// @return Nombre d'enregistrements
		std::size_t fingerBankCount() const { return fingerBankData_.size(); }
		/// @brief Vérifie si la base de données est prête et chargée
		/// @return true si la base contient des données, false sinon
		bool isReady() const { return !data_.empty(); }
		/// @brief Renvoie le répertoire de données utilisé
		/// @return Chemin du répertoire de données
		const std::string& dataDirectory() const { return dataDirectory_; }
		/// @brief Renvoie la liste des sources de données utilisées
		/// @return Vecteur contenant les informations sur les sources
		const std::vector<DataSourceInfo>& sources() const { return sources_; }

	private:
		void loadLookupCsv(const std::string& filePath);
		void loadManufFile(const std::string& filePath, const std::string& sourceName);
		void loadNmapPrefixes(const std::string& filePath);
		void loadDhcpFingerprints(const std::string& filePath);
		void loadP0fSignatures(const std::string& filePath);
		void loadFingerBankConf(const std::string& filePath);
		void addPassiveSource(const std::string& fileName,
		                      const std::string& label,
		                      const std::string& purpose);
		void addSource(const DataSourceInfo& source);
		void addVendor(const std::string& prefix,
		               const std::string& companyName,
		               DeviceCategory category,
		               const std::string& source);

		std::string dataDirectory_;
		std::unordered_map<std::string, DeviceInfo> data_;
		std::unordered_map<std::string, DhcpFingerprintInfo> dhcpData_;
		std::unordered_map<std::string, P0fSignatureInfo> p0fData_;
		std::unordered_map<std::string, FingerBankInfo> fingerBankData_;
		std::vector<DataSourceInfo> sources_;
	};

} // namespace myzone

#endif // MYZONE_DATABASE_HPP
