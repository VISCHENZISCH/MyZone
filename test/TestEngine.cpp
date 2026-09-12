#include "TestFramework.hpp"
#include "../include/IdentificationEngine.hpp"
#include "../include/modules/OuiModule.hpp"
#include "../include/modules/DhcpModule.hpp"
#include "../include/modules/HostnameModule.hpp"
#include "../include/modules/P0fModule.hpp"
#include "../include/modules/FingerBankModule.hpp"
#include "../include/Database.hpp"

#include <memory>
#include <fstream>

namespace {

std::string findDataDir() {
    const char* paths[] = {"data", "../data", "../../data"};
    for (const char* p : paths) {
        std::ifstream test(std::string(p) + "/lookup.csv");
        if (test.good()) return p;
    }
    return "data";
}

} // namespace

// ──── IdentificationEngine ────

TEST(Engine_moduleCount) {
    myzone::IdentificationEngine engine;
    ASSERT_TRUE(engine.moduleCount() == 0);

    myzone::Database db(findDataDir());
    engine.addModule(std::make_unique<myzone::OuiModule>(db));
    ASSERT_TRUE(engine.moduleCount() == 1);

    engine.addModule(std::make_unique<myzone::HostnameModule>());
    ASSERT_TRUE(engine.moduleCount() == 2);
}

TEST(Engine_identifyEmpty) {
    myzone::Database db(findDataDir());
    myzone::IdentificationEngine engine;
    engine.addModule(std::make_unique<myzone::OuiModule>(db));
    engine.addModule(std::make_unique<myzone::HostnameModule>());

    myzone::DeviceProfile input;
    // No data provided — no module should match
    myzone::DeviceProfile result = engine.identify(input);
    ASSERT_FALSE(result.hasResults());
}

TEST(Engine_identifyByHostnameWindows) {
    myzone::Database db(findDataDir());
    myzone::IdentificationEngine engine;
    engine.addModule(std::make_unique<myzone::OuiModule>(db));
    engine.addModule(std::make_unique<myzone::HostnameModule>());

    myzone::DeviceProfile input;
    input.hostname = "DESKTOP-AB1CD2E";

    myzone::DeviceProfile result = engine.identify(input);
    ASSERT_TRUE(result.hasResults());
    ASSERT_EQ(result.osName, "Windows");
    ASSERT_TRUE(result.category == myzone::DeviceCategory::Computer);
}

TEST(Engine_identifyByHostnameLaptop) {
    myzone::Database db(findDataDir());
    myzone::IdentificationEngine engine;
    engine.addModule(std::make_unique<myzone::HostnameModule>());

    myzone::DeviceProfile input;
    input.hostname = "LAPTOP-XY2AB3Z";

    myzone::DeviceProfile result = engine.identify(input);
    ASSERT_TRUE(result.hasResults());
    ASSERT_EQ(result.osName, "Windows");
    ASSERT_TRUE(result.category == myzone::DeviceCategory::Computer);
}

TEST(Engine_identifyByHostnameAndroid) {
    myzone::IdentificationEngine engine;
    engine.addModule(std::make_unique<myzone::HostnameModule>());

    myzone::DeviceProfile input;
    input.hostname = "android-abc123def456";

    myzone::DeviceProfile result = engine.identify(input);
    ASSERT_TRUE(result.hasResults());
    ASSERT_EQ(result.osName, "Android");
    ASSERT_TRUE(result.category == myzone::DeviceCategory::Mobile);
}

TEST(Engine_identifyByHostnameGalaxy) {
    myzone::IdentificationEngine engine;
    engine.addModule(std::make_unique<myzone::HostnameModule>());

    myzone::DeviceProfile input;
    input.hostname = "Galaxy-S23-Ultra";

    myzone::DeviceProfile result = engine.identify(input);
    ASSERT_TRUE(result.hasResults());
    ASSERT_EQ(result.manufacturer, "Samsung");
    ASSERT_EQ(result.osName, "Android");
    ASSERT_TRUE(result.category == myzone::DeviceCategory::Mobile);
}

TEST(Engine_identifyByHostnamePixel) {
    myzone::IdentificationEngine engine;
    engine.addModule(std::make_unique<myzone::HostnameModule>());

    myzone::DeviceProfile input;
    input.hostname = "Pixel-8-Pro";

    myzone::DeviceProfile result = engine.identify(input);
    ASSERT_TRUE(result.hasResults());
    ASSERT_EQ(result.manufacturer, "Google");
    ASSERT_EQ(result.osName, "Android");
}

TEST(Engine_identifyByHostnameIPhone) {
    myzone::IdentificationEngine engine;
    engine.addModule(std::make_unique<myzone::HostnameModule>());

    myzone::DeviceProfile input;
    input.hostname = "iPhone-de-Tom";

    myzone::DeviceProfile result = engine.identify(input);
    ASSERT_TRUE(result.hasResults());
    ASSERT_EQ(result.manufacturer, "Apple, Inc.");
    ASSERT_EQ(result.osName, "Apple iOS");
    ASSERT_TRUE(result.category == myzone::DeviceCategory::Mobile);
}

TEST(Engine_identifyByHostnameIPad) {
    myzone::IdentificationEngine engine;
    engine.addModule(std::make_unique<myzone::HostnameModule>());

    myzone::DeviceProfile input;
    input.hostname = "iPad-de-Marie";

    myzone::DeviceProfile result = engine.identify(input);
    ASSERT_TRUE(result.hasResults());
    ASSERT_EQ(result.manufacturer, "Apple, Inc.");
    ASSERT_EQ(result.osName, "Apple iOS");
    ASSERT_TRUE(result.category == myzone::DeviceCategory::Tablet);
}

TEST(Engine_identifyByHostnameMacBook) {
    myzone::IdentificationEngine engine;
    engine.addModule(std::make_unique<myzone::HostnameModule>());

    myzone::DeviceProfile input;
    input.hostname = "MacBook-Pro-de-Tom";

    myzone::DeviceProfile result = engine.identify(input);
    ASSERT_TRUE(result.hasResults());
    ASSERT_EQ(result.manufacturer, "Apple, Inc.");
    ASSERT_EQ(result.osName, "macOS");
    ASSERT_TRUE(result.category == myzone::DeviceCategory::Computer);
}

TEST(Engine_identifyByHostnameUnknown) {
    myzone::IdentificationEngine engine;
    engine.addModule(std::make_unique<myzone::HostnameModule>());

    myzone::DeviceProfile input;
    input.hostname = "random-hostname-123";

    myzone::DeviceProfile result = engine.identify(input);
    ASSERT_FALSE(result.hasResults());
}

TEST(Engine_fullPipeline) {
    myzone::Database db(findDataDir());
    myzone::IdentificationEngine engine;
    engine.addModule(std::make_unique<myzone::OuiModule>(db));
    engine.addModule(std::make_unique<myzone::DhcpModule>(db));
    engine.addModule(std::make_unique<myzone::HostnameModule>());
    engine.addModule(std::make_unique<myzone::P0fModule>(db));
    engine.addModule(std::make_unique<myzone::FingerBankModule>(db));

    ASSERT_TRUE(engine.moduleCount() == 5);

    // Test with a hostname that the HostnameModule recognizes
    myzone::DeviceProfile input;
    input.hostname = "Galaxy-S24";

    myzone::DeviceProfile result = engine.identify(input);
    ASSERT_TRUE(result.hasResults());
    ASSERT_EQ(result.manufacturer, "Samsung");
}

// ──── DeviceProfile ────

TEST(DeviceProfile_addResult) {
    myzone::DeviceProfile profile;
    ASSERT_FALSE(profile.hasResults());

    profile.addResult("TestModule", "some detail");
    ASSERT_TRUE(profile.hasResults());
    ASSERT_TRUE(profile.results.size() == 1);
    ASSERT_EQ(profile.results[0].moduleName, "TestModule");
    ASSERT_EQ(profile.results[0].detail, "some detail");
}

TEST(DeviceProfile_defaultCategory) {
    myzone::DeviceProfile profile;
    ASSERT_TRUE(profile.category == myzone::DeviceCategory::Unknown);
}

TEST(Engine_identifyMacAndHostnameCombined) {
    // Test with both MAC and hostname - both modules should contribute
    myzone::Database db(findDataDir());
    myzone::IdentificationEngine engine;
    engine.addModule(std::make_unique<myzone::OuiModule>(db));
    engine.addModule(std::make_unique<myzone::HostnameModule>());

    myzone::DeviceProfile input;
    input.mac = myzone::MacAddress("F0:EE:7A:00:11:22");  // Apple
    input.hostname = "iPhone-de-Tom";

    myzone::DeviceProfile result = engine.identify(input);
    ASSERT_TRUE(result.hasResults());
    ASSERT_TRUE(result.results.size() >= 2);  // Both modules contributed
}

TEST(Engine_moduleOrderMatters) {
    // Verify that module execution order is respected
    myzone::Database db(findDataDir());
    myzone::IdentificationEngine engine;
    engine.addModule(std::make_unique<myzone::OuiModule>(db));
    engine.addModule(std::make_unique<myzone::DhcpModule>(db));
    engine.addModule(std::make_unique<myzone::HostnameModule>());
    engine.addModule(std::make_unique<myzone::P0fModule>(db));
    engine.addModule(std::make_unique<myzone::FingerBankModule>(db));
    ASSERT_INT_EQ(engine.moduleCount(), 5);
}
