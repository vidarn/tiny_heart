find_package(OpenGL REQUIRED)

set(VLIB_SRC 
	${VLIB_INC_DIR}/sort/sort.c
	${VLIB_INC_DIR}/memory/buffer.c
	${VLIB_INC_DIR}/linalg/linalg.c
)
include_directories(${VLIB_INC_DIR})

set(SATIN_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../libs/satin/src)
set(SATIN_OS_SRC
    ${SATIN_SRC_DIR}/os/emscripten/os.c
    ${SATIN_SRC_DIR}/opengl/emscripten/opengl.c
)
set(SATIN_OS_LIBS
)
set(SATIN_SRC 
	${SATIN_SRC_DIR}/engine.c
	${SATIN_SRC_DIR}/gui/gui.c
	${SATIN_OS_SRC}
)
include_directories(${SATIN_SRC_DIR})
include_directories(${SATIN_SRC_DIR}/opengl)
#add_executable(tiny_heart WIN32 main.c ${GAME_SRC})

message( "blaj")
message( "${GAME_SRC}")
message( "${SATIN_SRC}")


add_executable(tiny_heart WIN32 main.c ${GAME_SRC} ${SATIN_SRC} ${VLIB_SRC})
#target_link_libraries(tiny_heart satin)
target_include_directories(tiny_heart PUBLIC ${LIB_INCLUDES})

set_property(TARGET tiny_heart PROPERTY
             MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

         #set_target_properties(tiny_heart PROPERTIES SUFFIX ".html")
set_target_properties(tiny_heart PROPERTIES LINK_FLAGS
    "-s WASM=0 -s USE_WEBGL2=1 -s ALLOW_MEMORY_GROWTH=1 --preload-file ${DATA_DIR}@/ --preload-file ${SATIN_DATA_DIR}@/")
#"-s WASM=0 -s ALLOW_MEMORY_GROWTH=1 --preload-file ${SATIN_DATA_DIR}@/")

install(TARGETS tiny_heart DESTINATION tiny_heart_web)
#install(FILES ${CMAKE_CURRENT_BINARY_DIR}/tiny_heart.js DESTINATION tiny_heart_web/)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/tiny_heart.data DESTINATION tiny_heart_web/)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/tiny_heart.js.mem DESTINATION tiny_heart_web/)
install(FILES index.html DESTINATION tiny_heart_web/)
