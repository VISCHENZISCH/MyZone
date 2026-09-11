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

	struct P0fSignatureInfo {
		std::string signature;
		std::string osName;
	};

	
	struct FingerBankInfo {
		std::string options;
		std::string osName;
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
		explicit Database(const std::string& dataDirectory);

		bool lookup(const MacAddress& mac, DeviceInfo& outInfo) const;
		bool lookupDhcpFingerprint(const std::string& fingerprint,
		                           DhcpFingerprintInfo& outInfo) const;
		bool lookupP0fSignature(const std::string& signature,
		                        P0fSignatureInfo& outInfo) const;
		bool lookupFingerBankOptions(const std::string& options,
		                             FingerBankInfo& outInfo) const;

		std::size_t vendorCount() const { return data_.size(); }
		std::size_t dhcpFingerprintCount() const { return dhcpData_.size(); }
		std::size_t p0fSignatureCount() const { return p0fData_.size(); }
		std::size_t fingerBankCount() const { return fingerBankData_.size(); }
		bool isReady() const { return !data_.empty(); }
		const std::string& dataDirectory() const { return dataDirectory_; }
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
