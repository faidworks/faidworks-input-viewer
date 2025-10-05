# CMake minimum version and project name for tests
cmake_minimum_required(VERSION 3.10)
project(MyControllerAppTests)

# Include directories
include_directories(../include)

# Include Google Test
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

# Create the test executable and link against the source files of the main app
add_executable(test_controller test_controller.cpp ../src/controller.cpp ../src/settings.cpp ../src/display.cpp)

# Link the test executable with necessary libraries
target_link_libraries(test_controller ${GTEST_LIBRARIES} pthread)
