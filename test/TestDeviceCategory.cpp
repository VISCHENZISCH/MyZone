#include "TestFramework.hpp"
#include "../include/DeviceCategory.hpp"

// ──── classifyFromDeviceType ────

TEST(DeviceCategory_Computer) {
    ASSERT_TRUE(myzone::classifyFromDeviceType("Computer") == myzone::DeviceCategory::Computer);
}

TEST(DeviceCategory_Phone) {
    ASSERT_TRUE(myzone::classifyFromDeviceType("Phone") == myzone::DeviceCategory::Mobile);
}

TEST(DeviceCategory_Tablet) {
    ASSERT_TRUE(myzone::classifyFromDeviceType("Tablet") == myzone::DeviceCategory::Tablet);
}

TEST(DeviceCategory_Server) {
    ASSERT_TRUE(myzone::classifyFromDeviceType("Server") == myzone::DeviceCategory::Server);
}

TEST(DeviceCategory_VM) {
    ASSERT_TRUE(myzone::classifyFromDeviceType("VM") == myzone::DeviceCategory::VirtualMachine);
}

TEST(DeviceCategory_Router) {
    ASSERT_TRUE(myzone::classifyFromDeviceType("Router") == myzone::DeviceCategory::Router);
}

TEST(DeviceCategory_Switch) {
    ASSERT_TRUE(myzone::classifyFromDeviceType("Switch") == myzone::DeviceCategory::Switch);
}

TEST(DeviceCategory_Modem) {
    ASSERT_TRUE(myzone::classifyFromDeviceType("Modem") == myzone::DeviceCategory::Modem);
}

TEST(DeviceCategory_AP) {
    ASSERT_TRUE(myzone::classifyFromDeviceType("AP") == myzone::DeviceCategory::AccessPoint);
}

TEST(DeviceCategory_IoT) {
    ASSERT_TRUE(myzone::classifyFromDeviceType("IoT") == myzone::DeviceCategory::IoT);
}

TEST(DeviceCategory_SmartHome) {
    ASSERT_TRUE(myzone::classifyFromDeviceType("SmartHome") == myzone::DeviceCategory::SmartHome);
}

TEST(DeviceCategory_Camera) {
    ASSERT_TRUE(myzone::classifyFromDeviceType("Camera") == myzone::DeviceCategory::Camera);
}

TEST(DeviceCategory_Printer) {
    ASSERT_TRUE(myzone::classifyFromDeviceType("Printer") == myzone::DeviceCategory::Printer);
}

TEST(DeviceCategory_TV) {
    ASSERT_TRUE(myzone::classifyFromDeviceType("TV") == myzone::DeviceCategory::SmartTV);
}

TEST(DeviceCategory_Console) {
    ASSERT_TRUE(myzone::classifyFromDeviceType("Console") == myzone::DeviceCategory::GamingConsole);
}

TEST(DeviceCategory_Wearable) {
    ASSERT_TRUE(myzone::classifyFromDeviceType("Wearable") == myzone::DeviceCategory::Wearable);
}

TEST(DeviceCategory_Audio) {
    ASSERT_TRUE(myzone::classifyFromDeviceType("Audio") == myzone::DeviceCategory::Audio);
}

TEST(DeviceCategory_Unknown) {
    ASSERT_TRUE(myzone::classifyFromDeviceType("") == myzone::DeviceCategory::Unknown);
    ASSERT_TRUE(myzone::classifyFromDeviceType("xyz") == myzone::DeviceCategory::Unknown);
}

// ──── toString ────

TEST(DeviceCategory_toStringComputer) {
    ASSERT_EQ(myzone::toString(myzone::DeviceCategory::Computer), "Ordinateur");
}

TEST(DeviceCategory_toStringMobile) {
    ASSERT_EQ(myzone::toString(myzone::DeviceCategory::Mobile), "Smartphone");
}

TEST(DeviceCategory_toStringRouter) {
    ASSERT_EQ(myzone::toString(myzone::DeviceCategory::Router), "Routeur");
}

TEST(DeviceCategory_toStringUnknown) {
    ASSERT_EQ(myzone::toString(myzone::DeviceCategory::Unknown), "Inconnu");
}

TEST(DeviceCategory_toStringPrinter) {
    ASSERT_EQ(myzone::toString(myzone::DeviceCategory::Printer), "Imprimante");
}

TEST(DeviceCategory_toStringSmartTV) {
    ASSERT_EQ(myzone::toString(myzone::DeviceCategory::SmartTV), "Smart TV");
}

TEST(DeviceCategory_toStringIoT) {
    ASSERT_EQ(myzone::toString(myzone::DeviceCategory::IoT), "IoT (Objet Connecté)");
}
