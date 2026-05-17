# CMake minimum version and project name for tests
cmake_minimum_required(VERSION 3.10)
project(FaidworksInputViewerTests)

# Include directories
include_directories(../include)

# Find SDL3
find_package(SDL3 REQUIRED)

# Find SFML
find_package(SFML REQUIRED COMPONENTS Graphics Audio Network)

# Include Google Test
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

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
target_link_libraries(test_app ${GTEST_LIBRARIES} faidworks-input-viewer-lib SDL3::SDL3 SFML::Graphics SFML::Audio SFML::Network)

# Conditionally link pthread for Unix-like systems
if(UNIX)
    target_link_libraries(test_app pthread)
endif()

# Optionally, add a test entry for CTest
add_test(NAME FaidworksInputViewerTests COMMAND test_app)
