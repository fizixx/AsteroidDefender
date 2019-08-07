cmake_minimum_required(VERSION 3.10)
project("AsteroidDefender")

# Dependencies

add_subdirectory(../elastic elastic)
add_subdirectory(../hive hive)
# add_subdirectory(../junctions junctions)

# AsteroidDefender

include_directories("src")

file(GLOB_RECURSE "SOURCE_FILES" "src/*.cpp" "src/*.h")

add_executable(AsteroidDefender WIN32 MACOSX_BUNDLE ${SOURCE_FILES})
target_link_libraries(AsteroidDefender elastic hive)
