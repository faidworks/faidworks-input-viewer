cmake_minimum_required(VERSION 4.1)
project(FaidworksInputViewer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(../include)

# Static linking for distribution
set(BUILD_SHARED_LIBS OFF)

# Fetch SDL3 from source
include(FetchContent)

if(NOT WIN32)
    FetchContent_Declare(SDL3
        GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
        GIT_TAG release-3.2.14
        GIT_SHALLOW ON
        EXCLUDE_FROM_ALL
        SYSTEM)
    FetchContent_MakeAvailable(SDL3)
endif()

# Fetch SFML from source
set(SFML_STATIC_LIBRARIES TRUE)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.2
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)

# Source files
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

add_library(faidworks-input-viewer-lib STATIC ${SOURCES})

add_executable(faidworks-input-viewer ${SOURCES})
target_link_libraries(faidworks-input-viewer PRIVATE faidworks-input-viewer-lib)

if (WIN32)
    target_link_libraries(faidworks-input-viewer PRIVATE
        Xinput
        SFML::Graphics SFML::Audio SFML::Network
        opengl32 winmm gdi32
    )
    set_target_properties(faidworks-input-viewer PROPERTIES WIN32_EXECUTABLE TRUE)
else()
    target_link_libraries(faidworks-input-viewer PRIVATE
        SDL3::SDL3 SDL3::Headers
        SFML::Graphics SFML::Audio SFML::Network
    )
endif()
