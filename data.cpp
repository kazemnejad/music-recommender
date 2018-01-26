//
// Created by AmirHosein on 12/29/2017 AD.
//

#include "data.hpp"

#define TOTAL_NUM_TRAIN_TRIPLETS 48373586

#include <iostream>
#include <chrono>
#include <algorithm>
#include <istream>
#include <iterator>

using std::cout;
using std::endl;
using std::copy;
using std::istream_iterator;
using std::sort;

using std::chrono::high_resolution_clock;
using std::chrono::milliseconds;
using std::chrono::duration_cast;

Data::Data(const string &trainingDataFilename, const string &evDataFilename) {
    auto startTs = high_resolution_clock::now();

    songListeners.reserve(390 * 1000);
    userSongs.reserve(120 * 1000);
    users.reserve(1020000);

    readTrainingData(trainingDataFilename);
    readEvaluationData(evDataFilename);

    cout << "### reading data took " << duration_cast<milliseconds>(high_resolution_clock::now() - startTs).count()
         << "ms\n";
}

const unordered_map<string, unordered_set<unsigned long>> &Data::getSongListeners() const {
    return songListeners;
}

void Data::readTrainingData(const string &filename) {
    ifstream fin(filename);

    unordered_map<string, unordered_set<string>> sl;
    sl.reserve(350 * 1000);
    string userId, songId, count;

    unsigned int counter = 0;
    cout << "start reading training data..." << endl;
    while (fin >> userId >> songId >> count) {
        if (sl.count(songId)) {
            auto &listeners = sl[songId];
            listeners.insert(userId);
        } else {
            unordered_set<string> listeners;
            listeners.reserve(10);
            listeners.insert(userId);
            sl[songId] = listeners;
        }

        users.insert(userId);
        cout << "# " << (++counter) * 1.0 / TOTAL_NUM_TRAIN_TRIPLETS * 100 << "%\n";
    }

    cout << "finished reading training file." << endl;

    cout << "* num of users: " << users.size() << endl;
    cout << "* num of songs: " << sl.size() << endl;

    cout << "start processing training data..." << endl;

    cout << "start converting ids..." << endl;
    intUserIdMap.reserve(users.size());
    unsigned long index = 0;
    for (auto &user : users) {
        intUserIdMap[user] = index;
        index++;
    }
    cout << "finish converting ids..." << endl;

    cout << "start replacing ids..." << endl;
    for (const auto &pair : sl) {
        unordered_set<unsigned long> converted;
        converted.reserve(pair.second.size());
        for (auto &strId : pair.second)
            converted.insert(intUserIdMap[strId]);

        songListeners[pair.first] = converted;
    }
    cout << "finish replacing ids..." << endl;

    cout << "start sorting..." << endl;
//    set<pair<string, int>, Comparator> sortedSet(
//            songListenersCount.begin(),
//            songListenersCount.end(),
//            compFunctor
//    );

//    std::vector<pair<string, int>> output(sortedSet.begin(), sortedSet.end());
//    std::vector<string> ss(output.size());
//    for (int i = 0; i < output.size(); ++i)
//        ss[i] = output[i].first;

    sortedSongs.reserve(songListeners.size());
    int i = 0;
    for (auto const &pair : songListeners)
        sortedSongs.emplace_back(pair.first);

    auto compareFunction = [this](const string &elem1, const string &elem2) {
        return songListeners[elem1].size() > songListeners[elem2].size();
    };
    sort(sortedSongs.begin(), sortedSongs.end(), compareFunction);

    cout << "finish sorting." << endl;
    cout << "done processing training data." << endl;
}

void Data::readEvaluationData(const string &filename) {
    ifstream fin(filename);

    cout << "Start Reading Evaluation Data..." << endl;

    unsigned int counter = 0;
    string userId, songId, count;
    while (fin >> userId >> songId >> count) {
        if (userSongs.count(userId)) {
            auto &songs = userSongs[userId];
            songs.insert(songId);
        } else {
            userSongs[userId] = {songId};
        }

        cout << "# " << (++counter) * 1.0 / 1450933 * 100 << "%\n";
    }

    cout << "done Reading Evaluation Data." << endl;
}

const unordered_map<string, int> &Data::getSongListenersCount() const {
    return songListenersCount;
}

const unordered_set<string> &Data::getUsers() const {
    return users;
}

const unordered_map<string, unsigned long> &Data::getIntUserIdMap() const {
    return intUserIdMap;
}

const unordered_map<string, unordered_set<string>> &Data::getEvaluationUsersSongs() const {
    return userSongs;
}

const std::vector<string> &Data::getSortedSongs() const {
    return sortedSongs;
}

std::vector<string> Data::getEvaluationUsers(const string &filename) {
    ifstream file(filename);

    std::vector<std::string> userIds;
    copy(istream_iterator<string>(file),
         istream_iterator<string>(),
         back_inserter(userIds));

    return userIds;
}
