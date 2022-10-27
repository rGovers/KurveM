cmake_minimum_required (VERSION 3.0)
project (KurveM VERSION 1.0)

FILE (STRINGS "lib/bullet3/VERSION" BULLET_VERSION)

set(CMAKE_CXX_STANDARD 17)

if (CMAKE_BUILD_TYPE STREQUAL "Debug" AND NOT WIN32)
    set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address")
    set (CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fsanitize=address")
endif()

set (KURVEM_VERSION_MAJOR ${CMAKE_PROJECT_VERSION_MAJOR})
set (KURVEM_VERSION_MINOR ${CMAKE_PROJECT_VERSION_MINOR})

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
    SOURCE_FILE "shaders/editorStandardSolid.frag"
    HEADER_FILE "include/Shaders/EditorStandardSolidPixel.h"
    VARIABLE_NAME "EditorStandardSolidPixel")

FileToHeader(
    SOURCE_FILE "shaders/animatorStandard.vert"
    HEADER_FILE "include/Shaders/AnimatorStandardVertex.h"
    VARIABLE_NAME "AnimatorStandardVertex")
FileToHeader(
    SOURCE_FILE "shaders/animatorStandard.frag"
    HEADER_FILE "include/Shaders/AnimatorStandardPixel.h"
    VARIABLE_NAME "AnimatorStandardPixel")
FileToHeader(
    SOURCE_FILE "shaders/animatorStandardSolid.frag"
    HEADER_FILE "include/Shaders/AnimatorStandardSolidPixel.h"
    VARIABLE_NAME "AnimatorStandardSolidPixel")

FileToHeader(
    SOURCE_FILE "shaders/animatorSBody.vert"
    HEADER_FILE "include/Shaders/AnimatorSoftbodyVertex.h"
    VARIABLE_NAME "AnimatorSoftbodyVertex")
FileToHeader(
    SOURCE_FILE "shaders/animatorSBody.frag"
    HEADER_FILE "include/Shaders/AnimatorSoftbodyPixel.h"
    VARIABLE_NAME "AnimatorSoftbodyPixel")
FileToHeader(
    SOURCE_FILE "shaders/animatorSBodySolid.frag"
    HEADER_FILE "include/Shaders/AnimatorSoftbodySolidPixel.h"
    VARIABLE_NAME "AnimatorSoftbodySolidPixel")

FileToHeader(
    SOURCE_FILE "shaders/weightStandard.vert"
    HEADER_FILE "include/Shaders/WeightStandardVertex.h"
    VARIABLE_NAME "WeightStandardVertex")
FileToHeader(
    SOURCE_FILE "shaders/weightStandard.frag"
    HEADER_FILE "include/Shaders/WeightStandardPixel.h"
    VARIABLE_NAME "WeightStandardPixel")

FileToHeader(
    SOURCE_FILE "shaders/gizmos.vert"
    HEADER_FILE "include/Shaders/GizmosVertex.h"
    VARIABLE_NAME "GizmosVertex")
FileToHeader(
    SOURCE_FILE "shaders/gizmos.frag"
    HEADER_FILE "include/Shaders/GizmosPixel.h"
    VARIABLE_NAME "GizmosPixel")
    
FileToHeader(
    SOURCE_FILE "shaders/grid.vert"
    HEADER_FILE "include/Shaders/GridVertex.h"
    VARIABLE_NAME "GridVertex")
FileToHeader(
    SOURCE_FILE "shaders/grid.frag"
    HEADER_FILE "include/Shaders/GridPixel.h"
    VARIABLE_NAME "GridPixel")

FileToHeader(
    SOURCE_FILE "shaders/referenceImage.vert"
    HEADER_FILE "include/Shaders/ReferenceImageVertex.h"
    VARIABLE_NAME "ReferenceImageVertex")
FileToHeader(
    SOURCE_FILE "shaders/referenceImage.frag"
    HEADER_FILE "include/Shaders/ReferenceImagePixel.h"
    VARIABLE_NAME "ReferenceImagePixel")

include_directories("${PROJECT_SOURCE_DIR}")
include_directories("${PROJECT_SOURCE_DIR}/include/")
include_directories("${PROJECT_SOURCE_DIR}/lib/bullet3/src/")
include_directories("${PROJECT_SOURCE_DIR}/lib/glad/include/")
include_directories("${PROJECT_SOURCE_DIR}/lib/glfw/include/")
include_directories("${PROJECT_SOURCE_DIR}/lib/glm/")
include_directories("${PROJECT_SOURCE_DIR}/lib/imgui/")
include_directories("${PROJECT_SOURCE_DIR}/lib/imgui/backends/")
include_directories("${PROJECT_SOURCE_DIR}/lib/stb/")
include_directories("${PROJECT_SOURCE_DIR}/lib/tinyxml2")

file(GLOB SOURCES "src/*.cpp" "lib/glad/src/*.c" "lib/tinyxml2/tinyxml2.cpp" "lib/imgui/*.cpp" "lib/imgui/backends/imgui_impl_opengl3.cpp" "lib/imgui/backends/imgui_impl_glfw.cpp")

# Bullet Options
IF(NOT WIN32)
	set(BUILD_SHARED_LIBS ON)
	SET(DL ${CMAKE_DL_LIBS})
	IF(CMAKE_SYSTEM_NAME MATCHES "Linux")
		MESSAGE("Linux")
		SET(OSDEF -D_LINUX)
	ELSE(CMAKE_SYSTEM_NAME MATCHES "Linux")
		IF(APPLE)
			MESSAGE("Apple")
			SET(OSDEF -D_DARWIN)
		ELSE(APPLE)
			MESSAGE("BSD?")
			SET(OSDEF -D_BSD)
		ENDIF(APPLE)
	ENDIF(CMAKE_SYSTEM_NAME MATCHES "Linux")
ELSE(NOT WIN32)
	set(BUILD_SHARED_LIBS OFF)
ENDIF(NOT WIN32)

set(USE_DOUBLE_PRECISION OFF)
set(USE_GRAPHICAL_BENCHMARK OFF)
set(USE_SOFT_BODY_MULTI_BODY_DYNAMICS_WORLD ON)
set(BULLET2_MULTITHREADING OFF)
set(USE_MSVC_INCREMENTAL_LINKING OFF)
set(USE_CUSTOM_VECTOR_MATH OFF)
set(BUILD_BULLET3 ON)

add_subdirectory("lib/glfw")
add_subdirectory("lib/bullet3/src")

add_executable(KurveM ${SOURCES})
target_link_libraries(KurveM glfw ${GLFW_LIBRARIES} BulletInverseDynamics BulletSoftBody BulletCollision BulletDynamics LinearMath Bullet3Common Bullet3Dynamics Bullet3Collision Bullet3Geometry)