//
// Created by AmirHosein on 12/29/2017 AD.
//

#include "recommendation-engine.hpp"

#include <algorithm>
#include <cmath>

using std::sort;
using std::set_intersection;
using std::back_inserter;
using std::sqrtf;

template<typename T>
std::vector<string> keys(const unordered_map<string, T> &map) {
    std::vector<string> result;
    result.reserve(map.size());

    for (const auto &it : map)
        result.emplace_back(it.first);

    return result;
}

RecommendationEngine::RecommendationEngine(const Data &d) : data(d) {}

unordered_map<string, std::vector<string>> RecommendationEngine::recommend(const std::vector<string> &users) {
    unordered_map<string, std::vector<string>> result;
    for (auto &userId: users)
        result[userId] = recommendToUser(userId);

    return result;
}

float RecommendationEngine::calculateSimilarity(const string &sId1, const string &sId2) {
    auto songList1 = data.getSongListeners().at(sId1);
    auto songList2 = data.getSongListeners().at(sId2);

    if (songList1.empty() || songList2.empty())
        return 0.0;

    std::vector<unsigned long> intersection;
    set_intersection(
            songList1.begin(), songList1.end(),
            songList2.begin(), songList2.end(),
            back_inserter(intersection)
    );

    return intersection.size() * 1.0f / (sqrtf(songList1.size()) * sqrtf(songList2.size()));
}

float RecommendationEngine::calculateScore(const string &songId, const string &userId) {
    bool isSongAvailable = data.getSongListeners().count(songId) == 1;
    if (!isSongAvailable)
        return 0;

    float score = 0.0;
    for (const auto &userSongId : data.getEvaluationUsersSongs().at(userId)) {
        isSongAvailable = data.getSongListeners().count(userSongId) == 1;
        if (!isSongAvailable)
            continue;

        float similarity = calculateSimilarity(songId, userSongId);

        // score += similarity ^ 3
        score += similarity * similarity * similarity;
    }

    return score;
}

unordered_map<string, float> RecommendationEngine::calculateScoreForUserSongs(const string &userId) {
    unordered_map<string, float> scores;
    for (const auto &songId : data.getSortedSongs())
        scores[songId] = calculateScore(songId, userId);

    return scores;
}

std::vector<string> RecommendationEngine::recommendToUser(const string &userId) {
    auto songs = data.getEvaluationUsersSongs();
    if (songs.count(userId) == 0)
        return std::vector<string>(
                data.getSortedSongs().begin(),
                data.getSortedSongs().begin() + MAX_NUM_RECOMMENDED_SONG
        );

    auto userSongs = songs.at(userId);
    auto songScores = calculateScoreForUserSongs(userId);

    // sort map by value and convert it to a list
    std::vector<string> recommendedSongs = keys(songScores);
    sort(recommendedSongs.begin(), recommendedSongs.end(),
         [songScores](const string &a, const string &b) -> bool {
             return songScores.at(a) > songScores.at(b);
         }
    );

    std::vector<string> result;
    for (auto &song : recommendedSongs) {
        if (result.size() >= MAX_NUM_RECOMMENDED_SONG)
            break;

        if (userSongs.count(song) == 0)
            result.push_back(song);
    }

    return result;
}