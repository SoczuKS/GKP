cmake_minimum_required(VERSION 3.20)
project(GK_Projekt)

include(FetchContent)

FetchContent_Declare(
    SDL
    GIT_REPOSITORY https://github.com/libsdl-org/SDL
    GIT_TAG        SDL2
)
FetchContent_MakeAvailable(SDL)

set(CMAKE_CXX_STANDARD 17)

add_executable(GK_Projekt main.cpp App.cpp)
target_link_libraries(GK_Projekt SDL2::SDL2 SDL2::SDL2main)
