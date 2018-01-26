#include <iostream>
#include <fstream>
#include <chrono>
#include <sstream>
#include "data.hpp"

#include "recommendation-engine.hpp"
#include "parallel-executor.hpp"

#define START_OFFSET 0
#define NUM_USERS 100

using std::ofstream;
using std::cout;
using std::cerr;
using std::endl;
using std::ostringstream;
using std::chrono::microseconds;
using std::chrono::nanoseconds;
using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;

void saveResult(const unordered_map<string, std::vector<string>> &result) {
    ofstream fout("result.txt");
    for (const auto &recommendation : result) {
        fout << recommendation.first << "__";

        ostringstream oss;
        std::copy(recommendation.second.begin(), recommendation.second.end() - 1,
                  std::ostream_iterator<string>(oss, ",")
        );
        oss << recommendation.second.back();

        fout << oss.str() << endl;
    }
}

void printResult(const unordered_map<string, std::vector<string>> &result) {
    for (const auto &pair : result) {
        cout << "[" << pair.first << "] =>" << endl;
        cout << "... [   ";
        for (const auto &song : pair.second) {
            cerr << song << "  |  ";
        }
        cout << "   ]" << endl;
    }
}

void run() {
    Data data("train_triplets.txt", "kaggle_visible_evaluation_triplets.txt");

    const auto &userIds = Data::getEvaluationUsers("kaggle_users.txt");
    std::vector<string> targetUsers(userIds.begin() + START_OFFSET, userIds.begin() + NUM_USERS);


    RecommendationEngine engine(data);
    const auto &recommendedSongs = engine.recommend(targetUsers);
    saveResult(recommendedSongs);
}

void runParallel() {
//    Data data("train_triplets.txt", "kaggle_visible_evaluation_triplets.txt");
//    Data data("/Volumes/RAM Disk/train_triplets.txt", "/Volumes/RAM Disk/kaggle_visible_evaluation_triplets.txt");
//
//    const auto &userIds = Data::getEvaluationUsers("/Volumes/RAM Disk/kaggle_users.txt");
//    std::vector<string> targetUsers(userIds.begin() + START_OFFSET, userIds.begin() + NUM_USERS);

//    RecommendationEngine engine(data);

    ParallelExecutor executor("result.txt");
    executor.start();
}

int main() {
    //runParallel();
    run();
    return 0;
}