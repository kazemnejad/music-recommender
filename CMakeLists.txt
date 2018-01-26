cmake_minimum_required(VERSION 3.9)
project(music_recommender)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "-O3")
set(CMAKE_CXX_FLAGS "-pthread")

add_executable(music_recommender main.cpp recommendation-engine.cpp recommendation-engine.hpp data.cpp data.hpp parallel-executor.cpp parallel-executor.hpp queue.hpp)