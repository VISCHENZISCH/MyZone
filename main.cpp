#include <iostream>
#include <string>
#include <stdexcept>

#include "include/MacAddress.hpp"

using namespace std;
using namespace myzone;

int main() {
    cout << "=== TEST DE LA CLASSE MAC ADDRESS ===" << endl;
    try {
        // Test 1 : Avec des tirets (Format Windows classique)
        cout << "\nTest 1 (Tirets) :" << endl;
        MacAddress mac1("00-1A-2B-3C-4D-5E");
        if (mac1.isValid()) {
            cout << "Adresse formatée : " << mac1.toString() << endl;
            cout << "OUI (Fabricant)  : " << mac1.yes() << endl;
        }
        // Test 2 : Avec des deux-points et minuscules (Format Linux/Mac classique)
        cout << "\nTest 2 (Deux-points & minuscules) :" << endl;
        MacAddress mac2("a1:b2:c3:d4:e5:f6");
        cout << "Adresse formatée : " << mac2.toString() << endl;
        cout << "OUI (Fabricant)  : " << mac2.yes() << endl;
        
        // Test 3 : Sans aucun séparateur
        cout << "\nTest 3 (Aucun séparateur) :" << endl;
        MacAddress mac3("112233445566");
        cout << "Adresse formatée : " << mac3.toString() << endl;
        // Test 4 : Testons une erreur exprès (l'adresse est trop courte)
        cout << "\nTest 4 (Erreur volontaire) :" << endl;
        MacAddress macErreur("00:11:22"); 
        
        // La ligne suivante ne s'affichera pas car l'erreur interrompt le code plus haut
        cout << macErreur.toString() << endl; 
    } catch (const std::invalid_argument& e) {
        // Cette partie va "attraper" l'erreur du Test 4 et afficher notre message
        cout << "-> Erreur interceptee avec succes : " << e.what() << endl;
    }
    cout << "\nFin des tests." << endl;
    return 0;
}