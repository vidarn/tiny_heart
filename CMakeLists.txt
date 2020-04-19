cmake_minimum_required(VERSION 2.6)

project(ld46)

set(VLIB_INC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs/vlib/src/libs)
add_subdirectory(libs/satin)

add_executable(ld46 WIN32 main.c play_state.c)
target_include_directories(ld46 PUBLIC libs/satin/src/ libs/vlib/src/libs)
target_link_libraries(ld46 satin)

install(TARGETS ld46 DESTINATION game)
install(DIRECTORY libs/satin/data DESTINATION game)
install(DIRECTORY data DESTINATION game)
