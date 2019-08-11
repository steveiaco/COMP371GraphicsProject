cmake_minimum_required(VERSION 3.14)
project(Project)

set(CMAKE_CXX_STANDARD 14)

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glm REQUIRED)
find_package(glfw3 3.3 REQUIRED)

add_definitions(${GLM_DEFINITIONS})

set(SOURCE_FILES)

file(GLOB SOURCE_FILES Source/*.cpp Source/*.h Source/*/*/*.cpp Source/*/*/*.h)
list(REMOVE_ITEM SOURCE_FILES "Source/Terrain.cpp" "Source/Terrain.h")

add_executable(${PROJECT_NAME} ${SOURCE_FILES})
include_directories(${PROJECT_NAME} ${OPENGL_INCLUDE_DIRS} ${GLFW_INCLUDE_DIRS} ${GLM_INCLUDE_DIRS})
target_include_directories(${PROJECT_NAME} PUBLIC ${OPENGL_INCLUDE_DIR} ${GLFW_INCLUDE_DIR} ${GLM_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARIES} GLEW::GLEW ${GLFW_LIBRARY} glfw ${GLM_LIBRARY_DIRS} -lfreeimage)
