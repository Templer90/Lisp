cmake_minimum_required(VERSION 3.24)
project(Lisp)

set(CMAKE_CXX_STANDARD 23)

add_executable(Lisp main.cpp includes/Crossline/crossline.c includes/mpc/mpc.c includes/mpc/mpc.h src/parser/Lval.cpp src/parser/Lval.h src/Constants.h src/environment/ValueHolder.cpp src/environment/ValueHolder.h src/buildins/LvalBuildins.h src/buildins/buildins_Math.h src/buildins/buildins_Misc.h src/buildins/Buildins_Function.h)

file(COPY tests DESTINATION .)

include_directories(includes/Crossline)