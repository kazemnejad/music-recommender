//
// Created by AmirHosein on 1/26/2018 AD.
//

#include "parallel-executor.hpp"

#include <sstream>
#include <iostream>
#include <thread>
#include <iterator>

using std::cerr;
using std::ostringstream;
using std::endl;
using std::make_pair;
using std::thread;

ParallelExecutor::ParallelExecutor(const string &oFilename) {
    Data data("/Volumes/RAM Disk/train_triplets.txt", "/Volumes/RAM Disk/kaggle_visible_evaluation_triplets.txt");

    const auto &userIds = Data::getEvaluationUsers("/Volumes/RAM Disk/kaggle_users.txt");
    std::vector<string> targetUsers(userIds.begin() + 0, userIds.begin() + 100);

    engine = new RecommendationEngine(data);

    output.open(oFilename);

    usersToRecommend = new ThreadSafeQueue<string>(100);
    resultToSave = new ThreadSafeQueue<pair<string, std::vector<string>>>(100);

    numUsers = targetUsers.size();
    for (const auto &userId : targetUsers)
        usersToRecommend->push(userId);
}

void ParallelExecutor::start() {
    unsigned numCpu = thread::hardware_concurrency();

    std::vector<thread> producers(numCpu - 1);
    for (int i = 0; i < producers.size(); i++)
        producers[i] = thread([=] { recommender(); });

    thread collector([=] { resultSaver(); });

    for (auto &t : producers)
        t.join();

    resultToSave->close();

    collector.join();
}

void ParallelExecutor::resultSaver() {
//    cerr << "start_result_save" << endl;
    typedef pair<string, std::vector<string>> RecommResult;

    int counter = 0;

    RecommResult item;
    while ((resultToSave->pop(item)) != ThreadSafeQueue<RecommResult>::CLOSED) {
        cerr << "## " << ++counter << " <done> " << item.first << endl;

        output << item.first << "__";

        ostringstream oss;
        std::copy(item.second.begin(), item.second.end() - 1,
                  std::ostream_iterator<string>(oss, ",")
        );
        oss << item.second.back();

        output << oss.str() << endl;


        if (counter == numUsers)
            usersToRecommend->close();
    }

    cerr << "finish_result_save" << endl;

    output.close();
}

void ParallelExecutor::recommender() {
//    cerr << "start_recommender" << endl;
    string user;
    while (usersToRecommend->pop(user) != ThreadSafeQueue<string>::CLOSED) {
//        cerr << "start for " << user << endl;
        resultToSave->push(make_pair(
                user,
                engine->recommendToUser(user)
        ));
    }
//    cerr << "finish_recommender" << endl;
}


