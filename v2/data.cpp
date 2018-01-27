//
// Created by AmirHosein on 1/27/2018 AD.
//

#include "data.hpp"

#include <iostream>
#include <fstream>
#include <chrono>

using std::ifstream;
using std::cout;
using std::endl;

using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;
using std::chrono::milliseconds;

std::vector<unsigned int> *songs;
unordered_map<string, unordered_set<unsigned int>> *evalUsers;
std::vector<unsigned int> *popularSongs;

void init_data() {
    songs = new std::vector<unsigned int>[NUM_SONGS + 1];
    evalUsers = new unordered_map<string, unordered_set<unsigned int>>(110000);

    auto start = high_resolution_clock::now();
    unordered_map<string, unsigned int> songToId(NUM_SONGS);
    ifstream fin(KAGGLE_SONGS);

    string songName;
    unsigned int songId;
    while (fin >> songName >> songId)
        songToId[songName] = songId;

    fin.close();

    cout << "Done reading songs name map" << endl;

    unordered_map<string, unsigned int> userToId(1020000);
    unsigned int index = 0;

    unordered_map<unsigned int, unordered_set<unsigned int>> songsUnordered(384546);
    string userName;
    int count;

    fin.open(TRAIN_TRIPLETS);
    while (fin >> userName >> songName >> count) {
        unsigned int userId = 0;
        if (userToId.count(userName) == 0) {
            userId = ++index;
            userToId[userName] = userId;
        } else {
            userId = userToId[userName];
        }

        songId = songToId[songName];
        if (songsUnordered.count(songId) == 1) {
            songsUnordered[songId].insert(userId);
        } else {
            unordered_set<unsigned int> set;
            set.reserve(130);
            set.insert(userId);

            songsUnordered[songId] = set;
        }
    }
    fin.close();

    cout << "Done reading songs unordered" << endl;

    for (const auto &pair : songsUnordered) {
        std::vector<unsigned int> dest(pair.second.size());
        copy(pair.second.begin(), pair.second.end(), dest.begin());
        std::sort(dest.begin(), dest.end());
        songs[pair.first] = dest;
    }
    cout << "Done sorting songs" << endl;

    fin.open(EVALUATION_TRIPLETS);
    while (fin >> userName >> songName >> count)
        if (evalUsers->count(userName) == 1) {
            (*evalUsers)[userName].insert(songToId[songName]);
        } else {
            unordered_set<unsigned int> set;
            set.reserve(20);
            set.insert(songToId[songName]);

            (*evalUsers)[userName] = set;
        }

    fin.close();

    cout << "Done reading eval songs" << endl;

    std::vector<unsigned int> tmp(songsUnordered.size());
    for (const auto &p : songsUnordered)
        tmp.emplace_back(p.first);

    auto compareFunction = [](unsigned int &elem1, unsigned int &elem2) {
        return songs[elem1].size() > songs[elem2].size();
    };
    std::sort(tmp.begin(), tmp.end(), compareFunction);

    popularSongs = new std::vector<unsigned int>(MAX_RECOMMENDED_SONG);
    copy(tmp.begin(), tmp.begin() + MAX_RECOMMENDED_SONG, popularSongs->begin());

    cout << "took " << duration_cast<milliseconds>(high_resolution_clock::now() - start).count() << endl;
}

void test() {
    cout << (popularSongs == nullptr) << endl;
}