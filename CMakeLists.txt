cmake_minimum_required(VERSION 3.10)

project("AsteroidDefender")

add_subdirectory(../elastic elastic)

file(GLOB_RECURSE AsteroidDefender_FILES src/*.h src/*.cpp)

nucleus_add_executable(AsteroidDefender WIN32 ${AsteroidDefender_FILES})
target_include_directories(AsteroidDefender PUBLIC src)
target_link_libraries(AsteroidDefender legion elastic)
