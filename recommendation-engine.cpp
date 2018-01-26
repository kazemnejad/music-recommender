//
// Created by AmirHosein on 12/29/2017 AD.
//

#include "recommendation-engine.hpp"

#include <iostream>

#include <utility>
#include <cmath>

using std::cout;
using std::cerr;
using std::endl;
using std::swap;
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
    result.reserve(users.size());

    for (auto &userId: users)
        result[userId] = recommendToUser(userId);

    return result;
}

std::vector<string> RecommendationEngine::recommendToUser(const string &userId) {
    if (data.getEvaluationUsersSongs().count(userId) == 0)
        return std::vector<string>(
                data.getSortedSongs().begin(),
                data.getSortedSongs().begin() + MAX_NUM_RECOMMENDED_SONG
        );

    const auto &userSongs = data.getEvaluationUsersSongs().at(userId);
    cout << "# num of user songs: " << userSongs.size() << endl;

    const auto &songScores = calculateScoreForUserSongs(userId);
    cerr << "done calculating scores" << endl;

    // sort map by value and convert it to a list
    std::vector<std::pair<string, double>> recommendedSongs(songScores.size());
    for (const auto &it : songScores)
        recommendedSongs.emplace_back(it);

    cerr << "done calculating scores" << endl;
    sort(recommendedSongs.begin(), recommendedSongs.end(),
         [](std::pair<string, double> a, std::pair<string, double> b) -> bool {
             return a.second > b.second;
         }
    );
    cerr << "done calculating scoress" << endl;
    std::vector<string> result;
    for (const auto &song : recommendedSongs) {
        if (result.size() >= MAX_NUM_RECOMMENDED_SONG)
            break;

        if (userSongs.count(song.first) == 0)
            result.push_back(song.first);
    }

    return result;
}

unordered_map<string, double> RecommendationEngine::calculateScoreForUserSongs(const string &userId) {
    unsigned long numSongs = data.getSortedSongs().size();
    unsigned long counter = 0;

    unordered_map<string, double> scores;
    scores.reserve(numSongs);
    for (const auto &songId : data.getSortedSongs()) {
        double score = calculateScore(songId, userId);
//        cerr << "# song " << ++counter << " / " << numSongs << " = " << score << endl;
        scores[songId] = score;
    }

    return scores;
}

double RecommendationEngine::calculateScore(const string &songId, const string &userId) {
    bool isSongAvailable = data.getSongListeners().count(songId) == 1;
    if (!isSongAvailable)
        return 0.0;

    if (data.getEvaluationUsersSongs().count(userId) == 0)
        return 0.0;

    double score = 0.0;
    for (const auto &userSongId : data.getEvaluationUsersSongs().at(userId)) {
        isSongAvailable = data.getSongListeners().count(userSongId) == 1;
        if (!isSongAvailable)
            continue;

        double similarity = calculateSimilarity(songId, userSongId);

        // score += similarity ^ 3
        score += similarity * similarity * similarity;
    }

    return score;
}

double RecommendationEngine::calculateSimilarity(const string &sId1, const string &sId2) {
    const auto &songList1 = data.getSongListeners().at(sId1);
    const auto &songList2 = data.getSongListeners().at(sId2);

    if (songList1.empty() || songList2.empty())
        return 0.0;

    return set_intersection(songList1, songList2).size() * 1.0 /
           (sqrtf(songList1.size()) * sqrtf(songList2.size()));
}

template<typename T>
std::vector<T> RecommendationEngine::set_intersection(const unordered_set<T> &a, const unordered_set<T> &b) {
    const unordered_set<T> &src = a.size() > b.size() ? b : a;
    const unordered_set<T> &dest = a.size() > b.size() ? a : b;

    std::vector<T> intersection;
    intersection.reserve(src.size());
    for (const auto &item : src)
        if (dest.count(item) == 1)
            intersection.push_back(item);

    return intersection;
}
