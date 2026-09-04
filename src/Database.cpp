#include "../include/Database.hpp"
#include <fstream>
#include <vector>
#include <iostream>

namespace myzone {

	static std::vector<std::string> parseCsvLine(const std::string& line) {
		std::vector<std::string> result;
		std::string current;
		bool inQuotes = false;
		
		for (char c : line) {
			if (c == '"') {
				inQuotes = !inQuotes;
			} else if (c == ',' && !inQuotes) {
				result.push_back(current);
				current.clear();
			} else {
				current += c;
			}
		}
		result.push_back(current);
		return result;
	}

	Database::Database(const std::string& csvFilePath) {
		std::ifstream file(csvFilePath);
		if (!file.is_open()) {
			std::cerr << "Error: Unable to open database file: " << csvFilePath << std::endl;
			return;
		}

		std::string line;
		// std::getline(file, line); // Uncomment to skip CSV header

		while (std::getline(file, line)) {
			if (line.empty()) continue;

			std::vector<std::string> columns = parseCsvLine(line);
			
			if (columns.size() >= 5) {
				std::string rawYes = columns[0];
				std::string companyName = columns[1];
				std::string deviceType = columns[4];

				// Remove ':' from Yes (e.g., "C4:A0:52" -> "C4A052")
				std::string cleanYes;
				for (char c : rawYes) {
					if (c != ':') cleanYes += c;
				}

				DeviceInfo info;
				info.companyName = companyName;
				info.category = classifyFromDeviceType(deviceType);

				data_[cleanYes] = info;
			}
		}
	}

	bool Database::lookup(const MacAddress& mac, DeviceInfo& outInfo) const {
		if (!mac.isValid()) return false;

		//  MacAddress::getYes() returns a 6-char string like "C4A052"
		std::string searchYes = mac.yes(); 

		auto it = data_.find(searchYes);
		if (it != data_.end()) {
			outInfo = it->second;
			return true;
		}

		return false;
	}

} // namespace myzone
