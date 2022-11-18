cmake_minimum_required (VERSION 3.24)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#give Project a better name
project(ggd VERSION 1.0)

set(IMGUI_SOURCE_FILES
  src/imgui/imgui.cpp
  src/imgui/imgui_draw.cpp
  src/imgui/imgui_impl_glfw.cpp
  src/imgui/imgui_impl_opengl3.cpp
  src/imgui/imgui_tables.cpp
  src/imgui/imgui_widgets.cpp
  )

set(GGD_SOURCE_FILES 
  ${CMAKE_SOURCE_DIR}/src/main.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/ShaderProgram/ShaderProgram.cpp 
  ${CMAKE_SOURCE_DIR}/src/Sphere.cpp
  ${CMAKE_SOURCE_DIR}/src/CubeFace.cpp
  ${CMAKE_SOURCE_DIR}/src/gui.cpp
  ${CMAKE_SOURCE_DIR}/src/camera.cpp
)
set(Project_SOURCE_DIR ${CMAKE_SOURCE_DIR})
configure_file(${CMAKE_SOURCE_DIR}/Include/config/config.h.in ${CMAKE_SOURCE_DIR}/Include/config/config.h)
#add&link libraries
add_library(imgui STATIC ${IMGUI_SOURCE_FILES})
#set_target_properties(imgui PROPERTIES COMPILE_FLAGS " ")

target_include_directories(imgui PUBLIC ${CMAKE_SOURCE_DIR}/Include/imgui)
target_include_directories(imgui PUBLIC ${CMAKE_SOURCE_DIR}/Include/)

add_library(glad STATIC ${CMAKE_SOURCE_DIR}/lib/glad/glad.c)
target_include_directories(glad PUBLIC ${CMAKE_SOURCE_DIR}/Include)


add_executable(${PROJECT_NAME} ${GGD_SOURCE_FILES})
#set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS " ")
target_link_libraries(${PROJECT_NAME} PUBLIC ${imgui})

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/Include)

if (WIN32)
    target_link_libraries(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/Lib/GLFW/glfw3.lib glad imgui)
endif (WIN32)

if(UNIX)
    target_link_libraries(${PROJECT_NAME} PUBLIC glfw glad imgui)
endif (UNIX)
