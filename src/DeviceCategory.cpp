#include "../include/DeviceCategory.hpp"

namespace myzone {

	DeviceCategory classifyFromDeviceType(const std::string& deviceType) {

		if (deviceType == "Computer")       return DeviceCategory::Computer;
		if (deviceType == "Phone")          return DeviceCategory::Mobile;
		if (deviceType == "Tablet")         return DeviceCategory::Tablet;
		if (deviceType == "Server")         return DeviceCategory::Server;
		if (deviceType == "VM")             return DeviceCategory::VirtualMachine;

		if (deviceType == "Router")         return DeviceCategory::Router;
		if (deviceType == "Switch")         return DeviceCategory::Switch;
		if (deviceType == "Modem")          return DeviceCategory::Modem;
		if (deviceType == "AP")             return DeviceCategory::AccessPoint;

		if (deviceType == "IoT")            return DeviceCategory::IoT;
		if (deviceType == "SmartHome")      return DeviceCategory::SmartHome;
		if (deviceType == "Camera")         return DeviceCategory::Camera;
		if (deviceType == "Printer")        return DeviceCategory::Printer;
		if (deviceType == "TV")             return DeviceCategory::SmartTV;
		if (deviceType == "Console")        return DeviceCategory::GamingConsole;
		if (deviceType == "Wearable")       return DeviceCategory::Wearable;
		if (deviceType == "Audio")          return DeviceCategory::Audio;

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