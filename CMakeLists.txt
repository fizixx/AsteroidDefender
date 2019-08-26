cmake_minimum_required(VERSION 3.10)
project("AsteroidDefender")

# Dependencies

add_subdirectory(../elastic elastic)
add_subdirectory(../hive hive)

# AsteroidDefender

set(AsteroidDefender_SOURCE_FILES
    src/ad/App/AsteroidDefender.cpp

    src/ad/Geometry/Geometry.h
    src/ad/Geometry/Geometry.cpp

    src/ad/Sprites/Sprite.h
    src/ad/Sprites/SpriteConverter.cpp
    src/ad/Sprites/SpriteConverter.h
    src/ad/Sprites/SpriteRenderer.cpp
    src/ad/Sprites/SpriteRenderer.h

    src/ad/World/Camera.cpp
    src/ad/World/Camera.h
    src/ad/World/TopDownCameraController.h
    src/ad/World/TopDownCameraController.cpp
    src/ad/World/CameraController.h
    src/ad/World/CameraController.cpp
    src/ad/World/Entity.h
    src/ad/World/World.cpp
    src/ad/World/World.h
    )

add_executable(AsteroidDefender WIN32 MACOSX_BUNDLE ${AsteroidDefender_SOURCE_FILES})
target_include_directories(AsteroidDefender PUBLIC src)
target_link_libraries(AsteroidDefender elastic hive)
set_target_properties(AsteroidDefender PROPERTIES
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
    FOLDER AsteroidDefender
    )
