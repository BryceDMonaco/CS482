cmake_minimum_required (VERSION 2.6)
set (CMAKE_CXX_STANDARD 11)
project (2-spamfilter)

add_executable (training Training.cpp)
add_executable (classify Classify.cpp)

