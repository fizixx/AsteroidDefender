cmake_minimum_required(VERSION 3.15)

project("AsteroidDefender")

add_subdirectory(../elastic elastic)
add_subdirectory(../hive hive)
add_subdirectory(../legion legion)

file(GLOB_RECURSE AsteroidDefender_FILES src/**.h src/**.cpp)

nucleus_add_executable(AsteroidDefender WIN32 ${AsteroidDefender_FILES})
target_include_directories(AsteroidDefender PUBLIC src)
target_link_libraries(AsteroidDefender PRIVATE hive)
target_link_libraries(AsteroidDefender PRIVATE elastic)
target_link_libraries(AsteroidDefender PRIVATE legion)

set(TESTS_FILES
    tests/ad/world/prefabs_tests.cpp
    )

nucleus_add_executable(AsteroidDefender_tests ${TESTS_FILES} ${AsteroidDefender_FILES})
target_compile_definitions(AsteroidDefender_tests PRIVATE -DAS_TESTS)
target_include_directories(AsteroidDefender_tests PUBLIC src)
target_link_libraries(AsteroidDefender_tests PRIVATE testsMain)
target_link_libraries(AsteroidDefender_tests PRIVATE hive)
target_link_libraries(AsteroidDefender_tests PRIVATE elastic)
target_link_libraries(AsteroidDefender_tests PRIVATE legion)
