
set(CANAVS_ROOT_DIR "" CACHE PATH "Canvas source root directory")
if(NOT CANAVS_ROOT_DIR)
  message(FATAL_ERROR "Canvas not found. Please set CANAVS_ROOT_DIR.")
endif()

add_subdirectory("${CANAVS_ROOT_DIR}" "canvas")
