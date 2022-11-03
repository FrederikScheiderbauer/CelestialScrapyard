#[[https://codevion.github.io/#!cpp/cmake.md]]
cmake_minimum_required(VERSION 3.24)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(ggd VERSION 1.0)
add_library(glad STATIC ../lib/glad/glad.c)
target_include_directories(glad PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../lib/glad/include)

add_executable(${PROJECT_NAME} main.cpp ${CMAKE_CURRENT_SOURCE_DIR}/ShaderProgram/ShaderProgram.cpp)
target_link_libraries(${PROJECT_NAME} PUBLIC glad GL dl glfw)
