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

private:
    float calculateSimilarity(const string &sId1, const string &sId2);

    float calculateScore(const string &songId, const string &userId);

    unordered_map<string, float> calculateScoreForUserSongs(const string &userId);

    std::vector<string> recommendToUser(const string &userId);

    Data data;
};

