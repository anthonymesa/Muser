#                    `.://////:.`                   
#               ./oohNy+-.``.-+yNhoo/.              
#           `/so/.`yo`          `oy`./os/`          
#        `/sooo` `d.   /hNMMdh/   .d`  /hos/`       
#      -ss-   .+/y/   yMMMM- `my   /y`+.   -ss-     
#     :d`  `-s   d`  `MMMMMdoyMM`  .d    /+` `d:    
#      -sss+-  `/y/   yMMMMMMMMy   /y`.   -yss-     
#        `+so. s/`d.   /hNMMNh/   .d` +o-os/`       
#           `/sh/.`y+`          `+y`./os/`          
#               ./oohNy+-.``.-+yNhoo/.              
#                    `.://++//:.`                                                          
#    __ __                            ___   _       
#   / // /  __ __   ___  ___   ____  / _/  (_) __ __
#  / _  /  / // /  / _ \/ -_) / __/ / _/  / /  \ \ /
# /_//_/   \_, /  / .__/\__/ /_/   /_/   /_/  /_\_\ 
#         /___/  /_/                                
# 
#====================================================
#	File: C++ Makefile
#	Author: admesa@pm.me
#	Date: 02-March-21
#====================================================	

cmake_minimum_required(VERSION 3.16.0)

# App info.

set(APPLICATION_NAME "Muser")
project(${APPLICATION_NAME})

# CHOOSE YOUR COMPILER.

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
	#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -stdlib=libc++")

    # fPIC flag and -shared keep relocation and undefined reference errors
    # at bay on linux.
    #
    # Had an issue in Mantis, and these compile variables fixed it
    #  -fPIC -shared
    # But I also fixed it by way of code in Mantis.hpp by initializing
    # the mantis log directory string variable

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++latest /D_USE_MATH_DEFINES ")
endif()

# Set directory variables.

set(RESOURCE_DIRECTORY ${CMAKE_SOURCE_DIR}/res)

# Include libraries and source headers.

include_directories(${CMAKE_SOURCE_DIR}/lib/include)
include_directories(${CMAKE_SOURCE_DIR}/src/header)

# Collect source files.

file(GLOB_RECURSE SRC_FILES "${CMAKE_SOURCE_DIR}/src/*.cpp" "${CMAKE_SOURCE_DIR}/src/*.c")
file(GLOB_RECURSE PROJECT_H_FILES "${CMAKE_SOURCE_DIR}/src/*.hpp" "${CMAKE_SOURCE_DIR}/src/*.h")
file(GLOB_RECURSE LIBRARY_H_FILES "${CMAKE_SOURCE_DIR}/lib/*.h")
file(GLOB_RECURSE RESOURCE_FILES "${RESOURCE_DIRECTORY}/*")

# Print globbed files for checking.

message(STATUS "==========================================\n")
message(STATUS "Source Files === ${SRC_FILES}\n")
message(STATUS "Project Headers === ${PROJECT_H_FILES}\n")
message(STATUS "Library Headers === ${LIBRARY_H_FILES}\n")

# This will only work on Windows and Linux.

macro(copy_resources)
    if(WIN32 OR UNIX AND NOT APPLE)

        # Print info to console.

        message(STATUS "==========================================================")
        message(STATUS "WIN COPY RESOURCES MACRO > PRINT RESOURCES----------------{")
        message(STATUS "==========================================================\n")
        message(STATUS "Resource Directory === ${RESOURCE_DIRECTORY}\n")
        message(STATUS "Resource Files === ${RESOURCE_FILES}\n")
        message(STATUS "==========================================================")
        message(STATUS "ITERATE RESOURCES-----------------------------------------")
        message(STATUS "==========================================================\n")

        # Iterate through each resource in the /res directory.

        foreach(ITEM ${RESOURCE_FILES})

            # Print the full file path for the resource.

            message(STATUS "Item Directory ==== ${ITEM}")
            
            # Get length of full path of /res and current resource.
            
            string(LENGTH ${RESOURCE_DIRECTORY} ITEM_POSITION)
            string(LENGTH ${ITEM} ITEM_LENGTH)

            # Create a new substring that is just the file name.
            # When the substring length is -1, it just outputs the rest
            # of the string from the begin position (ITEM_POSITION).

            string(SUBSTRING ${ITEM} ${ITEM_POSITION} -1 TRIMMED_ITEM)

            message(STATUS "Trimmed Item === ${TRIMMED_ITEM}")

            # Add a custom command that will run after build is complete.
            # This macro is run below after EXECUTABLE_OUTPUT_PATH is set.

            add_custom_command(

                TARGET ${APPLICATION_NAME} POST_BUILD

                # Copy the item from its location in the full path
                # to the path in the binary build directory
                # CMAKE_COMMAND is the full path to the CMake executable
                # cmake(1) which is useful from custom commands that want
                # to use the cmake -E option for portable system commands. 

                # Files are being copied from /res to /build at the same
                # level as the program output

                COMMAND ${CMAKE_COMMAND} -E copy
                        ${ITEM}
                        ${EXECUTABLE_OUTPUT_PATH}${TRIMMED_ITEM}
            )
            
            message(STATUS "Copied from - ${ITEM}")
            message(STATUS "Copied to - ${EXECUTABLE_OUTPUT_PATH}${TRIMMED_ITEM}\n")
        
        endforeach()
        
        message(STATUS "==========================================================")
        message(STATUS "END MACRO-------------------------------------------------}")
        message(STATUS "==========================================================\n")
    endif()
endmacro()

if(WIN32)

    set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
    set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/build)
    set_source_files_properties(${SRC_FILES} PROPERTIES COMPILE_FLAGS /Y-)
    add_executable(${APPLICATION_NAME} ${SRC_FILES} ${LIBRARY_H_FILES} ${PROJECT_H_FILES})

    #find_package(OpenGL)
    #target_link_libraries(${APPLICATION_NAME} ${OpenGL_LIBRARY})
    #target_link_libraries(${APPLICATION_NAME} ${CMAKE_SOURCE_DIR}/lib/GLFW/Win64/lib-vc2019/glfw3.lib)

    copy_resources()

endif()

if(UNIX AND NOT APPLE)

    set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
    set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/build)
    add_executable(${APPLICATION_NAME} ${SRC_FILES} ${LIBRARY_H_FILES} ${PROJECT_H_FILES})

    set(CPACK_PROJECT_NAME ${PROJECT_NAME})
    set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})

    #find_package(OpenGL)
    #target_link_libraries(${APPLICATION_NAME} ${OpenGL_LIBRARY})
    #target_link_libraries(${APPLICATION_NAME} ${CMAKE_SOURCE_DIR}/lib/GLFW/Win64/lib-vc2019/glfw3.lib)

    copy_resources()

endif()