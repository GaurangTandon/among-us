cmake_minimum_required(VERSION 3.10)

project(AmongUs VERSION 1.0)

set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(SOURCES "${SRC_DIR}/main.cpp" "${SRC_DIR}/game.cpp" "${SRC_DIR}/sprite_renderer.cpp" "${SRC_DIR}/stb_image.cpp" "${SRC_DIR}/shader.cpp" "${SRC_DIR}/texture.cpp" "${SRC_DIR}/resource_manager.cpp" "${SRC_DIR}/game_object.cpp" "glad.c" src/game_maze.cpp src/game_room.cpp src/player.cpp src/text_renderer.cpp src/task.cpp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(TARGET_NAME ${PROJECT_NAME})
add_executable(${TARGET_NAME} ${SOURCES})

target_compile_options(${TARGET_NAME} PRIVATE -g -O2) # -fsanitize=address -fsanitize=undefined)
target_link_libraries(${TARGET_NAME} PRIVATE freetype pthread glfw GLU GL Xrandr Xxf86vm Xi Xinerama X11 rt dl) #-fsanitize=address -fsanitize=undefined)

