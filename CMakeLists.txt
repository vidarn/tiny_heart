cmake_minimum_required(VERSION 2.6)

project(tiny_heart)

#NOTE(Vidar): Replace /MD with /MT, for static linking to the crt
#Taken from https://stackoverflow.com/questions/14172856/compile-with-mt-instead-of-md-using-cmake
set(CompilerFlags
        CMAKE_CXX_FLAGS
        CMAKE_CXX_FLAGS_DEBUG
        CMAKE_CXX_FLAGS_RELEASE
        CMAKE_CXX_FLAGS_MINSIZEREL
        CMAKE_CXX_FLAGS_RELWITHDEBINFO
        CMAKE_C_FLAGS
        CMAKE_C_FLAGS_DEBUG
        CMAKE_C_FLAGS_RELEASE
        CMAKE_C_FLAGS_MINSIZEREL
        CMAKE_C_FLAGS_RELWITHDEBINFO
        )
foreach(CompilerFlag ${CompilerFlags})
    string(REPLACE "/MD" "/MT" ${CompilerFlag} "${${CompilerFlag}}")
    set(${CompilerFlag} "${${CompilerFlag}}" CACHE STRING "msvc compiler flags" FORCE)
    message("MSVC flags: ${CompilerFlag}:${${CompilerFlag}}")
endforeach()

set(VLIB_INC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs/vlib/src/libs)
add_subdirectory(libs/satin)

set(LIB_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/libs/satin/src/ ${CMAKE_CURRENT_SOURCE_DIR}/libs/vlib/src/libs)

set(SATIN_DATA_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs/satin/data)
set(DATA_DIR ${CMAKE_CURRENT_SOURCE_DIR}/data)
set(GAME_SRC ${CMAKE_CURRENT_SOURCE_DIR}/play_state.c ${CMAKE_CURRENT_SOURCE_DIR}/title_state.c)

#add_subdirectory(frontends/win32)
add_subdirectory(frontends/web)
