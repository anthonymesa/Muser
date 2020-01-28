cmake_minimum_required(VERSION 3.16.0)

# Set application name
#
set(APPLICATION_NAME "Muser")

project(${APPLICATION_NAME})

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -stdlib=libc++")
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++latest /D_USE_MATH_DEFINES ")
endif()

# Set directory variables
#
set(RESOURCE_DIRECTORY ${CMAKE_SOURCE_DIR}/res)

# Include libraries and source headers
#
include_directories(${CMAKE_SOURCE_DIR}/lib/include)
include_directories(${CMAKE_SOURCE_DIR}/src/header)

file(GLOB_RECURSE SRC_FILES "${CMAKE_SOURCE_DIR}/src/*.cpp" "${CMAKE_SOURCE_DIR}/src/*.c")
file(GLOB_RECURSE PROJECT_H_FILES "${CMAKE_SOURCE_DIR}/src/*.hpp" "${CMAKE_SOURCE_DIR}/src/*.h")
file(GLOB_RECURSE LIBRARY_H_FILES "${CMAKE_SOURCE_DIR}/lib/*.h")
file(GLOB_RECURSE RESOURCE_FILES "${RESOURCE_DIRECTORY}/*")

message(STATUS "==========================================\n")
message(STATUS "Source Files === ${SRC_FILES}\n")
message(STATUS "Project Headers === ${PROJECT_H_FILES}\n")
message(STATUS "Library Headers === ${LIBRARY_H_FILES}\n")

#
# MacOs: Set up resource list for MacOs bundle.
# Windows: Copies all of the resources from the res folder to the new build folder.
#
macro(copy_resources)
    if(WIN32)
        message(STATUS "==========================================================")
        message(STATUS "WIN COPY RESOURCES MACRO > PRINT RESOURCES----------------{")
        message(STATUS "==========================================================\n")
        message(STATUS "Resource Directory === ${RESOURCE_DIRECTORY}\n")
        message(STATUS "Resource Files === ${RESOURCE_FILES}\n")
        message(STATUS "==========================================================")
        message(STATUS "ITERATE RESOURCES-----------------------------------------")
        message(STATUS "==========================================================\n")
        foreach(ITEM ${RESOURCE_FILES})
            message(STATUS "Item Directory ==== ${ITEM}")
            string(LENGTH ${RESOURCE_DIRECTORY} ITEM_POSITION)
            string(LENGTH ${ITEM} ITEM_LENGTH)
            string(SUBSTRING ${ITEM} ${ITEM_POSITION} -1 TRIMMED_ITEM)
            message(STATUS "Trimmed Item === ${TRIMMED_ITEM}")
            add_custom_command(
                TARGET ${APPLICATION_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                        ${ITEM}
                        ${EXECUTABLE_OUTPUT_PATH}/Release/${TRIMMED_ITEM}
            )
            message(STATUS "Copied from - ${ITEM}")
            message(STATUS "Copied to - ${EXECUTABLE_OUTPUT_PATH}/Release/${TRIMMED_ITEM}\n")
        endforeach()
        message(STATUS "==========================================================")
        message(STATUS "END MACRO-------------------------------------------------}")
        message(STATUS "==========================================================\n")
    endif()
    if(UNIX)
        message(STATUS "==========================================================")
        message(STATUS "MAC COPY RESOURCES MACRO > PRINT RESOURCES----------------{")
        message(STATUS "==========================================================\n")
        message(STATUS "Resource Directory === ${RESOURCE_DIRECTORY}\n")
        message(STATUS "Resource Files === ${RESOURCE_FILES}\n")

        # Get the index for the last item in RESOURCE_FILES, no matter the length
        list(LENGTH RESOURCE_FILES LAST_INDEX)
        message(STATUS "\n${LAST_INDEX}\n")
        math(EXPR LAST_RESOURCE "${LAST_INDEX} - 1")
        list(GET RESOURCE_FILES ${LAST_RESOURCE} LAST_ITEM)

        foreach(ITEM ${RESOURCE_FILES})
            message(STATUS "Item Directory ==== ${ITEM}")
            string(LENGTH ${RESOURCE_DIRECTORY} ITEM_POSITION)
            math(EXPR ITEM_POSITION_INDEX "${ITEM_POSITION} + 1")
            string(LENGTH ${ITEM} ITEM_LENGTH)
            string(SUBSTRING ${ITEM} ${ITEM_POSITION} -1 TRIMMED_ITEM)
            message(STATUS "Trimmed Item === ${TRIMMED_ITEM}\n")

            if(NOT ${ITEM} MATCHES ${LAST_ITEM})
                string(APPEND LOCAL_RESOURCES "res${TRIMMED_ITEM};")
            else()
                string(APPEND LOCAL_RESOURCES "res${TRIMMED_ITEM}")
            endif()
        endforeach()

        message(STATUS "Local Resources === ${LOCAL_RESOURCES}")

        message(STATUS "==========================================================")
        message(STATUS "END MACRO-------------------------------------------------}")
        message(STATUS "==========================================================\n")
    endif()
endmacro()

if(WIN32)

    set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin/win)
    set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/build/win)
    set_source_files_properties(${SRC_FILES} PROPERTIES COMPILE_FLAGS /Y-)
    add_executable(${APPLICATION_NAME} ${SRC_FILES} ${LIBRARY_H_FILES} ${PROJECT_H_FILES})

    find_package(OpenGL)
    target_link_libraries(${APPLICATION_NAME} ${OpenGL_LIBRARY})
    target_link_libraries(${APPLICATION_NAME} ${CMAKE_SOURCE_DIR}/lib/GLFW/Win64/lib-vc2019/glfw3.lib)

    copy_resources()

endif()
if(UNIX)
	
    set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin/mac)
    set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/build/mac)

    set(LOCAL_RESOURCES "")

    copy_resources()
    
    foreach(ITEM ${LOCAL_RESOURCES})
        string(FIND ${ITEM} "/data/" FIND_DATA)        
        if(NOT ${FIND_DATA} MATCHES "-1")
            set_property(
                SOURCE ${ITEM}
                PROPERTY MACOSX_PACKAGE_LOCATION "Resources/data")
        endif()
        string(FIND ${ITEM} "/media/" FIND_MEDIA)
        if(NOT ${FIND_MEDIA} MATCHES "-1")
            set_property(
                SOURCE ${ITEM}
                PROPERTY MACOSX_PACKAGE_LOCATION "Resources/media")
        endif()
        string(FIND ${ITEM} "/temp/" FIND_MEDIA)
        if(NOT ${FIND_MEDIA} MATCHES "-1")
            set_property(
                SOURCE ${ITEM}
                PROPERTY MACOSX_PACKAGE_LOCATION "Resources/temp")
        endif()
    endforeach()

    add_executable(${APPLICATION_NAME} ${SRC_FILES} ${LIBRARY_H_FILES} ${PROJECT_H_FILES} ${LOCAL_RESOURCES})

    find_library(CORE_FOUNDATION CoreFoundation)
    find_library(CORE_GRAPHICS CoreGraphics)
    find_library(CORE_VIDEO CoreVideo)
    find_Library(IOKIT IOKit)
    find_Library(APP_KIT AppKit)
    find_library(OPENGL OpenGl)

    target_link_libraries(${APPLICATION_NAME} ${CORE_FOUNDATION} ${CORE_GRAPHICS} ${CORE_VIDEO} ${IOKIT} ${APP_KIT} ${OPENGL})
    target_link_libraries(${APPLICATION_NAME} ${CMAKE_SOURCE_DIR}/lib/GL/Mac/glew32s.lib)
    target_link_libraries(${APPLICATION_NAME} ${CMAKE_SOURCE_DIR}/lib/GLFW/Mac/lib-macos/libglfw3.a)

    set_target_properties(${APPLICATION_NAME} PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_FRAMEWORK_IDENTIFIER com.bisektor.${APPLICATION_NAME}
        #RESOURCE "${RESOURCE_FILES}"
    )

endif()

#   Macro that prints all of the variabls of the current CMake run.
#
# macro(print_all_variables)
#     message(STATUS "print_all_variables------------------------------------------{")
#     get_cmake_property(_variableNames VARIABLES)
#     foreach (_variableName ${_variableNames})
#         message(STATUS "${_variableName}=${${_variableName}}")
#     endforeach()
#     message(STATUS "print_all_variables------------------------------------------}")
# endmacro()

# print_all_variables()