//
// Created by AmirHosein on 12/29/2017 AD.
//

#pragma once

#include "data.hpp"

#define MAX_NUM_RECOMMENDED_SONG 100

class RecommendationEngine {
public:
    explicit RecommendationEngine(const Data &d);

    unordered_map<string, std::vector<string>> recommend(const std::vector<string> &users);

    std::vector<string> recommendToUser(const string &userId);

private:

    unordered_map<string, double> calculateScoreForUserSongs(const string &userId);

    double calculateScore(const string &songId, const string &userId);

    double calculateSimilarity(const string &sId1, const string &sId2);

    template<typename T>
    static std::vector<T> set_intersection(const unordered_set<T> &a, const unordered_set<T> &b);

    const Data &data;
};

