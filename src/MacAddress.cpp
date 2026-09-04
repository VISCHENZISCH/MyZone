#include "../include/MacAddress.hpp"
#include <algorithm>
#include <cctype>
#include <sstream>
#include <iomanip>
#include <stdexcept>

namespace myzone {
	
	//fonction utilitaire qui nettoie la chaîne d'entrée
	std::string MacAddress::normalizeInput(const std::string& raw) {
		
		std::string clean;
		for (char c : raw) {
			if (std::isxdigit(c)) {
				clean += (char)std::toupper(c);
			}
		}
		return clean;
	}
	
	MacAddress::MacAddress(const std::string& raw) {
		
		std::string clean = normalizeInput(raw);
		if (clean.length() != 12) {
			valid_ = false;
			throw std::invalid_argument("Format d'adresse MAC invalide : " + raw);
		}
		
		try {
			
			for (size_t i = 0; i < 6; ++i) {
				std::string byteString = clean.substr(i * 2, 2);
				bytes_[i] = static_cast<unsigned char> (std::stoi(byteString, nullptr, 16));
			}
			
			valid_ = true;
			
		} catch (const std::exception&) {
			valid_ = false;
			throw std::invalid_argument("Erreur  de parsing (lecture) de l'adresse MAC : " + raw);
		}
	}
	
	// toString() pour afficher l'adresse complète
	
	std::string MacAddress::toString() const {
		if (!valid_) return "INVALID_MAC";
		
		std::stringstream ss;
		
		ss << std::hex << std::uppercase << std::setfill('0');
		
		for (size_t i = 0; i < 6; ++i) {
			
			ss << std::setw(2) << static_cast<int>(bytes_[i]);
			
			if (i < 5) {
				ss << ":"; // AJout du séparateur  ':' 
			}
		}
		

			return ss.str();
	}
	
	//Méthode yes()
	
	std::string MacAddress::yes() const {
		
		if (!valid_) return "";
		std::stringstream ss;
		ss << std::hex << std::uppercase << std::setfill('0');
		
		for (size_t i = 0; i < 3; ++i) {
			ss << std::setw(2) << static_cast<int>(bytes_[i]);
		}
		
			return ss.str();
	}
	
}