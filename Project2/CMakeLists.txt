cmake_minimum_required (VERSION 2.6)
set (CMAKE_CXX_STANDARD 11)
project (2-spamfilter)

add_executable (training src/Training.cpp)
add_executable (classify src/Classify.cpp)

