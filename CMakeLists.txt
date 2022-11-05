cmake_minimum_required (VERSION 3.24)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#give Project a better name
project(ggd VERSION 1.0)

#add&link libraries
add_library(glad STATIC ${CMAKE_SOURCE_DIR}/lib/glad/glad.c)
target_include_directories(glad PUBLIC ${CMAKE_SOURCE_DIR}/Include)


add_executable(${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/src/main.cpp)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/Include)

if (WIN32)
    target_link_libraries(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/Lib/GLFW/glfw3.lib glad)
endif (WIN32)

if(UNIX)
    target_link_libraries(${PROJECT_NAME} PUBLIC glfw glad)
endif (UNIX)
