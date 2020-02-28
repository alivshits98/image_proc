cmake_minimum_required(VERSION 3.0)

project(lab_1_1)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(lab_1_1 lab_1_1.cpp)
target_link_libraries(lab_1_1 ${OpenCV_LIBS})