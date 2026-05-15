# CMake minimum version and project name
cmake_minimum_required(VERSION 4.1)
project(MyControllerApp)

# C++ standards
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Specify the include directories
include_directories(../include)

# Find SDL3 library
find_package(SDL3 REQUIRED)

if(NOT SDL3_FOUND)
    # include(FetchContent)
    # FetchContent_Declare(SDL3
    #     GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    #     GIT_TAG release-3.2.24
    #     GIT_SHALLOW ON
    #     EXCLUDE_FROM_ALL
    #     SYSTEM)
    # FetchContent_MakeAvailable(SDL3)
endif()

# Find SFML 3
find_package(SFML REQUIRED COMPONENTS Graphics Audio Network)

if(NOT SFML_FOUND)
    # include(FetchContent)
    # FetchContent_Declare(SFML
    #     GIT_REPOSITORY https://github.com/SFML/SFML.git
    #     GIT_TAG 3.0.2
    #     GIT_SHALLOW ON
    #     EXCLUDE_FROM_ALL
    #     SYSTEM)
    # FetchContent_MakeAvailable(SFML)
endif()

# Glob the source files
file(GLOB SOURCES
    "main.cpp"
    "controller.cpp"
    "settings.cpp"
    "display.cpp"
    "display_events.cpp"
    "display_render.cpp"
    "display_layout.cpp"
    "display_settings.cpp"
)

# Create a library from the source files
add_library(MyControllerLib STATIC ${SOURCES})

# Optionally create an executable
add_executable(MyControllerApp ${SOURCES})
target_link_libraries(MyControllerApp PRIVATE MyControllerLib)


# Link against the appropriate libraries based on platform
if (WIN32)
    target_link_libraries(MyControllerApp PRIVATE Xinput SFML::Graphics SFML::Audio SFML::Network)
else()
    target_link_libraries(MyControllerApp PRIVATE SDL3::SDL3 SDL3::Headers SFML::Graphics SFML::Audio SFML::Network)
endif()
