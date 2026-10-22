#include "../include/DeviceCategory.hpp"
#include <unordered_map>

namespace myzone {

	DeviceCategory classifyFromDeviceType(const std::string& deviceType) {
		static const std::unordered_map<std::string, DeviceCategory> categoryMap = {
			{"Computer", DeviceCategory::Computer},
			{"Phone", DeviceCategory::Mobile},
			{"Tablet", DeviceCategory::Tablet},
			{"Server", DeviceCategory::Server},
			{"VM", DeviceCategory::VirtualMachine},
			{"Router", DeviceCategory::Router},
			{"Switch", DeviceCategory::Switch},
			{"Modem", DeviceCategory::Modem},
			{"AP", DeviceCategory::AccessPoint},
			{"IoT", DeviceCategory::IoT},
			{"SmartHome", DeviceCategory::SmartHome},
			{"Camera", DeviceCategory::Camera},
			{"Printer", DeviceCategory::Printer},
			{"TV", DeviceCategory::SmartTV},
			{"Console", DeviceCategory::GamingConsole},
			{"Wearable", DeviceCategory::Wearable},
			{"Audio", DeviceCategory::Audio}
		};

		auto it = categoryMap.find(deviceType);
		if (it != categoryMap.end()) {
			return it->second;
		}

		return DeviceCategory::Unknown;
	}

	std::string toString(DeviceCategory category) {
		switch (category) {
			case DeviceCategory::Computer:       return "Ordinateur";
			case DeviceCategory::Mobile:         return "Smartphone";
			case DeviceCategory::Tablet:         return "Tablette";
			case DeviceCategory::Server:         return "Serveur";
			case DeviceCategory::VirtualMachine: return "Machine Virtuelle";
			case DeviceCategory::Modem:          return "Modem / Box";
			case DeviceCategory::Router:         return "Routeur";
			case DeviceCategory::Switch:         return "Switch Réseau";
			case DeviceCategory::AccessPoint:    return "Point d'Accès Wi-Fi";
			case DeviceCategory::Printer:        return "Imprimante";
			case DeviceCategory::SmartTV:        return "Smart TV";
			case DeviceCategory::GamingConsole:  return "Console de Jeux";
			case DeviceCategory::Audio:          return "Audio Connecté";
			case DeviceCategory::Camera:         return "Caméra IP";
			case DeviceCategory::SmartHome:      return "Domotique";
			case DeviceCategory::Wearable:       return "Montre Connectée";
			case DeviceCategory::IoT:            return "IoT (Objet Connecté)";
			case DeviceCategory::Unknown:
			default:                             return "Inconnu";
		}
	}

	std::string toKey(DeviceCategory category) {
		switch (category) {
			case DeviceCategory::Computer:       return "Computer";
			case DeviceCategory::Mobile:         return "Mobile";
			case DeviceCategory::Tablet:         return "Tablet";
			case DeviceCategory::Server:         return "Server";
			case DeviceCategory::VirtualMachine: return "VirtualMachine";
			case DeviceCategory::Modem:          return "Modem";
			case DeviceCategory::Router:         return "Router";
			case DeviceCategory::Switch:         return "Switch";
			case DeviceCategory::AccessPoint:    return "AccessPoint";
			case DeviceCategory::Printer:        return "Printer";
			case DeviceCategory::SmartTV:        return "SmartTV";
			case DeviceCategory::GamingConsole:  return "GamingConsole";
			case DeviceCategory::Audio:          return "Audio";
			case DeviceCategory::Camera:         return "Camera";
			case DeviceCategory::SmartHome:      return "SmartHome";
			case DeviceCategory::Wearable:       return "Wearable";
			case DeviceCategory::IoT:            return "IoT";
			case DeviceCategory::Unknown:
			default:                             return "Unknown";
		}
	}
}
