cmake_minimum_required(VERSION 3.0)

project(lab_3)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(lab_3 lab_3.cpp)
target_link_libraries(lab_3 ${OpenCV_LIBS})