cmake_minimum_required(VERSION 3.14)
project(MyZone VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB SOURCES "src/*.cpp" "src/modules/*.cpp")
file(GLOB TEST_SOURCES "test/*.cpp")

add_executable(MyZone main.cpp ${SOURCES})
target_include_directories(MyZone PRIVATE include)
target_compile_options(MyZone PRIVATE -Wall -Wextra -Wpedantic)

add_executable(MyZoneTests ${TEST_SOURCES} ${SOURCES})
target_include_directories(MyZoneTests PRIVATE include)
target_compile_options(MyZoneTests PRIVATE -Wall -Wextra -Wpedantic)

if(CMAKE_COMPILER_IS_GNUCXX AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9.0)
    target_link_libraries(MyZone PRIVATE stdc++fs)
    target_link_libraries(MyZoneTests PRIVATE stdc++fs)
endif()

file(COPY data DESTINATION ${CMAKE_BINARY_DIR})
