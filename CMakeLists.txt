cmake_minimum_required(VERSION 3.6)
project(Eschelle)

find_package(Threads)

set(CMAKE_CXX_STANDARD 11)

file(GLOB_RECURSE HEADERS Sources/*.h)
file(GLOB_RECURSE SOURCES Sources/*.cc)

add_executable(Eschelle ${HEADERS} ${SOURCES})
target_link_libraries(Eschelle ${CMAKE_THREAD_LIBS_INIT})