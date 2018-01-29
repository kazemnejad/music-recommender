#include <chrono>
#include <iostream>

#include "data.hpp"
#include "recommender.hpp"

using std::cout;
using std::endl;

using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;
using std::chrono::milliseconds;

int main() {
    auto startTs = high_resolution_clock::now();

    init_data();
    run_recommender();

    cout << "all actions took: "
         << duration_cast<milliseconds>(high_resolution_clock::now() - startTs).count() * 1.0 / 1000
         << "s\n";

    return 0;
}