
set(JUNCTIONS_ROOT_DIR "" CACHE PATH "Junctions source root directory")
if(NOT JUNCTIONS_ROOT_DIR)
  message(FATAL_ERROR "Junctions not found.  Please set JUNCTIONS_ROOT_DIR.")
endif()

add_subdirectory("${JUNCTIONS_ROOT_DIR}" "junctions")
target_include_directories("junctions" PUBLIC "${junctions_SOURCE_DIR}/include")
