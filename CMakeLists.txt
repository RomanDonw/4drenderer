cmake_minimum_required(VERSION 3.23)
project(4drenderer LANGUAGES C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

add_library(glad OBJECT "external/glad/glad.c")
target_include_directories(glad PUBLIC "external/glad/")

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src/*.h" "src/*.c")
add_executable(${PROJECT_NAME} ${SOURCES})
unset(SOURCES)

find_package(glfw3 3.3 REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE glfw glad)
