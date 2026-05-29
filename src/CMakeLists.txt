include_directories(../include)

# Source files (no main.cpp — library is linked by both app and tests)
file(GLOB LIB_SOURCES
    "controller.cpp"
    "settings.cpp"
    "display.cpp"
    "display_events.cpp"
    "display_render.cpp"
    "display_layout.cpp"
    "display_settings.cpp"
)

add_library(faidworks-input-viewer-lib STATIC ${LIB_SOURCES})

if (MSVC)
    target_compile_definitions(faidworks-input-viewer-lib PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()

if (WIN32)
    target_link_libraries(faidworks-input-viewer-lib PUBLIC
        Xinput
        SFML::Graphics SFML::Audio SFML::Network
        opengl32 winmm gdi32
    )
else()
    target_link_libraries(faidworks-input-viewer-lib PUBLIC
        SDL3::SDL3 SDL3::Headers
        SFML::Graphics SFML::Audio SFML::Network
    )
endif()

add_executable(faidworks-input-viewer main.cpp)
target_link_libraries(faidworks-input-viewer PRIVATE faidworks-input-viewer-lib)

if (WIN32)
    set_target_properties(faidworks-input-viewer PROPERTIES WIN32_EXECUTABLE TRUE)
    if (MSVC)
        target_link_options(faidworks-input-viewer PRIVATE /ENTRY:mainCRTStartup)
    endif()
endif()
