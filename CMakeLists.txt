cmake_minimum_required(VERSION 3.16.0)
project(Muser)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -stdlib=libc++")

set(SOURCE_DIRECTORY ${CMAKE_SOURCE_DIR}/src)
set(LIB_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(LIB_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)

INCLUDE_DIRECTORIES(${LIB_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${SOURCE_DIRECTORY}/header)

if(WIN32)

    set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin/win)
    set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/build/win)

    file(GLOB_RECURSE SRC_FILES ${SOURCE_DIRECTORY}/*.cpp)
    file(GLOB_RECURSE PROJECT_H_FILES ${SOURCE_DIRECTORY}/*.h)
    file(GLOB_RECURSE LIBRARY_H_FILES ${LIB_INCLUDE_DIR}/*.h)
    file(GLOB_RECURSE RESOURCE_FILES ${SOURCE_DIRECTORY}/res/*)

    add_executable(Muser ${SRC_FILES} ${LIBRARY_H_FILES} ${PROJECT_H_FILES} ${RESOURCE_FILES})

    target_link_libraries(Muser ${CMAKE_SOURCE_DIR}/lib/GL/glew32s.lib)
    target_link_libraries(Muser ${CMAKE_SOURCE_DIR}/lib/GLFW/glfw-3.3.bin.WIN64/lib-vc2019/glfw3.lib)
endif()

if(UNIX)
	
    set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin/Mac)
    set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/build/mac)

    file(GLOB_RECURSE SRC_FILES ${SOURCE_DIRECTORY}/*.cpp)
    file(GLOB_RECURSE PROJECT_H_FILES ${SOURCE_DIRECTORY}/*.h)
    file(GLOB_RECURSE LIBRARY_H_FILES ${LIB_INCLUDE_DIR}/*.h)
    file(GLOB_RECURSE RESOURCE_FILES ${SOURCE_DIRECTORY}/res/*)

    add_executable(Muser ${SRC_FILES} ${LIBRARY_H_FILES} ${PROJECT_H_FILES} ${RESOURCE_FILES})

    find_library(CORE_FOUNDATION CoreFoundation)
    find_library(CORE_GRAPHICS CoreGraphics)
    find_library(CORE_VIDEO CoreVideo)
    find_Library(IOKIT IOKit)
    find_Library(APP_KIT AppKit)
    find_library(OPENGL OpenGl)

    target_link_libraries(Muser ${CORE_FOUNDATION} ${CORE_GRAPHICS} ${CORE_VIDEO} ${IOKIT} ${APP_KIT} ${OPENGL})
    target_link_libraries(Muser ${LIB_DIRECTORY}/GL/glew32s.lib)
    target_link_libraries(Muser ${LIB_DIRECTORY}/GLFW/glfw-3.3.bin.MACOS/lib-macos/libglfw3.a)
endif()
