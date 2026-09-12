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
}