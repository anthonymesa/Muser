cmake_minimum_required(VERSION 3.16.0)
project(Muser)

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/include)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src/header)

if(WIN32)

    set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin/win)
    set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/build/win)

    add_executable(Muser ${CMAKE_SOURCE_DIR}/src/application.cpp)

    target_link_libraries(Muser ${CMAKE_SOURCE_DIR}/lib/GL/glew32s.lib)
    target_link_libraries(Muser ${CMAKE_SOURCE_DIR}/lib/GLFW/glfw-3.3.bin.WIN64/lib-vc2019/glfw3.lib)
endif()

if(UNIX)
	
    set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin/Mac)
    set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/build/mac)

    add_executable(Muser ${CMAKE_SOURCE_DIR}/src/application.cpp)

    target_link_libraries(Muser ${CMAKE_SOURCE_DIR}/lib/GL/glew32s.lib)
    target_link_libraries(Muser ${CMAKE_SOURCE_DIR}/lib/GLFW/glfw-3.3.bin.MACOS/lib-macos/libglfw3.a)
endif()