# CMAKELists

cmake_minimum_required (VERSION 2.8.11)
project (cppDisplay)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I/usr/include/SDL -D_GNU_SOURCE=1 -D_REENTRANT -g")

include_directories (include)

set(EXE_SOURCES
"src/main.cpp"
"src/Mpu9250.cpp"
"src/Renderer.cpp"
)

add_executable (cppDisplay ${EXE_SOURCES})

target_link_libraries (cppDisplay LINK_PUBLIC SDL2 cairo pthread)
