//
// Created by AmirHosein on 1/26/2018 AD.
//

#ifndef MUSIC_RECOMMENDER_PARALLEL_EXECUTOR_HPP
#define MUSIC_RECOMMENDER_PARALLEL_EXECUTOR_HPP

#include "recommendation-engine.hpp"
#include "queue.hpp"

#include <fstream>

using std::ofstream;
using std::pair;


class ParallelExecutor {
public:
    explicit ParallelExecutor(RecommendationEngine &engine, const std::vector<string> &users,
                              const string &outputFilename);

    void start();

private:
    void resultSaver();

    void recommender();

    RecommendationEngine &engine;
    ofstream output;
    ThreadSafeQueue<string> usersToRecommend;
    ThreadSafeQueue<pair<string, std::vector<string>>> resultToSave;
    unsigned long numUsers;

};


#endif //MUSIC_RECOMMENDER_PARALLEL_EXECUTOR_HPP
