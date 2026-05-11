cmake_minimum_required(VERSION 4.3.2)

project(Ship C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)


add_executable(Ships main.c src/usefulMath/usefulMath.c src/lodepng/lodepng.c src/imageWork/imageWork.c src/structures/structures.c)
