// RNG.cpp
//
// Class is responsible for generating the game's randomized assets. 
//

#include "RNG.h"
using TenTenths::RNG;
using TenTenths::Multiplier;
using TenTenths::Obstacles;
using TenTenths::Spacing;
using TenTenths::Spectacle;

TenTenths::RNG::RNG()
{
}

TenTenths::RNG::~RNG()
{
}

// finds out what type of random-number generator will be used in the game (non-deterministic/deterministic)
bool 
TenTenths::RNG::estimateEntropy()
{
    random_device randomNumGenerator;

    if (randomNumGenerator.entropy() == 0)
    {
        isRNGDeterministic = true;

        return(true);
    }
    else
    {
        isRNGDeterministic = false;

        return(false);
    }
}

bool 
TenTenths::RNG::getIsRNGDeterministic() const
{
    return (isRNGDeterministic);
}

TenTenths::Multiplier::Multiplier(int probabilityCountdown) : probabilityCountdown(probabilityCountdown)
{
}

TenTenths::Multiplier::~Multiplier()
{
}

// may generate a multiplier
vector<bool>
TenTenths::Multiplier::generateMultiplier(int multiplierPopulationIncr, mt19937 *RNGPtr)
{
    const int COUNTDOWN_STARTING_VALUE = 16;

    if (probabilityCountdown < COUNTDOWN_STARTING_VALUE)
        --probabilityCountdown;
    else if (probabilityCountdown == 0)
        probabilityCountdown = COUNTDOWN_STARTING_VALUE;

    const int INITIAL_POPULATION_SIZE = 135;

    int maximum = INITIAL_POPULATION_SIZE + 3 * multiplierPopulationIncr; // largest value that can be produced by the 
                                                                          // random-number generator distribution. The 
                                                                          // probability of generating a multiplier is kept 
                                                                          // near constant by growing the value as the game 
                                                                          // speed increases 
  
    // maps the value produced by the random-number generator to a value in the specified range. The outcome determines 
    // whether or not a multiplier is generated 
    uniform_int_distribution<int> distribution1(1, maximum);
    // could be non-deterministic, depending on how it is implemented in the library
    random_device randomNumGenerator;
    int RNGResult;

    bool isRNGDeterministic = getIsRNGDeterministic();

    if (!isRNGDeterministic)
        RNGResult = distribution1(randomNumGenerator);
    else
        RNGResult = distribution1(*RNGPtr);

    const int CONTAINS_MULTIPLIER = 5;      // index that is used to find out if the row has a multiplier
    const int LANE_5 = 5;
    const int MULTIPLIER_UPPER_BOUND_1 = 3; // a multiplier is generated if the outcome is less than or equal to this number
    const int MULTIPLIER_UPPER_BOUND_2 = 5; // a multiplier is generated if the outcome is less than or equal to this number

    // maps the value produced by the random-number generator to a value in the specified range. The outcome determines a 
    // multiplier's lane number
    uniform_int_distribution<int> distribution2(1, LANE_5);
    // a multiplier's location can be stored in the vector. Inspection of the final element reveals the presence or absence 
    // of a multiplier  
    vector<bool> hasAMultiplier = { false, false, false, false, false, false };

    // determines which lane has a multiplier, provided there is a multiplier for the row
    if (probabilityCountdown == COUNTDOWN_STARTING_VALUE && RNGResult <= MULTIPLIER_UPPER_BOUND_1)
    {
        --probabilityCountdown;
        if (!isRNGDeterministic)
            hasAMultiplier[distribution2(randomNumGenerator) - 1] = true;
        else
            hasAMultiplier[distribution2(*RNGPtr) - 1] = true;
        hasAMultiplier[CONTAINS_MULTIPLIER] = true;

        return (hasAMultiplier);
    }
    else if (probabilityCountdown < COUNTDOWN_STARTING_VALUE && RNGResult <= MULTIPLIER_UPPER_BOUND_2)
    {
        if (!isRNGDeterministic)
            hasAMultiplier[distribution2(randomNumGenerator) - 1] = true;
        else
            hasAMultiplier[distribution2(*RNGPtr) - 1] = true;
        hasAMultiplier[CONTAINS_MULTIPLIER] = true;

        return (hasAMultiplier);
    }
    else
        return (hasAMultiplier);
}

TenTenths::Obstacles::Obstacles()
{
}

TenTenths::Obstacles::~Obstacles()
{
}

// generates road hazards
vector<bool> 
TenTenths::Obstacles::generateObstacles(bool raiseMinObstacles, int maxObstacles, mt19937 *RNGPtr) const
{
    random_device randomNumGenerator; // could be non-deterministic, depending on how it is implemented in the library
    int minObstacles;                 // the minimum, allowable number of hazards per line

    bool isRNGDeterministic = getIsRNGDeterministic();

    if (raiseMinObstacles)
    {
        const int MIN_OBSTACLES_UPPER_BOUND = 2;

        // maps the value produced by the random-number generator to a value in the specified range. The outcome determines 
        // the minimum number of hazards that can be generated 
        uniform_int_distribution<int> distribution1(1, MIN_OBSTACLES_UPPER_BOUND);

        if (!isRNGDeterministic)
            minObstacles = distribution1(randomNumGenerator);
        else
            minObstacles = distribution1(*RNGPtr);
    }
    else
        minObstacles = 1;

    // maps the value produced by the random-number generator to a value in the specified range. The outcome determines the 
    // number of hazards that are generated 
    uniform_int_distribution<int> distribution2(minObstacles, maxObstacles);
    int obstacleNum;
 
    if (!isRNGDeterministic)
        obstacleNum = distribution2(randomNumGenerator);
    else
        obstacleNum = distribution2(*RNGPtr);

    vector<bool> hasAnObstacle = { false, false, false, false, false }; // vector contains information on the number of 
                                                                        // hazards for the row and their respective 
                                                                        // positions

    // determines which lanes have hazards
    for (int loopCounter = 0; loopCounter < obstacleNum; ++loopCounter)
    {
        const int LANE_5 = 5;

        // maps the value produced by the random-number generator to a value in the specified range. The outcome, that is to 
        // say the lane number, is assigned a hazard
        uniform_int_distribution<int> distribution3(1, LANE_5);  
        int obstacleLocation;

        if (!isRNGDeterministic)
            obstacleLocation = distribution3(randomNumGenerator);
        else
            obstacleLocation = distribution3(*RNGPtr);

        // stores hazard location
        if (!hasAnObstacle[obstacleLocation - 1])
            hasAnObstacle[obstacleLocation - 1] = true;
        // finds another location for the hazard if the current location has already been taken
        else
        {
            bool skip = false;

            for (int loopCntr = 0; loopCntr < LANE_5 && skip == false; ++loopCntr)
            {
                if (!hasAnObstacle[loopCntr])
                {
                    hasAnObstacle[loopCntr] = true;
                    skip = true;
                }
            }
        }
    }

    return (hasAnObstacle);
}

TenTenths::Spacing::Spacing()
{
}

TenTenths::Spacing::~Spacing()
{
}

// returns a value that determines if a line of hazards will be added to the road
int
TenTenths::Spacing::generateSpacing(int minObstacleSpacing, int maxObstacleSpacing, mt19937 *RNGPtr) const
{
    // maps the value produced by the random-number generator to a value in the specified range. The outcome is the spacing
    // between hazards
    uniform_int_distribution<int> distribution(minObstacleSpacing, maxObstacleSpacing);
    // could be non-deterministic, depending on how it is implemented in the library
    random_device randomNumGenerator;

    if (!getIsRNGDeterministic())
        return (distribution(randomNumGenerator));
    else
        return (distribution(*RNGPtr));
}

TenTenths::Spectacle::Spectacle()
{
}

TenTenths::Spectacle::~Spectacle()
{
}

// may generate a spectacle
void
TenTenths::Spectacle::generateSpectacle(int spectaclePopulationIncr, mt19937 *RNGPtr) const
{
    const int INITIAL_POPULATION_SIZE = 278;

    int maximum = INITIAL_POPULATION_SIZE + 2 * spectaclePopulationIncr; // largest value that can be produced by the 
                                                                         // random-number generator distribution. The 
                                                                         // probability of generating a spectacle is kept 
                                                                         // near constant by growing the value as the game 
                                                                         // speed increases 

    // maps the value produced by the random-number generator to a value in the specified range. The outcome determines 
    // whether or not a spectacle is generated 
    uniform_int_distribution<int> distribution(1, maximum);
    // could be non-deterministic, depending on how it is implemented in the library
    random_device randomNumGenerator;
    int RNGResult;

    bool isRNGDeterministic = getIsRNGDeterministic();

    if (!isRNGDeterministic)
        RNGResult = distribution(randomNumGenerator);
    else
        RNGResult = distribution(*RNGPtr);

    const int SPECTACLE_UPPER_BOUND = 1; // a spectacle is generated if the outcome is less than or equal to this number

    if (RNGResult == SPECTACLE_UPPER_BOUND)
    {
    }
}
