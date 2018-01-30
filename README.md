# Song Recommendation System 

It is a song recommender implemented using Item-Based Collaborative filtering  algorithm.

This project is implemented by pure modern C++, and no extra library is used(roughly)

# How to compile
1- First install dependencies:

### on Ubuntu(Linux):
`sudo apt install build-essetials git libtbb-dev`

### on MacOS:</br>
first install xcode, And then run following command:</br>
`brew install tbb`

2- Get the source `git clone https://github.com/kazemnejad/music-recommender.git && cd music_recommender` </br>
3- Compile! `make`

# How to run
First you need to get data, downlond using the following links and put them project folder:</br>
1- [train_triplets.txt](http://labrosa.ee.columbia.edu/millionsong/sites/default/files/challenge/train_triplets.txt.zip) </br>
2- [kaggle_visible_evaluation_triplets.txt](https://www.kaggle.com/c/msdchallenge/data) </br>
3- [kaggle_songs.txt](https://www.kaggle.com/c/msdchallenge/data) </br>
4- [kaggle_users.txt](https://www.kaggle.com/c/msdchallenge/data) </br>

then simply execute it:</br>
`./music_recommender`
