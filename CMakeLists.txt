cmake_minimum_required(VERSION 3.19)

project(AsteroidDefender)

add_subdirectory(../legion legion)

set(SOURCE_FILES
    src/ad/app/user_interface.cpp
    src/ad/world/generator.cpp
    src/ad/world/world.cpp
    )

nucleus_add_library(ad ${SOURCE_FILES})
target_include_directories(ad PUBLIC src)
target_link_libraries(ad PUBLIC legion)

set(TESTS_FILES
    tests/ad/world/prefabs_tests.cpp
    )

nucleus_add_executable(ad_tests ${TESTS_FILES})
target_compile_definitions(ad_tests PRIVATE -DAS_TESTS)
target_link_libraries(ad_tests PRIVATE ad tests_main)

nucleus_add_executable(AsteroidDefender WIN32 src/ad/main.cpp)
target_link_libraries(AsteroidDefender PRIVATE ad)
