cmake_minimum_required(VERSION 3.14)
project(MyZone VERSION 1.2.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(GNUInstallDirs)

option(MYZONE_BUILD_TESTS "Compiler la suite de tests unitaires" ON)
option(MYZONE_SANITIZE "Activer AddressSanitizer et UndefinedBehaviorSanitizer (GCC/Clang)" OFF)

# Version unique : project(VERSION) -> Version.hpp
configure_file(include/Version.hpp.in ${CMAKE_CURRENT_BINARY_DIR}/generated/Version.hpp @ONLY)

set(MYZONE_WARNINGS
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /utf-8>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic>)

# --- Bibliothèque réutilisable : moteur d'identification sans dépendance à la console ---
add_library(myzone_core STATIC
    src/DataLocator.cpp
    src/Database.cpp
    src/DeviceCategory.cpp
    src/DeviceProfile.cpp
    src/IdentificationEngine.cpp
    src/MacAddress.cpp
    src/modules/DhcpModule.cpp
    src/modules/FingerBankModule.cpp
    src/modules/HostnameModule.cpp
    src/modules/OuiModule.cpp
    src/modules/P0fModule.cpp)
target_include_directories(myzone_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/generated>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_compile_options(myzone_core PRIVATE ${MYZONE_WARNINGS})
target_compile_definitions(myzone_core PRIVATE
    MYZONE_INSTALL_DATADIR="${CMAKE_INSTALL_FULL_DATADIR}/myzone/data")
if(CMAKE_COMPILER_IS_GNUCXX AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9.0)
    target_link_libraries(myzone_core PUBLIC stdc++fs)
endif()

if(MYZONE_SANITIZE AND NOT MSVC)
    target_compile_options(myzone_core PUBLIC -fsanitize=address,undefined -fno-omit-frame-pointer)
    target_link_options(myzone_core PUBLIC -fsanitize=address,undefined)
endif()

# --- CLI ---
add_executable(MyZone
    main.cpp
    src/CliParser.cpp
    src/InteractiveCli.cpp
    src/UI.cpp)
target_link_libraries(MyZone PRIVATE myzone_core)
target_compile_options(MyZone PRIVATE ${MYZONE_WARNINGS})

# Copie data/ à côté des binaires à chaque build (et non une seule fois à la configuration)
add_custom_target(myzone_data ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/data ${CMAKE_CURRENT_BINARY_DIR}/data
    COMMENT "Copie des bases de signatures")

# --- Tests ---
if(MYZONE_BUILD_TESTS)
    enable_testing()
    file(GLOB TEST_SOURCES CONFIGURE_DEPENDS "test/*.cpp")
    add_executable(MyZoneTests ${TEST_SOURCES})
    target_link_libraries(MyZoneTests PRIVATE myzone_core)
    target_compile_options(MyZoneTests PRIVATE ${MYZONE_WARNINGS})
    add_dependencies(MyZoneTests myzone_data)
    add_test(NAME MyZoneUnitTests COMMAND MyZoneTests WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

    # Tests de bout en bout du CLI : codes de sortie et formats machine
    add_test(NAME cli_version COMMAND MyZone --version)
    set_tests_properties(cli_version PROPERTIES PASS_REGULAR_EXPRESSION "v${PROJECT_VERSION}")
    add_test(NAME cli_invalid_mac COMMAND MyZone --mac zz --json)
    set_tests_properties(cli_invalid_mac PROPERTIES WILL_FAIL TRUE)
    add_test(NAME cli_unknown_option COMMAND MyZone --bogus)
    set_tests_properties(cli_unknown_option PROPERTIES WILL_FAIL TRUE)
    add_test(NAME cli_missing_value COMMAND MyZone --mac)
    set_tests_properties(cli_missing_value PROPERTIES WILL_FAIL TRUE)
    add_test(NAME cli_json_stable_keys
             COMMAND MyZone --mac E8:0A:B9:00:00:01 --json
             WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    set_tests_properties(cli_json_stable_keys PROPERTIES
        PASS_REGULAR_EXPRESSION "\"category\":\"Router\"")
endif()

# --- Installation ---
install(TARGETS MyZone myzone_core
    EXPORT MyZoneTargets
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/myzone
    FILES_MATCHING PATTERN "*.hpp"
    PATTERN "UI.hpp" EXCLUDE
    PATTERN "CliParser.hpp" EXCLUDE
    PATTERN "InteractiveCli.hpp" EXCLUDE)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/generated/Version.hpp
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/myzone)
install(DIRECTORY data/
    DESTINATION ${CMAKE_INSTALL_DATADIR}/myzone/data
    PATTERN "README_DATABASES.md" EXCLUDE)
install(FILES LICENSE THIRD_PARTY_LICENSES.md
    DESTINATION ${CMAKE_INSTALL_DATADIR}/doc/myzone)
install(EXPORT MyZoneTargets
    NAMESPACE MyZone::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/MyZone)

include(CMakePackageConfigHelpers)
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/MyZoneConfig.cmake
    "include(\"\${CMAKE_CURRENT_LIST_DIR}/MyZoneTargets.cmake\")\n")
write_basic_package_version_file(${CMAKE_CURRENT_BINARY_DIR}/MyZoneConfigVersion.cmake
    COMPATIBILITY SameMinorVersion)
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/MyZoneConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/MyZoneConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/MyZone)
