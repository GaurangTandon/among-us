cmake_minimum_required(VERSION 3.10)

project(Assignment VERSION 1.0)

set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(SOURCES "${SRC_DIR}/main.cpp" "../glad.c")


set(CMAKE_CXX_FLAGS --std=c++17 -g -pthread -lglfw -lGLU -lGL -lXrandr -lXxf86vm -lXi -lXinerama -lX11 -lrt -ldl -fsanitize=address -fsanitize=undefined)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(${PROJECT_NAME} ${SOURCES})
