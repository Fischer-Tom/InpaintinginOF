cmake_minimum_required(VERSION 3.16)
project(DiffusioninOF)

set(CMAKE_CXX_STANDARD 14)
add_executable(${PROJECT_NAME} main.cpp)

find_package(PNG REQUIRED)
include_directories(${PNG_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME}  ${PNG_LIBRARY})
