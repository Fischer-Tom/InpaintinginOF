cmake_minimum_required(VERSION 3.16)
project(DiffusioninOF)

set(CMAKE_CXX_STANDARD 14)
add_executable(${PROJECT_NAME} main.cpp Flow/Flow.cpp Image/Derivative.cpp Diffusion/Homogeneous.cpp)

find_package(PNG REQUIRED)
include_directories(${PNG_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME}  ${PNG_LIBRARY})

set(IMAGE_HEADER Image/Image.h)
include_directories(${PROJECT_NAME} Image)

set(FLOW_HEADER Flow/Flow.h)
include_directories(${PROJECT_NAME} Flow)
