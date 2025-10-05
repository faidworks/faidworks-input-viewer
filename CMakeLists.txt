cmake_minimum_required(VERSION 3.10)
project(MyControllerApp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add the main application
add_subdirectory(src)

# Add the testing framework
enable_testing()
add_subdirectory(tests)
