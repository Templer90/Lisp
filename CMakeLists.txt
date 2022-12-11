cmake_minimum_required(VERSION 3.24)
project(Lisp)

set(CMAKE_CXX_STANDARD 23)

add_executable(Lisp main.cpp includes/Crossline/crossline.c includes/mpc/mpc.c includes/mpc/mpc.h src/parser/Lval.cpp src/parser/Lval.h src/buildins/buildins.h src/Constants.h src/environment/ValueHolder.cpp src/environment/ValueHolder.h src/buildins/LvalBuiltins.h)

file(COPY tests DESTINATION .)

include_directories(includes/Crossline)