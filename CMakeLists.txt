cmake_minimum_required(VERSION 2.6)

project(tiny_heart)

set(VLIB_INC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs/vlib/src/libs)
add_subdirectory(libs/satin)

set(LIB_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/libs/satin/src/ ${CMAKE_CURRENT_SOURCE_DIR}/libs/vlib/src/libs)

set(SATIN_DATA_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs/satin/data)
set(DATA_DIR ${CMAKE_CURRENT_SOURCE_DIR}/data)
set(GAME_SRC ${CMAKE_CURRENT_SOURCE_DIR}/play_state.c ${CMAKE_CURRENT_SOURCE_DIR}/title_state.c)

add_subdirectory(frontends/win32)
