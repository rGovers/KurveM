cmake_minimum_required (VERSION 2.9)
project (KurveM)

set(CMAKE_CXX_STANDARD 17)

set (KURVEM_VERSION_MAJOR 0)
set (KURVEM_VERSION_MINOR 1)

add_compile_definitions(IMGUI_IMPL_OPENGL_LOADER_GLAD)

configure_file (
    "${PROJECT_SOURCE_DIR}/KurveMConfig.h.in"
    "${PROJECT_SOURCE_DIR}/include/KurveMConfig.h"
)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/modules")

include("FileToHeader")

FileToHeader(
    SOURCE_FILE "shaders/editorStandard.vert"
    HEADER_FILE "include/Shaders/EditorStandardVertex.h"
    VARIABLE_NAME "EditorStandardVertex")
FileToHeader(
    SOURCE_FILE "shaders/editorStandard.frag"
    HEADER_FILE "include/Shaders/EditorStandardPixel.h"
    VARIABLE_NAME "EditorStandardPixel")

FileToHeader(
    SOURCE_FILE "shaders/grid.vert"
    HEADER_FILE "include/Shaders/GridVertex.h"
    VARIABLE_NAME "GridVertex")
FileToHeader(
    SOURCE_FILE "shaders/grid.frag"
    HEADER_FILE "include/Shaders/GridPixel.h"
    VARIABLE_NAME "GridPixel")

FileToHeader(
    SOURCE_FILE "shaders/gizmos.vert"
    HEADER_FILE "include/Shaders/GizmosVertex.h"
    VARIABLE_NAME "GizmosVertex")
FileToHeader(
    SOURCE_FILE "shaders/gizmos.frag"
    HEADER_FILE "include/Shaders/GizmosPixel.h"
    VARIABLE_NAME "GizmosPixel")
    
include_directories("${PROJECT_SOURCE_DIR}")
include_directories("${PROJECT_SOURCE_DIR}/include/")
include_directories("${PROJECT_SOURCE_DIR}/lib/glad/include/")
include_directories("${PROJECT_SOURCE_DIR}/lib/glfw/include/")
include_directories("${PROJECT_SOURCE_DIR}/lib/glm/")
include_directories("${PROJECT_SOURCE_DIR}/lib/imgui/")
include_directories("${PROJECT_SOURCE_DIR}/lib/imgui/backends/")
include_directories("${PROJECT_SOURCE_DIR}/lib/tinyxml2")

file(GLOB SOURCES "src/*.cpp" "lib/glad/src/*.c" "lib/tinyxml2/tinyxml2.cpp" "lib/imgui/*.cpp" "lib/imgui/backends/imgui_impl_opengl3.cpp" "lib/imgui/backends/imgui_impl_glfw.cpp")

add_subdirectory("lib/glfw")

add_executable(KurveM ${SOURCES})
target_link_libraries(KurveM glfw ${GLFW_LIBRARIES})