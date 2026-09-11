#ifndef MYZONE_DEVICE_CATEGORY_HPP
#define MYZONE_DEVICE_CATEGORY_HPP

#include <string>

namespace myzone {

enum class DeviceCategory {
    Computer,
    Mobile,
    Tablet,
    Server,
    VirtualMachine,

    Modem,
    Router,
    Switch,
    AccessPoint,

    Printer,
    SmartTV,
    GamingConsole,
    Audio,

    Camera,
    SmartHome,
    Wearable,
    IoT,

    Unknown
};

DeviceCategory classifyFromDeviceType(const std::string& deviceType);
std::string toString(DeviceCategory category);

}

#endif // MYZONE_DEVICE_CATEGORY_HPP