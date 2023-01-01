#include "../headers/Random.hpp"

float Random::getInRange(float lower, float upper) {
    //https://stackoverflow.com/questions/7560114/random-number-c-in-some-range
    static std::random_device rd; // obtain a random number from hardware
    static std::mt19937 gen(rd()); // seed the generator

    std::uniform_real_distribution<>distr(lower, upper); // define the range
    return distr(gen); // generate numbers
}

float Random::getFromNormalDistribution() {
    //https://stackoverflow.com/questions/38244877/how-to-use-stdnormal-distribution
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::normal_distribution<float> d;
    return d(gen);
}
