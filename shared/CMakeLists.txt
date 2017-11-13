cmake_minimum_required(VERSION 3.8)
project(shared)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        include/Camera.h
        include/Drawing.h
        include/Editor.h
        include/Game.h
        include/glfont2.h
        include/GlutEngine.h
        include/Parts.h
        include/Physics.h
        include/PhysicsDelegate.h
        include/PinballBridgeInterface.h
        include/Playfield.h
        include/Renderer.h
        include/Texture.h
        include/TimerDelegate.h
        include/Types.h
        include/Util.h
        src/Camera.cpp
        src/Drawing.cpp
        src/Editor.cpp
        src/Game.cpp
        src/glfont2.cpp
        src/GlutEngine.cpp
        src/Physics.cpp
        src/Playfield.cpp
        src/Renderer.cpp
        src/Util.cpp include/Parts/Flipper.h src/Parts/Flipper.cpp include/shapeGroup.h)

include_directories(include)
include_directories(../lib/chipmunk/include)
include_directories(../lib/lua/include)

link_directories(/usr/)

add_executable(shared ${SOURCE_FILES})
