add_executable(tiny_heart WIN32 main.c ${GAME_SRC})
target_link_libraries(tiny_heart satin)
target_include_directories(tiny_heart PUBLIC ${LIB_INCLUDES})

set_property(TARGET tiny_heart PROPERTY
             MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

install(TARGETS tiny_heart DESTINATION tiny_heart_win32)
install(DIRECTORY ${SATIN_DATA_DIR} DESTINATION tiny_heart_win32)
install(DIRECTORY ${DATA_DIR} DESTINATION tiny_heart_win32)
