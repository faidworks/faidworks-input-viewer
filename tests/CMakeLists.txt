# CMake minimum version and project name for tests
cmake_minimum_required(VERSION 3.10)
project(MyControllerAppTests)

# Include directories
include_directories(../include)

# Include Google Test
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

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

# Glob the test source files
file(GLOB TEST_SOURCES
    "test_main.cpp"
    "test_controller.cpp"
)

# Create the test executable
add_executable(test_app ${TEST_SOURCES})

# Link the test executable with necessary libraries
target_link_libraries(test_app ${GTEST_LIBRARIES} MyControllerLib SDL2::SDL2 SFML::Graphics SFML::Audio SFML::Network)

# Conditionally link pthread for Unix-like systems
if(UNIX)
    target_link_libraries(test_app pthread)
endif()

# Optionally, add a test entry for CTest
add_test(NAME MyControllerTests COMMAND test_app)
