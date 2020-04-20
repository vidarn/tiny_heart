add_executable(ld46 WIN32 main.c ${GAME_SRC})
target_link_libraries(ld46 satin)
target_include_directories(ld46 PUBLIC ${LIB_INCLUDES})


install(TARGETS ld46 DESTINATION win32)
install(DIRECTORY ${SATIN_DATA_DIR} DESTINATION win32)
install(DIRECTORY ${DATA_DIR} DESTINATION win32)
