// RNG.h
//
// Class is responsible for generating the game's randomized assets. 
//

#ifndef TENTENTHS_RNG_H
#define TENTENTHS_RNG_H

#include <random>
using std::mt19937;
using std::random_device;
using std::uniform_int_distribution;

#include <vector>
using std::vector;

namespace TenTenths
{
    class RNG
    {
    public:
        RNG();
        ~RNG();
        // finds out what type of random-number generator will be used in the game (non-deterministic/deterministic)   
        bool estimateEntropy();
        bool getIsRNGDeterministic() const;
    private:
        bool isRNGDeterministic;
    };

    class Multiplier : public RNG
    {
    public:
        Multiplier(int probabilityCountdown);
        ~Multiplier();
        // may generate a multiplier
        vector<bool> generateMultiplier(int multiplierPopulationIncr, mt19937 *RNGPtr);
    private:
        int probabilityCountdown; // the appearance of a multiplier increases the probability of another multiplier being 
                                  // generated. The probability resets to its default value after a certain number of 
                                  // attempts have been made 
    };

    class Obstacles : public RNG
    {
    public:
        Obstacles();
        ~Obstacles();
        // generates road hazards
        vector<bool> generateObstacles(bool raiseMinObstacles, int maxObstacles, mt19937 *RNGPtr) const;
    };

    class Spacing : public RNG
    {
    public:
        Spacing();
        ~Spacing();
        // returns a value that determines if a line of hazards will be added to the road 
        int generateSpacing(int minObstacleSpacing, int maxObstacleSpacing, mt19937 *RNGPtr) const;
    };

    class Spectacle : public RNG
    {
    public:
        Spectacle();
        ~Spectacle();
        // may generate a spectacle
        void generateSpectacle(int spectaclePopulationIncr, mt19937 *RNGPtr) const;
    };
}

#endif
