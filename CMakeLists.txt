cmake_minimum_required(VERSION 3.5)
enable_language(CXX)

project(sampleint)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_BUILD_TYPE RELEASE)
set(HEADER_FILES include/config_parser.h include/functon.h include/integration.h)
set(CMAKE_CXX_FLAGS "-msse4 -msse3 -march=native -O3")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=thread -fPIE -pie -g")
find_package(Threads REQUIRED)
add_executable(sampleint integr_main.cpp src/config_parser.cpp src/function.cpp src/integration.cpp)
target_link_libraries(sampleint Threads::Threads)

