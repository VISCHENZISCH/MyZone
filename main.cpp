#include <iostream>
#include <string>
#include <stdexcept>
#include <vector>

#include "include/MacAddress.hpp"
#include "include/DeviceCategory.hpp"
#include "include/Database.hpp"

using namespace std;
using namespace myzone;

// Fonction utilitaire pour tester une adresse MAC dans la DB
void testMacLookup(const Database& db, const string& macStr) {
    try {
        MacAddress mac(macStr);
        cout << "MAC: " << mac.toString() << " | Yes: " << mac.yes() << endl;

        DeviceInfo info;
        if (db.lookup(mac, info)) {
            cout << "  -> Manufacturer: " << info.companyName << endl;
            cout << "  -> Category:     " << toString(info.category) << endl;
        } else {
            cout << "  -> Not found in database." << endl;
        }
    } catch (const std::invalid_argument& e) {
        cout << "Invalid MAC: " << e.what() << endl;
    }
    cout << "------------------------------------------\n";
}

int main() {
    cout << "=== MYZONE GLOBAL TEST ===\n" << endl;

    // Load CSV database
    cout << "Loading database..." << endl;
    
    // Assure-toi que le chemin vers lookup.csv est bon par rapport à l'exécutable
    Database db("data/lookup.csv"); 
    
    cout << "Database loaded.\n" << endl;

    vector<string> macsToTest = {
        "C4:A0:52:11:22:33", // Should match Motorola (Phone)
        "3C:08:CD:AA:BB:CC", // Should match Juniper Networks (Router)
        "00:1A:EB:99:88:77", // Should match Allied Telesis (Switch)
        "11-22-33-44-55-66", // Unknown
        "00:11:22"           // Error: Invalid length
    };

    for (const string& mac : macsToTest) {
        testMacLookup(db, mac);
    }

    return 0;
}
