cmake_minimum_required(VERSION 4.2)
project(HoverX VERSION 1.0 LANGUAGES CXX)

set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(SRC_FILES
    ${SRC_DIR}/main.cpp
    ${SRC_DIR}/poly2d.cpp
    ${SRC_DIR}/client.cpp
    ${SRC_DIR}/entity.cpp
    ${SRC_DIR}/glex.cpp
    ${SRC_DIR}/level.cpp
    ${SRC_DIR}/line2d.cpp
    ${SRC_DIR}/obj.cpp
    ${SRC_DIR}/renderer.cpp
    ${SRC_DIR}/server.cpp
    ${SRC_DIR}/font.cpp
    ${SRC_DIR}/sdl.cpp
    ${SRC_DIR}/tinerxml.cpp
    ${SRC_DIR}/ui.cpp
)
add_executable(HoverX ${SRC_FILES})

find_package(OpenGL REQUIRED)
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
# GLEW: prefer system headers/libs by locating them directly here
find_path(GLEW_INCLUDE_DIR NAMES GL/glew.h PATHS /usr/include /usr/local/include PATH_SUFFIXES "")
find_library(GLEW_LIBRARY NAMES GLEW glew libGLEW PATHS /usr/lib /usr/lib/x86_64-linux-gnu /usr/local/lib)

if(GLEW_INCLUDE_DIR AND GLEW_LIBRARY)
	set(GLEW_FOUND TRUE)
	set(GLEW_INCLUDE_DIRS ${GLEW_INCLUDE_DIR})
	set(GLEW_LIBRARIES ${GLEW_LIBRARY})
else()
	message(FATAL_ERROR "GLEW not found. Install libglew-dev or set GLEW_INCLUDE_DIR/GLEW_LIBRARY variables.")
endif()

# Link SDL2 (prefer imported target when available)
if(TARGET SDL2::SDL2)
	target_link_libraries(HoverX PRIVATE SDL2::SDL2)
else()
	target_include_directories(HoverX PRIVATE ${SDL2_INCLUDE_DIRS})
	target_link_libraries(HoverX PRIVATE ${SDL2_LIBRARIES})
endif()

# Link OpenGL (support several variable names / imported target)
if(TARGET OpenGL::GL)
	target_link_libraries(HoverX PRIVATE OpenGL::GL)
elseif(DEFINED OPENGL_gl_LIBRARY)
	target_link_libraries(HoverX PRIVATE ${OPENGL_gl_LIBRARY})
elseif(DEFINED OPENGL_LIBRARIES)
	target_link_libraries(HoverX PRIVATE ${OPENGL_LIBRARIES})
endif()

# Link LibXml2 (prefer imported target)
## tinyxml (project uses TinyXML/TinyXML.h -- not libxml2)
find_path(TINYXML_INCLUDE_DIR NAMES tinyxml.h)
find_library(TINYXML_LIB NAMES tinyxml)
if(TINYXML_LIB AND TINYXML_INCLUDE_DIR)
	target_include_directories(HoverX PRIVATE ${TINYXML_INCLUDE_DIR})
	target_link_libraries(HoverX PRIVATE ${TINYXML_LIB})
endif()

# ENet (networking)
find_library(ENET_LIB NAMES enet)
if(ENET_LIB)
	target_link_libraries(HoverX PRIVATE ${ENET_LIB})
endif()

# SDL2_image (image loading for IMG_Load)
if(TARGET SDL2_image::SDL2_image)
    target_link_libraries(HoverX PRIVATE SDL2_image::SDL2_image)
elseif(SDL2_image_FOUND)
    if(DEFINED SDL2_IMAGE_LIBRARIES)
        target_link_libraries(HoverX PRIVATE ${SDL2_IMAGE_LIBRARIES})
    endif()
endif()

# GLEW (OpenGL extensions)
if(TARGET GLEW::GLEW)
	target_link_libraries(HoverX PRIVATE GLEW::GLEW)
elseif(GLEW_FOUND)
	if(DEFINED GLEW_LIBRARIES)
		target_link_libraries(HoverX PRIVATE ${GLEW_LIBRARIES})
	endif()
endif()

# GLU (utility library providing gluPerspective, gluLookAt)
find_library(GLU_LIB NAMES GLU GLU32 glu)
if(GLU_LIB)
	target_link_libraries(HoverX PRIVATE ${GLU_LIB})
endif()

# Project include directory for headers in this folder
target_include_directories(HoverX PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Require C++11
target_compile_features(HoverX PRIVATE cxx_std_11)
