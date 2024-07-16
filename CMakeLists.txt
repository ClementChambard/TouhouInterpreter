# required
cmake_minimum_required(VERSION 3.20)

# set some flags
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE "Debug")

add_compile_options(-Wall -Wextra -Werror)
add_compile_options(-DNS_IMPORT)

if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
  add_compile_options(-O4)
endif()

# set the project name and version
project(ecl VERSION 0.1)

# add the NSEngine lib
set(NS_USE_IMGUI ON CACHE BOOL "enable Imgui" FORCE)
add_subdirectory(NSEngine "${PROJECT_SOURCE_DIR}/NSEngine/build")

# get source files
file(GLOB_RECURSE SRCS src/*.cpp src/*.c)
file(GLOB_RECURSE INCLUDES src/*.hpp src/*.h)

# add the executable
add_executable(ecl ${SRCS} ${INCLUDES})

# set executable props
set_property(TARGET ecl PROPERTY CXX_STANDARD 20)
set_property(TARGET ecl PROPERTY CXX_STANDARD_REQUIRED True)

# link the NSEngine library. All the dependencies are added directly to the engine
target_link_libraries(ecl PUBLIC NSEngine -ljsoncpp -lSDL2 -lSDL2_ttf)

# TODO do this better (copy assets in build directory)
file(COPY assets DESTINATION ${PROJECT_BINARY_DIR})
