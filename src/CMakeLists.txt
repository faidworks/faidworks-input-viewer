# CMake minimum version and project name
cmake_minimum_required(VERSION 3.10)
project(MyControllerApp)

# C++ standards
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Specify the include directories
include_directories(../include)

# Find SDL2 library
find_package(SDL2 REQUIRED)

if(SDL2_FOUND)
    message(STATUS "SDL2 found: ${SDL2_INCLUDE_DIRS}")
    include_directories(${SDL2_INCLUDE_DIRS})
else()
    message(FATAL_ERROR "SDL2 not found!")
endif()

# Find SFML 3
find_package(SFML 3 REQUIRED COMPONENTS Graphics Audio Network)

if(SFML_FOUND)
    message(STATUS "SFML found: ${SFML_INCLUDE_DIRS}")
    include_directories(${SFML_INCLUDE_DIRS})
else()
    message(FATAL_ERROR "SFML not found!")
endif()

# Glob the source files
file(GLOB SOURCES
    "main.cpp"
    "controller.cpp"
    "settings.cpp"
    "display.cpp"
)

# Create a library from the source files
add_library(MyControllerLib STATIC ${SOURCES})

# Optionally create an executable
add_executable(MyControllerApp ${SOURCES})
target_link_libraries(MyControllerApp MyControllerLib ...)


# Link against the appropriate libraries based on platform
if (WIN32)
    target_link_libraries(MyControllerApp Xinput SFML::Graphics SFML::Audio SFML::Network)
else()
    target_link_libraries(MyControllerApp SDL2::SDL2 SFML::Graphics SFML::Audio SFML::Network)
endif()
