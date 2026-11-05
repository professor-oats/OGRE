cmake_minimum_required(VERSION 3.13.0)
project(OgreBestPracticeApp)

# Set C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_PREFIX_PATH "/usr/local")

# Find the OGRE package and require the Bites component for ApplicationContext
find_package(OGRE REQUIRED COMPONENTS Bites CONFIG)

# Define your executable and list all source/header files
add_executable(OgreBestPracticeApp
    main.cpp
    GameApplication.h
    GameApplication.cpp
)

# Link against OgreBites 
# (This automatically sets up include directories and pulls in all core OGRE dependencies)
target_link_libraries(OgreBestPracticeApp OgreBites)
