#include "../include/DataLocator.hpp"

#include <vector>
#include <sys/stat.h>

#ifdef _WIN32
#include <direct.h>
#else
#include <unistd.h>
#endif

namespace myzone {

namespace {

bool isDirectory(const std::string& path) {
    struct stat info;
    if (stat(path.c_str(), &info) != 0) {
        return false;
    }
    return (info.st_mode & S_IFDIR) != 0;
}

std::string getCurrentDirectory() {
    char buffer[4096];
#ifdef _WIN32
    if (_getcwd(buffer, sizeof(buffer)) != nullptr) {
#else
    if (getcwd(buffer, sizeof(buffer)) != nullptr) {
#endif
        return std::string(buffer);
    }
    return ".";
}

std::string getParentDirectory(const std::string& path) {
    std::size_t pos = path.find_last_of("/\\");
    if (pos == std::string::npos) {
        return "";
    }
    return path.substr(0, pos);
}

} // namespace

std::string DataLocator::locate(const char* executablePath) {
    std::vector<std::string> roots;
    roots.push_back(getCurrentDirectory());

    if (executablePath != nullptr) {
        std::string execStr(executablePath);
        std::string execParent = getParentDirectory(execStr);
        if (!execParent.empty()) {
            roots.push_back(execParent);
        }
    }

    for (std::string root : roots) {
        for (int level = 0; level < 4 && !root.empty(); ++level) {
            std::string candidate = root;
            if (!candidate.empty() && candidate.back() != '/' && candidate.back() != '\\') {
                candidate += "/";
            }
            candidate += "data";

            if (isDirectory(candidate)) {
                return candidate;
            }
            root = getParentDirectory(root);
        }
    }

    return "data";
}

} // namespace myzone
