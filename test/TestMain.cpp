// MyZone — Programme principal des tests unitaires
// Compile et exécute tous les fichiers TestXxx.cpp
//
// Compilation :
//   g++ -std=c++17 -Wall -Wextra -Iinclude
//       test/TestMain.cpp
//       test/TestMacAddress.cpp
//       test/TestDeviceCategory.cpp
//       test/TestDatabase.cpp
//       test/TestEngine.cpp
//       src/MacAddress.cpp
//       src/DeviceCategory.cpp
//       src/DeviceProfile.cpp
//       src/Database.cpp
//       src/IdentificationEngine.cpp
//       src/modules/OuiModule.cpp
//       src/modules/DhcpModule.cpp
//       src/modules/HostnameModule.cpp
//       src/modules/P0fModule.cpp
//       src/modules/FingerBankModule.cpp
//       -o MyZoneTests
//
//   ./MyZoneTests

#include "TestFramework.hpp"

int main() {
    return myzone::test::runAllTests();
}
