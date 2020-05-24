cmake_minimum_required(VERSION 3.0)

project(kursach)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(kursach kursach.cpp)
target_link_libraries(kursach ${OpenCV_LIBS})

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/markdown.txt DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/Images/img_1.png DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/Images/img_2.png DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/Images/img_3.png DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/Images/img_4.png DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
