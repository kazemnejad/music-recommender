//
// Created by AmirHosein on 12/29/2017 AD.
//

#include "data.hpp"

#include <iostream>
#include <istream>

using std::cout;
using std::endl;
using std::copy;
using std::istream_iterator;

Data::Data(const string &trainingDataFilename, const string &evDataFilename) {
    readTrainingData(trainingDataFilename);
    readEvaluationData(evDataFilename);
}

const unordered_map<string, set<unsigned long>> &Data::getSongListeners() const {
    return songListeners;
}

void Data::readTrainingData(const string &filename) {
    ifstream fin(filename);

    unordered_map<string, set<string>> sl;
    string userId, songId, count;

    cout << "start reading training data..." << endl;
    while (fin >> userId >> songId >> count) {
        if (sl.count(songId)) {
            auto &listeners = sl[songId];
            listeners.insert(userId);
        } else {
            sl[songId] = {userId};
        }

        if (songListenersCount.count(songId))
            songListenersCount[songId] += 1;
        else
            songListenersCount[songId] = 1;

        users.insert(userId);
    }

    cout << "finished reading training file." << endl;
    cout << "start processing training data..." << endl;

    unsigned long index = 0;
    for (auto &user : users) {
        intUserIdMap[user] = index;
        index++;
    }

    for (auto const&[song, listeners] : sl) {
        set<unsigned long> converted;
        for (auto &strId : listeners)
            converted.insert(intUserIdMap[strId]);

        songListeners[song] = converted;
    }

    typedef function<bool(pair<string, int>, pair<string, int>)> Comparator;
    Comparator compFunctor =
            [](std::pair<std::string, int> elem1, std::pair<std::string, int> elem2) {
                return elem1.second > elem2.second;
            };


    set<pair<string, int>, Comparator> sortedSet(
            songListenersCount.begin(),
            songListenersCount.end(),
            compFunctor
    );

    std::vector<pair<string, int>> output(sortedSet.begin(), sortedSet.end());
    std::vector<string> ss(output.size());
    for (int i = 0; i < output.size(); ++i)
        ss[i] = output[i].first;

    this->sortedSongs = ss;
    cout << "done processing training data." << endl;
}

void Data::readEvaluationData(const string &filename) {
    ifstream fin(filename);

    unordered_map<string, set<string>> sl;
    string userId, songId, count;

    cout << "Start Reading Evaluation Data..." << endl;

    while (fin >> userId >> songId >> count) {
        if (userSongs.count(userId)) {
            auto &songs = userSongs[userId];
            songs.insert(songId);
        } else {
            userSongs[userId] = {songId};
        }
    }

    cout << "done Reading Evaluation Data." << endl;
}

const unordered_map<string, int> &Data::getSongListenersCount() const {
    return songListenersCount;
}

const set<string> &Data::getUsers() const {
    return users;
}

const unordered_map<string, unsigned long> &Data::getIntUserIdMap() const {
    return intUserIdMap;
}

const unordered_map<string, set<string>> &Data::getEvaluationUsersSongs() const {
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
