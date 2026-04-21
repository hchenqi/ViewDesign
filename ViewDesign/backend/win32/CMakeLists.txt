if(NOT BACKEND STREQUAL "win32")
  return()
endif()

file(GLOB_RECURSE LIB_SOURCES *.h *.cpp)
target_sources(ViewDesign PRIVATE ${LIB_SOURCES})
