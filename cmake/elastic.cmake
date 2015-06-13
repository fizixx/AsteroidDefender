
set(ELASTIC_ROOT_DIR "" CACHE PATH "Elastic source root directory")
if(NOT ELASTIC_ROOT_DIR)
  message(FATAL_ERROR "Elastic not found.  Please set ELASTIC_ROOT_DIR.")
endif()

add_subdirectory("${ELASTIC_ROOT_DIR}" "elastic")
