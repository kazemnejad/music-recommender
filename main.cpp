#include <iostream>
#include "data.hpp"
#include "recommendation-engine.hpp"

#define MAX_NUM_USERS 10

using std::cout;
using std::endl;

int main() {
    Data data("train_triplets.txt", "kaggle_visible_evaluation_triplets.txt");
    auto userIds = Data::getEvaluationUsers("kaggle_users.txt");
    std::vector<string> usersToRecommend(userIds.begin(), userIds.end() + MAX_NUM_USERS);

    RecommendationEngine engine(data);
    unordered_map<string, std::vector<string>> recommendedSongs = engine.recommend(usersToRecommend);

    for (auto const&[user, recommSongs] : recommendedSongs) {
        cout << "[" << user << "] =>" << endl;
        cout << "... [   ";
        for (const auto &song : recommSongs) {
            cout << song << "   ";
        }
        cout << "   ]" << endl;
    }

    return 0;
}