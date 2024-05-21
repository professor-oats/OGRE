cmake_minimum_required(VERSION 3.28)
project(Samples)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_USE_RELATIVE_PATHS true)
set(CMAKE_SUPPRESS_REGENERATION true)

set(OGRE_HOME "C:/Users/chron/OGRE/out/Debug/sdk")
set(OGRE_DIR "C:/Users/chron/OGRE/out/Debug/sdk/CMake")
set(CMAKE_MODULE_PATH "$ENV{OGRE_HOME}/CMake/;${CMAKE_MODULE_PATH}")

set(CMAKE_DEBUG_POSTFIX "_d")

set(dir ${PROJECT_SOURCE_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${dir}/mybin")

find_package(OGRE REQUIRED COMPONENTS Bites CONFIG)

add_executable(Samples main.cpp)

set_target_properties(Samples PROPERTIES DEBUG_POSTFIX _d)

target_include_directories(Samples PUBLIC "C:/Users/chron/OGRE/out/Debug/sdk/include/OGRE" "C:/Users/chron/OGRE/out/Debug/sdk/include/OGRE/Bites"
        "C:/Users/chron/OGRE/out/Debug/sdk/include/OGRE/RTShaderSystem")

target_link_libraries(Samples OgreBites OgreMain Codec_STBI)


