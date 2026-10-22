#include "../include/DataLocator.hpp"

#include <cstdlib>
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
    // 1. Surcharge explicite par l'utilisateur
    if (const char* fromEnv = std::getenv("MYZONE_DATA")) {
        if (isDirectory(fromEnv)) {
            return fromEnv;
        }
    }

    // 2. Arborescence de développement : data/ à côté ou au-dessus du binaire / répertoire courant
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

    // 3. Installation : <prefix>/bin/MyZone -> <prefix>/share/myzone/data
    if (executablePath != nullptr) {
        const std::string binDir = getParentDirectory(executablePath);
        if (!binDir.empty()) {
            const std::string installed = binDir + "/../share/myzone/data";
            if (isDirectory(installed)) {
                return installed;
            }
        }
    }
#ifdef MYZONE_INSTALL_DATADIR
    if (isDirectory(MYZONE_INSTALL_DATADIR)) {
        return MYZONE_INSTALL_DATADIR;
    }
#endif

    return "data";
}

} // namespace myzone
