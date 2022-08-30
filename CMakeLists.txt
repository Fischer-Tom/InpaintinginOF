cmake_minimum_required(VERSION 3.16)
project(DiffusioninOF)

set(CMAKE_CXX_STANDARD 14)
add_executable(${PROJECT_NAME} main.cpp)

find_package(PNG REQUIRED)
include_directories(${PNG_INCLUDE_DIR})
add_subdirectory(Image)
target_include_directories(${PROJECT_NAME} PUBLIC Image)
target_link_directories(${PROJECT_NAME} PRIVATE Image/)
target_link_libraries(${PROJECT_NAME} Image ${PNG_LIBRARY})
