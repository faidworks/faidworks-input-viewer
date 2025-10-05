# CMake minimum version and project name
cmake_minimum_required(VERSION 3.10)
project(MyControllerApp)

# C++ standards
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Specify the include directories
include_directories(include)

# Find SDL2 library
find_package(SDL2 REQUIRED)

# Find SFML
find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)

# Create the main executable
add_executable(MyControllerApp main.cpp controller.cpp settings.cpp display.cpp)

# Link against the appropriate libraries based on platform
if (WIN32)
    target_link_libraries(MyControllerApp Xinput sfml-graphics sfml-window sfml-system)
else()
    target_link_libraries(MyControllerApp SDL2::SDL2 sfml-graphics sfml-window sfml-system)
endif()
