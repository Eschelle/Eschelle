file(GLOB_RECURSE HEADERS *.h)
file(GLOB_RECURSE SOURCES *.cc)
list(REMOVE_ITEM SOURCES main.cc)

add_library("lib${PROJECT_NAME}" ${HEADERS} ${SOURCES})

add_executable(${PROJECT_NAME} main.cc)
target_link_libraries(${PROJECT_NAME} "lib${PROJECT_NAME}")
install(TARGETS ${PROJECT_NAME} DESTINATION bin)
