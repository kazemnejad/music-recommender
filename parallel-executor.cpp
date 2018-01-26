//
// Created by AmirHosein on 1/26/2018 AD.
//

#include "parallel-executor.hpp"

#include <sstream>
#include <iostream>
#include <thread>

using std::cerr;
using std::ostringstream;
using std::endl;
using std::make_pair;
using std::thread;

ParallelExecutor::ParallelExecutor(
        RecommendationEngine &engine,
        const std::vector<string> &users,
        const string &oFilename) : engine(engine) {
    output.open(oFilename);

    numUsers = users.size();
    for (const auto &userId : users)
        usersToRecommend.push(userId);
}

void ParallelExecutor::start() {
    unsigned numCpu = thread::hardware_concurrency();

    std::vector<thread> producers(numCpu - 1);
    for (auto &t : producers)
        t = thread([=] { recommender(); });

    thread collector([=] { resultSaver(); });

    for (auto &t : producers)
        t.join();

    resultToSave.close();

    collector.join();
}

void ParallelExecutor::resultSaver() {
    typedef pair<string, std::vector<string>> RecommResult;

    int counter = 0;

    RecommResult item;
    while ((resultToSave.pop(item)) != ThreadSafeQueue<RecommResult>::CLOSED) {
        output << item.first << "__";

        ostringstream oss;
        std::copy(item.second.begin(), item.second.end() - 1,
                  std::ostream_iterator<string>(oss, ",")
        );
        oss << item.second.back();

        output << oss.str() << endl;

        cerr << "## " << ++counter << " <done> " << item.first << endl;

        if (counter == numUsers)
            usersToRecommend.close();
    }

    output.close();
}

void ParallelExecutor::recommender() {
    string user;
    while (usersToRecommend.pop(user) != ThreadSafeQueue<string>::CLOSED)
        resultToSave.push(make_pair(
                user,
                engine.recommendToUser(user)
        ));
}


