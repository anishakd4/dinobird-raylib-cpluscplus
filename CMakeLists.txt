cmake_minimum_required(VERSION 3.15)
project(raylib_app)

set(CMAKE_CXX_STANDARD 17)

# Find raylib via Homebrew
# You can override these with -DRAYLIB_PATH=... when calling cmake
if(APPLE)
    if(EXISTS "/opt/homebrew/opt/raylib")
        set(RAYLIB_PATH "/opt/homebrew/opt/raylib")
    else()
        set(RAYLIB_PATH "/usr/local/opt/raylib")
    endif()
endif()

include_directories(${RAYLIB_PATH}/include)
link_directories(${RAYLIB_PATH}/lib)

add_executable(raylib_app main.cpp)

target_link_libraries(raylib_app
    raylib
    "-framework OpenGL"
    "-framework Cocoa"
    "-framework IOKit"
    "-framework CoreAudio"
)
