find_package(Threads REQUIRED)

include(ExternalProject)
ExternalProject_Add(
  gtest
  URL "https://github.com/google/googletest/archive/master.zip"
  PREFIX "${CMAKE_CURRENT_BINARY_DIR}/gtest"
  INSTALL_COMMAND ""
)
ExternalProject_Get_Property(gtest source_dir binary_dir)

add_library(libgtest IMPORTED STATIC 
