//
// Created by AmirHosein on 1/27/2018 AD.
//

#include "recommender.hpp"

#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <thread>
#include <sstream>
#include <algorithm>
#include <mutex>

#include "tbb/concurrent_queue.h"

#include "config.hpp"
#include "data.hpp"

using std::pair;
using std::string;
using std::ostringstream;
using std::ostream_iterator;
using std::ifstream;
using std::ofstream;
using std::thread;
using tbb::concurrent_queue;

using std::cerr;
using std::endl;
using std::min;
using std::set_intersection;
using std::back_inserter;
using std::sqrt;
using std::pow;

using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;
using std::chrono::milliseconds;

typedef pair<string, std::vector<unsigned int>> ResultItem;

static concurrent_queue<string> *usersQueue;
static concurrent_queue<ResultItem> *resultQueue;

class acout : public std::ostringstream {
public:
    acout() = default;

    ~acout() {
        std::lock_guard<std::mutex> guard(_mutexPrint);
        std::cout << this->str();
    }

private:
    static std::mutex _mutexPrint;
};

std::mutex acout::_mutexPrint{};

void fillUserQueue() {
    ifstream fin(KAGGLE_USERS);

    std::vector<string> users;
    users.reserve(NUM_USERS);
    string userName;
    while (fin >> userName)
        users.push_back(userName);

    for (auto it = users.begin() + START_POINT; it < users.begin() + NUM_USER_TO_RECOMMEND; ++it)
        usersQueue->push(*it);
}

double calculate_cosine_similarity(const unsigned int &sId1, const unsigned int &sId2) {
    size_t len1 = songs[sId1].size();
    size_t len2 = songs[sId2].size();

    std::vector<unsigned int> intersection;
    intersection.reserve(min(len1, len2));
    set_intersection(
            songs[sId1].begin(), songs[sId1].end(),
            songs[sId2].begin(), songs[sId2].end(),
            back_inserter(intersection)
    );

    if (intersection.empty())
        return 0.0;

    return intersection.size() * 1.0 / (sqrt(len1) * sqrt(len2));
}

double calculate_similarity_prob(const unsigned int &sId, const unsigned int &uId) {
    size_t sLen = songs[sId].size();
    size_t uLen = songs[uId].size();

    std::vector<unsigned int> intersection;
    intersection.reserve(min(sLen, uLen));
    set_intersection(
            songs[sId].begin(), songs[sId].end(),
            songs[uId].begin(), songs[uId].end(),
            back_inserter(intersection)
    );

    if (intersection.empty())
        return 0.0;

    return intersection.size() * 1.0 / (pow(sLen, PROB_SIM_PARAM) * pow(uLen, 1 - PROB_SIM_PARAM));
}

double calculate_similarity_tversky(const unsigned int &sId, const unsigned int &uId) {
    size_t sLen = songs[sId].size();
    size_t uLen = songs[uId].size();

    std::vector<unsigned int> intersection;
    intersection.reserve(min(sLen, uLen));
    set_intersection(
            songs[sId].begin(), songs[sId].end(),
            songs[uId].begin(), songs[uId].end(),
            back_inserter(intersection)
    );

    if (intersection.empty())
        return 0.0;

    size_t intersectionSize = intersection.size();

    return intersectionSize * 1.0 /
           (intersectionSize + T_SIM_ALPHA * (uLen - intersectionSize) + T_SIM_BETA * (sLen - intersectionSize));
}

double calculate_similarity_jacard(const unsigned int &sId, const unsigned int &uId) {
    size_t sLen = songs[sId].size();
    size_t uLen = songs[uId].size();

    std::vector<unsigned int> intersection;
    intersection.reserve(min(sLen, uLen));
    set_intersection(
            songs[sId].begin(), songs[sId].end(),
            songs[uId].begin(), songs[uId].end(),
            back_inserter(intersection)
    );

    if (intersection.empty())
        return 0.0;

    size_t intersectionSize = intersection.size();

    return intersectionSize * 1.0 / (uLen + sLen - intersectionSize);
}

std::vector<unsigned int> recommend(const string &user) {
    auto startTs = high_resolution_clock::now();

    if (evalUsers->count(user) == 0)
        return std::vector<unsigned int>(popularSongs->begin(), popularSongs->end());

    const auto &userSongs = (*evalUsers)[user];

    auto *scores = new double[NUM_SONGS + 1];
    for (unsigned songId = 1; songId <= NUM_SONGS; ++songId) {
        scores[songId] = 0.0;

        if (songs[songId].empty())
            continue;

        for (const auto &uSongId : userSongs) {
            if (songs[uSongId].empty())
                continue;

            double similarity = calculate_similarity(songId, uSongId);
            scores[songId] += similarity * similarity * similarity;
        }
    }

    std::vector<unsigned int> result(NUM_SONGS);
    for (unsigned int i = 0; i < NUM_SONGS; ++i)
        result[i] = i + 1;

    std::sort(result.begin(), result.end(),
              [scores](const unsigned int &songId1, const unsigned int &songId2) -> bool {
                  return scores[songId1] > scores[songId2];
              }
    );

    std::vector<unsigned int> newSongs;
    newSongs.reserve(MAX_RECOMMENDED_SONG);
    for (auto const &songId : result) {
        if (newSongs.size() >= MAX_RECOMMENDED_SONG)
            break;

        if (userSongs.count(songId) == 0)
            newSongs.emplace_back(songId);
    }

    acout{} << "# done, took " << duration_cast<milliseconds>(high_resolution_clock::now() - startTs).count()
            << "ms <- "
            << user << endl;

    delete[] scores;

    return newSongs;
}

void result_collector() {
    int numResult = 0;

    ofstream fout(OUTPUT_PATH);
    while (numResult != NUM_USER_TO_RECOMMEND) {
        ResultItem result;
        if (!resultQueue->try_pop(result))
            continue;

        fout << result.first << "__";

        ostringstream oss;
        std::copy(result.second.begin(), result.second.end() - 1,
                  std::ostream_iterator<unsigned int>(oss, ",")
        );
        oss << result.second.back();

        fout << oss.str() << endl;

        numResult++;
    }

    fout.close();
}

void recommender() {
    string user;
    while (usersQueue->try_pop(user))
        resultQueue->push(make_pair(
                user,
                recommend(user)
        ));
}

void run_recommender() {
    usersQueue = new concurrent_queue<string>;
    resultQueue = new concurrent_queue<ResultItem>;

    fillUserQueue();

    unsigned int numCpu = thread::hardware_concurrency();

    std::vector<thread> workers(numCpu - 1);
    for (auto &t : workers)
        t = thread(&recommender);

    thread collector(&result_collector);

    for (auto &t : workers)
        t.join();

    collector.join();
}
