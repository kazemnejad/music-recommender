//
// Created by AmirHosein on 1/27/2018 AD.
//

#ifndef MUSIC_SIMILARITIES_CACHE_CONFIG_HPP
#define MUSIC_SIMILARITIES_CACHE_CONFIG_HPP

#define KAGGLE_SONGS "kaggle_songs.txt"
#define KAGGLE_USERS "kaggle_users.txt"
#define OUTPUT_PATH "result.txt"
#define TRAIN_TRIPLETS "train_triplets.txt"
#define EVALUATION_TRIPLETS "kaggle_visible_evaluation_triplets.txt"

#define NUM_USERS 110000
#define NUM_SONGS 386220

#define START_POINT 0
#define NUM_USER_TO_RECOMMEND 1000

#define MAX_RECOMMENDED_SONG 500

#define PROB_SIM_PARAM 0.15

#define T_SIM_ALPHA 0.8
#define T_SIM_BETA (1-T_SIM_ALPHA)

#define calculate_similarity calculate_cosine_similarity


#endif //MUSIC_SIMILARITIES_CACHE_CONFIG_HPP
