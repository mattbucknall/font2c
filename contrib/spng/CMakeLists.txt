add_library(spng STATIC "${CMAKE_CURRENT_SOURCE_DIR}/spng/spng.c")
target_link_libraries(spng PUBLIC ${ZLIB_LIBRARIES})
target_include_directories(spng PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/spng" ${ZLIB_INCLUDE_DIRS})
