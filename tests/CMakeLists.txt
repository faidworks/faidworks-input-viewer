# CMake minimum version and project name for tests
cmake_minimum_required(VERSION 3.10)
project(FaidworksInputViewerTests)

# Include directories
include_directories(../include)


# Glob the test source files
file(GLOB TEST_SOURCES
    "test_main.cpp"
    "test_controller.cpp"
    "test_settings.cpp"
    "test_display_utils.cpp"
    "test_font_settings.cpp"
    "test_layout.cpp"
    "test_mappings.cpp"
)

# Create the test executable
add_executable(test_app ${TEST_SOURCES})

# Link the test executable with necessary libraries
if(WIN32)
    target_link_libraries(test_app GTest::gtest GTest::gtest_main faidworks-input-viewer-lib Xinput SFML::Graphics SFML::Audio SFML::Network opengl32 winmm gdi32)
else()
    target_link_libraries(test_app GTest::gtest GTest::gtest_main faidworks-input-viewer-lib SDL3::SDL3 SFML::Graphics SFML::Audio SFML::Network)
endif()

# Conditionally link pthread for Unix-like systems
if(UNIX)
    target_link_libraries(test_app pthread)
endif()

# Optionally, add a test entry for CTest
add_test(NAME FaidworksInputViewerTests COMMAND test_app)
