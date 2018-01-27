cmake_minimum_required(VERSION 3.9)
project(music_similarities_cache)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-ltbb -O3")

add_executable(music_similarities_cache main.cpp data.hpp recommender.cpp recommender.hpp config.hpp data.cpp)