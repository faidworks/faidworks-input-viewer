# CMake minimum version and project name for tests
cmake_minimum_required(VERSION 3.10)
project(MyControllerAppTests)

# Include directories
include_directories(../include)

# Include Google Test
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

# Create the test executable
add_executable(test_app test_main.cpp test_controller.cpp ../src/controller.cpp ../src/settings.cpp ../src/display.cpp)

# Link the test executable with necessary libraries
target_link_libraries(test_app ${GTEST_LIBRARIES})

# Conditionally link pthread for Unix-like systems
if(UNIX)
    target_link_libraries(test_app pthread)
endif()

# Optionally, add a test entry for CTest
add_test(NAME MyControllerTests COMMAND test_app)
