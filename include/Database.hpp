#ifndef MYZONE_DATABASE_HPP
#define MYZONE_DATABASE_HPP

#include <cstddef>
#include <string>
#include <unordered_map>
#include <vector>
#include "MacAddress.hpp"
#include "DeviceCategory.hpp"

namespace myzone {

	struct DeviceInfo {
		std::string companyName;
		DeviceCategory category;
		std::string source;
	};

	struct DhcpFingerprintInfo {
		std::string fingerprint;
		std::string deviceName;
		std::string requestedOptions;
		int confidence = 0;
	};

	struct DataSourceInfo {
		std::string fileName;
		std::string label;
		std::string purpose;
		std::size_t records = 0;
		bool available = false;
		bool indexed = false;
	};

	class Database {
	public:
		// Charge les sources situées dans le dossier data.
		explicit Database(const std::string& dataDirectory);

		// Cherche une adresse MAC dans les index OUI chargés.
		bool lookup(const MacAddress& mac, DeviceInfo& outInfo) const;
		bool lookupDhcpFingerprint(const std::string& fingerprint,
		                           DhcpFingerprintInfo& outInfo) const;

		std::size_t vendorCount() const { return data_.size(); }
		std::size_t dhcpFingerprintCount() const { return dhcpData_.size(); }
		bool isReady() const { return !data_.empty(); }
		const std::string& dataDirectory() const { return dataDirectory_; }
		const std::vector<DataSourceInfo>& sources() const { return sources_; }

	private:
		void loadLookupCsv(const std::string& filePath);
		void loadManufFile(const std::string& filePath, const std::string& sourceName);
		void loadNmapPrefixes(const std::string& filePath);
		void loadDhcpFingerprints(const std::string& filePath);
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
		std::vector<DataSourceInfo> sources_;
	};

} // namespace myzone

#endif // MYZONE_DATABASE_HPP
