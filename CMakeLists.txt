cmake_minimum_required(VERSION 3.15)

project("AsteroidDefender")

add_subdirectory(../elastic elastic)
add_subdirectory(../hive hive)
add_subdirectory(../legion legion)

file(GLOB_RECURSE AsteroidDefender_FILES src/**.h src/**.cpp)

nucleus_add_executable(AsteroidDefender WIN32 ${AsteroidDefender_FILES})
target_include_directories(AsteroidDefender PUBLIC src)
target_link_libraries(AsteroidDefender hive)
target_link_libraries(AsteroidDefender elastic)
target_link_libraries(AsteroidDefender legion)
