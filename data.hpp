//
// Created by AmirHosein on 12/29/2017 AD.
//

#pragma once

#include <string>
#include <set>
#include <unordered_map>
#include <tuple>
#include <vector>

#include <fstream>

using std::vector;
using std::string;
using std::tuple;
using std::ifstream;
using std::pair;
using std::function;
using std::string;
using std::set;
using std::unordered_map;

class Data {
public:
    explicit Data(const string &trainingDataFilename, const string &evDataFilename);

    const unordered_map<string, set<unsigned long>> &getSongListeners() const;

    const unordered_map<string, int> &getSongListenersCount() const;

    const set<string> &getUsers() const;

    const unordered_map<string, unsigned long> &getIntUserIdMap() const;

    const unordered_map<string, set<string>> &getEvaluationUsersSongs() const;

    const std::vector<string> &getSortedSongs() const;

    static std::vector<string> getEvaluationUsers(const string &filename);

protected:
    void readTrainingData(const string &filename);

    void readEvaluationData(const string &filename);

private:
    unordered_map<string, set<unsigned long >> songListeners;
    unordered_map<string, set<string>> userSongs;

    std::vector<string> sortedSongs;
    set<string> users;

    unordered_map<string, int> songListenersCount;
    unordered_map<string, unsigned long> intUserIdMap;
};

