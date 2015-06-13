cmake_minimum_required(VERSION 3.0)
project("AsteroidDefender")

# Dependencies

include("cmake/canvas.cmake")
include("cmake/elastic.cmake")
include("cmake/junctions.cmake")
include("cmake/nucleus.cmake")
include("cmake/sfml.cmake")

# AsteroidDefender

include_directories("src")

file(GLOB_RECURSE "SOURCE_FILES" "src/*.cpp" "src/*.h")

add_executable("AsteroidDefender" WIN32 MACOSX_BUNDLE ${SOURCE_FILES})
target_link_libraries("AsteroidDefender" "junctions" "elastic" "canvas" "nucleus")
