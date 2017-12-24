cmake_minimum_required(VERSION 3.9)
project(music_recommender)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(music_recommender main.cpp recommendation-engine.cpp recommendation-engine.hpp data.cpp data.hpp)