cmake_minimum_required(VERSION 3.22)
project(Learn_OpenGL)

set(CMAKE_CXX_STANDARD 20)

include_directories(glad/include)
include_directories(lib)

find_package(glfw3 3.3 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(assimp REQUIRED)

file(GLOB_RECURSE SRCS "src/*.h" "lib/*.c" "lib/imui/*.cpp")
set(TARGET_SOURCES ${SRCS})


add_executable(Learn_OpenGL src/main.cpp ${TARGET_SOURCES})

target_link_libraries(Learn_OpenGL glfw)
target_link_libraries(Learn_OpenGL OpenGL::GL)
target_link_libraries(Learn_OpenGL assimp::assimp)