cmake_minimum_required(VERSION 3.24)
project(Lisp)

set(CMAKE_CXX_STANDARD 23)

add_executable(Lisp main.cpp includes/Crossline/crossline.c includes/mpc/mpc.c includes/mpc/mpc.h src/parser/Parser.cpp src/parser/Parser.h src/Evaluation/Evaluation.cpp src/Evaluation/Evaluation.h src/parser/Lval.cpp src/parser/Lval.h src/buildins/buildins.h src/Constants.h)

include_directories(includes/Crossline)