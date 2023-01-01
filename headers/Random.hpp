#ifndef GGD_RANDOM_HPP
#define GGD_RANDOM_HPP
#include <random>

class Random {
private:
    Random(){};
public:
    static float getInRange(float lower, float upper);
    static float getFromNormalDistribution();
    Random(Random const&) = delete;
    void operator=(Random const&) = delete;
};

#endif //GGD_RANDOM_HPP
