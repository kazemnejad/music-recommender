//
// Created by AmirHosein on 1/27/2018 AD.
//

#ifndef MUSIC_SIMILARITIES_CACHE_UTILS_HPP
#define MUSIC_SIMILARITIES_CACHE_UTILS_HPP

#include <unordered_map>
#include <unordered_set>
#include <string>
#include <vector>

#include "config.hpp"

using std::unordered_map;
using std::unordered_set;
using std::string;

extern std::vector<unsigned int> *songs;
extern unordered_map<string, unordered_set<unsigned int>> *evalUsers;
extern std::vector<unsigned int> *popularSongs;

void init_data();

void test();

#endif //MUSIC_SIMILARITIES_CACHE_UTILS_HPP
