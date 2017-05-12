// Track.cpp
// 
// Class object displays the player's vehicle, road, hazards, and multipliers. 
// 

#include <iomanip>
using std::fixed;
using std::setfill;
using std::setprecision;
using std::setw;

#include <iostream>
using std::wcout;

#include <windows.h>

#include "Track.h"
using TenTenths::Track;

TenTenths::Track::Track()
{
}

TenTenths::Track::~Track()
{
}

// continually updates the road and the player's location
void 
TenTenths::Track::layTrack()
{
    CONSOLE_CURSOR_INFO cursorInfoSave; // structure that saves information about the console cursor
    CONSOLE_CURSOR_INFO cursorInfo;     // structure that stores information about the console cursor

    // returns the handle associated with the console screen buffer
    HANDLE stdHandle = GetStdHandle(STD_OUTPUT_HANDLE);

    // saves cursor settings
    GetConsoleCursorInfo(stdHandle, &cursorInfoSave);
    // obtains cursor settings
    GetConsoleCursorInfo(stdHandle, &cursorInfo);
    // sets cursor visibility
    cursorInfo.bVisible = false;
    // the console cursor is no longer visible
    SetConsoleCursorInfo(stdHandle, &cursorInfo);

    const int PLAYER_ROW = 5;        // the row that the player's vehicle is in
    const int ROAD_SIZE = 23;        // number of rows displayed
    const int STARTING_LOCATION = 5; // player's starting location

    double score = 0; // the player's score

    leftOffroad.assign(ROAD_SIZE, L" ");
    rightOffroad.assign(ROAD_SIZE, L" ");
    road.assign(ROAD_SIZE, L"| : : : : |");
    road[PLAYER_ROW].replace(STARTING_LOCATION, 1, L"v");
    oldLocation = STARTING_LOCATION;
    // displays the road and the player's vehicle
    for (size_t loopCounter = 0; loopCounter < road.size(); ++loopCounter)
    {
        wcout << setw(35) << leftOffroad[loopCounter] << road[loopCounter] << rightOffroad[loopCounter] << "\n";
    }
    wcout << "       Score: " << fixed << setprecision(0) << score << "\n";

    vector<bool> multiplierLocator; // determines which rows have multipliers
    vector<bool> obstacleLocator;   // determines which rows have hazards

    multiplierLocator.assign(ROAD_SIZE, false);
    obstacleLocator.assign(ROAD_SIZE, false);

    mt19937 randomNumGenerator; // produces 32-bit integers. Sequence begins to repeat after 2^19937 - 1 numbers have been 
                                // generated

    // multiplier RNG non-deterministic/deterministic?
    multiplierPtr->estimateEntropy();
    // obstacles RNG non-deterministic/deterministic?
    obstaclesPtr->estimateEntropy();
    // spacing RNG non-deterministic/deterministic?
    spacingPtr->estimateEntropy();
    // spectacle RNG non-deterministic/deterministic?
    if (spectaclePtr->estimateEntropy())
    {
        time_t timeSample;
        tm newYear;

        // records the current time
        time(&timeSample);
        // fills the structure with values that are derived from the time argument and are expressed in the user's time zone  
        localtime_s(&newYear, &timeSample);
        newYear.tm_mon = 0;
        newYear.tm_mday = 1;
        newYear.tm_hour = 0;
        newYear.tm_min = 0;
        newYear.tm_sec = 0;

        // RNG is initialized by using the number of seconds that have passed since New Year 
        randomNumGenerator.seed((unsigned int)difftime(timeSample, mktime(&newYear)));
    }

    // enumerators will trigger the characters' special abilities
    enum
    {
        Carl, Hannah, William, Omar, Giselle
    };

    const int OBSTACLES_PER_ROW = 1;           // the maximum, allowable number of hazards per row
    const int OBSTACLE_SPACING = 6;            // OBSTACLE_SPACING - 1 = number of rows between hazards
    const double MULTIPLIER_TIME_LIMIT = 16.2; // score multiplier can be increased within the time limit
    const double STARTING_DIVISOR = 1;         // reduces the time it takes for the road to update

    bool activateMultiplier = false;             // activate the score multiplier?
    bool changeMultiplier = false;               // change the score multiplier?
    bool displayTenTenthsMessage = false;        // display the ten-tenths message? 
    bool isThereACollision = false;              // did a collision occur? 
    bool raiseMinObstacles = false;              // raise the minimum hazards per row? 
    bool skip[] = { true, false, false };        // determines when raises in difficulty should be skipped
    int maxObstacles = OBSTACLES_PER_ROW;        // the maximum, allowable number of hazards per row
    int maxObstacleSpacing = OBSTACLE_SPACING;   // maxObstacleSpacing - 1 = maximum number of rows between hazards
    int minObstacleSpacing = OBSTACLE_SPACING;   // minObstacleSpacing - 1 = minimum number of rows between hazards
    int multiplierCount = 0;                     // number of multipliers acquired within a certain time interval
    int multiplierPopulationIncr = 0;            // increases the population size for the multiplier RNG distribution
    int rowCount = 0;                            // is used to count to the next row that will have hazards
    int spectaclePopulationIncr = 0;             // increases the population size for the spectacle RNG distribution
    int textColor = 0;                           // determines the color of the console text
    double timeWindowDivisor = STARTING_DIVISOR; // reduces the time it takes for the road to update
    
    // starting time at which the ten-tenths message appears on-screen
    time_t message1StartingTime = 0;
    // starting now, points earned by the player are multiplied
    time_t multiplierStartingTime;
    // records the starting time for the obstacle timer. After a certain amount of time has passed, the game will either 
    // raise the maximum number of hazards per row or decrease the minimum/maximum number of rows between hazards  
    time_t obstacleStartingTime = clock();
    // records the player's starting time
    time_t playerStartingTime = clock();
    // records the starting time for the speed timer. After a certain amount of time has passed, the game will speed up
    time_t speedStartingTime = clock();
    // records the starting time for the text attribute timer. The tenth-tenths message changes color while the timer is 
    // active 
    time_t textAttributeStartingTime = 0;

    // game ends when player collides into a hazard
    while (!isThereACollision)
    {
        const int LEFTMOST_LANE = 1;  // road index for the leftmost lane
        const int LEFT_LANE = 3;      // road index for the left lane 
        const int MIDDLE_LANE = 5;    // road index for the middle lane 
        const int OFFROAD = 70;       // signals that the player is not in a lane
        const int RIGHTMOST_LANE = 9; // road index for the rightmost lane
        const int RIGHT_LANE = 7;     // road index for the right lane

        COORD startingCoordinates = { 0, 0 }; // the console cursor starts at the coordinate system's origin, which is 
                                              // located at the top left corner of the console screen

        int location;                                           // location last obtained from player input
        double timeWindow = CLOCKS_PER_SEC / timeWindowDivisor; // the allotted time for receiving player input

        // records the end time for receiving player input;
        time_t inputEndTime = 0;
        // records the starting time for receiving player input
        time_t inputStartingTime = clock();

        // receives player input. The game difficulty may rise 
        while (inputEndTime < timeWindow && !isThereACollision)
        {
            const int END_DIVISOR = 7;                    // reduces the time it takes for the road to update
            const int MIN_OBSTACLE_SPACING = 1;           // minimum number of rows between hazards 
            const int SECONDS_TILL_OBSTACLE_RAISE_1 = 30; // number of seconds before obstacle raise
            const int SECONDS_TILL_OBSTACLE_RAISE_2 = 60; // number of seconds before obstacle raise
            const int SECONDS_TILL_OBSTACLE_RAISE_3 = 90; // number of seconds before obstacle raise
            const int SECONDS_TILL_SPEED_INCREASE = 9;    // number of seconds before the next speed increase

            // records the seconds that have elapsed since the start of the speed timer
            time_t timeDiff1 = (clock() - speedStartingTime) / CLOCKS_PER_SEC;
            // records the seconds that have elapsed since the start of the obstacle timer 
            time_t timeDiff2 = (clock() - obstacleStartingTime) / CLOCKS_PER_SEC;

            // timer raises the game speed
            if (timeDiff1 >= SECONDS_TILL_SPEED_INCREASE && (int)timeWindowDivisor != END_DIVISOR)
            {
                const int MULTIPLIER_POPULATION_INCR = 9;
                const int SPECTACLE_POPULATION_INCR = 27;
                const double SPEED_INCREASE = .2;

                timeWindowDivisor += SPEED_INCREASE;
                multiplierPopulationIncr += MULTIPLIER_POPULATION_INCR;
                spectaclePopulationIncr += SPECTACLE_POPULATION_INCR;
                speedStartingTime = clock();
                if ((int)timeWindowDivisor == END_DIVISOR)
                {
                    displayTenTenthsMessage = true;
                    message1StartingTime = clock();
                    textAttributeStartingTime = clock();
                }
            }
            // timer either raises the maximum number of hazards per row or decreases the minimum/maximum number of rows 
            // between hazards
            if (timeDiff2 >= SECONDS_TILL_OBSTACLE_RAISE_2 && maxObstacles == OBSTACLES_PER_ROW)
                ++maxObstacles;
            else if (timeDiff2 >= SECONDS_TILL_OBSTACLE_RAISE_1 && (int)timeWindowDivisor != END_DIVISOR && !skip[0]
                && !skip[1])
            {
                --maxObstacleSpacing;
                skip[1] = true;
            }
            else if (timeDiff2 >= SECONDS_TILL_OBSTACLE_RAISE_2 && (int)timeWindowDivisor != END_DIVISOR && !skip[0]
                && !skip[2])
            {
                ++maxObstacles;
                skip[2] = true;
            }
            else if (timeDiff2 >= SECONDS_TILL_OBSTACLE_RAISE_3 && (int)timeWindowDivisor != END_DIVISOR)
            {
                --minObstacleSpacing;
                skip[0] = false;
                skip[1] = false;
                skip[2] = false;
                // resets the obstacle timer
                obstacleStartingTime = clock();
            }
            else if (timeDiff2 >= SECONDS_TILL_OBSTACLE_RAISE_1 && (int)timeWindowDivisor == END_DIVISOR
                && minObstacleSpacing != MIN_OBSTACLE_SPACING)
                --minObstacleSpacing;
            else if (timeDiff2 >= SECONDS_TILL_OBSTACLE_RAISE_3 && (int)timeWindowDivisor == END_DIVISOR 
                && !raiseMinObstacles)
            {
                raiseMinObstacles = true;
                changeMultiplier = true;
            }

            const int LANE_1 = 1;
            const int LANE_5 = 5;

            bool isItAValidLocation; // did the player enter a valid command?

            // stores player input
            location = driverPtr->getDriverLocation();
            // does the player want to use Omar's ability?
            if (location == OFFROAD && driverPtr->identifyDriver() == Omar && (oldLocation == LEFTMOST_LANE 
                || oldLocation == RIGHTMOST_LANE))
            {
                // activates Omar's ability if it's available
                if (driverPtr->driverAbilityActivates(isThereACollision, oldLocation, road[PLAYER_ROW]))
                {
                    if (oldLocation == LEFTMOST_LANE)
                        leftOffroad[PLAYER_ROW] = L"v";
                    else if (oldLocation == RIGHTMOST_LANE)
                        rightOffroad[PLAYER_ROW] = L"v";
                    oldLocation = OFFROAD;
                }
            }
            // determines if player input is valid
            if (location >= LANE_1 && location <= LANE_5)
                isItAValidLocation = true;
            else
            {
                isItAValidLocation = false;
                location = oldLocation;
            }

            // moves the player's vehicle
            if (isItAValidLocation)
            {
                // changes the lane number to the corresponding road index
                switch (location)
                {
                    case 2:
                        location = LEFT_LANE;
                        break;
                    case 3:
                        location = MIDDLE_LANE;
                        break;
                    case 4:
                        location = RIGHT_LANE;
                        break;
                    case 5:
                        location = RIGHTMOST_LANE;
                        break;
                }

                // determines if vehicle moved sideways into a hazard
                isThereACollision = driverPtr->detectCollision(road[PLAYER_ROW], location);
                // updates vehicle location
                if (!isThereACollision)
                {
                    if (oldLocation == OFFROAD)
                    {
                        leftOffroad[PLAYER_ROW] = L" ";
                        rightOffroad[PLAYER_ROW] = L" ";
                        oldLocation = location;
                    }
                    // is there still time left on the clock to increase the score multiplier?
                    if (activateMultiplier && (clock() - multiplierStartingTime) / (double)CLOCKS_PER_SEC 
                        >= MULTIPLIER_TIME_LIMIT)
                    {
                        activateMultiplier = false;
                        multiplierCount = 0;
                    }
                    // determines if vehicle moved sideways into a multiplier
                    if (road[PLAYER_ROW][location] == 'c' || road[PLAYER_ROW][location] == '$')
                    {
                        // starts the timer. The player can increase the score multiplier before the time runs out  
                        if (!activateMultiplier)
                            multiplierStartingTime = clock();
                        // extends the time limit. The player now has more time to increase the score multiplier
                        else
                        {
                            multiplierStartingTime = clock();
                            ++multiplierCount;
                        }
                        activateMultiplier = true;
                        multiplierLocator[PLAYER_ROW] = false;
                    }
                    road[PLAYER_ROW].replace(oldLocation, 1, L" ");
                    road[PLAYER_ROW].replace(location, 1, L"v");
                    oldLocation = location;
                }
                // vehicle crashes
                else
                {
                    if (oldLocation == OFFROAD)
                    {
                        leftOffroad[PLAYER_ROW] = L" ";
                        rightOffroad[PLAYER_ROW] = L" ";
                    }
                    road[PLAYER_ROW].replace(oldLocation, 1, L" ");
                    road[PLAYER_ROW].replace(location, 1, L"*");
                }
            }

            // resets the cursor's position to the top of the console screen. This forces the new update to overwrite the 
            // old update, giving the illusion of movement
            SetConsoleCursorPosition(stdHandle, startingCoordinates);

            // displays the updated road and the player's updated location  
            showTrack(stdHandle, leftOffroad, rightOffroad, road, multiplierLocator, changeMultiplier);
            wcout << "       Score: " << score << "\n";
            // displays the playing time and game messages   
            showPlayingTime(stdHandle, playerStartingTime, message1StartingTime, textAttributeStartingTime, 
                displayTenTenthsMessage, textColor);
            
            inputEndTime = clock() - inputStartingTime;
        }

        // the contents of a row are replaced with the contents of the row ahead of it 
        if (!isThereACollision)
        {
            ++rowCount;
            for (size_t loopCounter = 0; loopCounter < road.size(); ++loopCounter)
            {
                const int SEARCH_FOR_MULTIPLIER = 5; // index that is used to find out if the final row has a multiplier

                int multiplierLocation; // the lane that the multiplier is in 

                // a multiplier may appear in the final row when the road is updated
                vector<bool> hasAMultiplier = multiplierPtr->generateMultiplier(multiplierPopulationIncr, 
                    &randomNumGenerator);

                // should the final row be searched and, if so, which lane has a multiplier?
                for (size_t loopCntr = 0; hasAMultiplier[SEARCH_FOR_MULTIPLIER] && loopCntr < hasAMultiplier.size() - 1; 
                    ++loopCntr)
                {
                    if (hasAMultiplier[loopCntr])
                    {
                        // finds the corresponding road index
                        switch (loopCntr)
                        {
                            case 0:
                                multiplierLocation = LEFTMOST_LANE;
                                break;
                            case 1:
                                multiplierLocation = LEFT_LANE;
                                break;
                            case 2:
                                multiplierLocation = MIDDLE_LANE;
                                break;
                            case 3:
                                multiplierLocation = RIGHT_LANE;
                                break;
                            case 4:
                                multiplierLocation = RIGHTMOST_LANE;
                                break;
                        }
                    }
                }

                // hazards might be added to the final row
                if (rowCount >= spacingPtr->generateSpacing(minObstacleSpacing, maxObstacleSpacing, &randomNumGenerator) 
                    && loopCounter == road.size() - 1)
                {
                    int obstacleCount = 0;

                    // returns a vector that has information on the number of hazards for the final row and their respective 
                    // positions
                    vector<bool> hasAnObstacle = obstaclesPtr->generateObstacles(raiseMinObstacles, maxObstacles, 
                        &randomNumGenerator);
                    
                    // removes old hazards 
                    road[loopCounter] = L"| : : : : |";
                    // which lane in the final row has a hazard?
                    for (size_t loopCntr = 0; loopCntr < hasAnObstacle.size(); ++loopCntr)
                    {
                        if (hasAnObstacle[loopCntr])
                        {
                            int obstacleLocation; // the lane that the obstacle is in

                            ++obstacleCount;
                            // finds the corresponding road index
                            switch (loopCntr)
                            {
                                case 0:
                                    obstacleLocation = LEFTMOST_LANE;
                                    break;
                                case 1:
                                    obstacleLocation = LEFT_LANE;
                                    break;
                                case 2:
                                    obstacleLocation = MIDDLE_LANE;
                                    break;
                                case 3:
                                    obstacleLocation = RIGHT_LANE;
                                    break;
                                case 4:
                                    obstacleLocation = RIGHTMOST_LANE;
                                    break;
                            }
                            // a hazard is added to the final row unless it is in the same location as a multiplier
                            if (!hasAMultiplier[loopCntr])
                                road[loopCounter].replace(obstacleLocation, 1, L"*");
                            // a dollar multiplier is added to the final row
                            else if (changeMultiplier)
                            {
                                road[loopCounter].replace(obstacleLocation, 1, L"$");
                                multiplierLocator[loopCounter] = true;
                                --obstacleCount;
                            }
                            // a cent multiplier is added to the final row
                            else
                            {
                                road[loopCounter].replace(obstacleLocation, 1, L"c");
                                multiplierLocator[loopCounter] = true;
                                --obstacleCount;
                            }
                        }
                    }
                    if (obstacleCount > 0)
                        obstacleLocator[loopCounter] = true;
                    // resets the count to the next row that will have hazards
                    rowCount = 0;
                }
                // updates a row
                else
                {
                    // ensures that the row behind the player is correctly displayed, i.e., it does not display the player's 
                    // vehicle 
                    if (loopCounter == PLAYER_ROW - 1)
                    {
                        if (oldLocation != OFFROAD)
                        {
                            road[loopCounter + 1].replace(location, 1, L" ");
                            road[loopCounter] = road[loopCounter + 1];
                        }
                        else
                            road[loopCounter] = road[loopCounter + 1];
                    }
                    // ensures that the row with the player is correctly displayed
                    else if (loopCounter == STARTING_LOCATION)
                    {
                        // determines if the player's vehicle crashed into an obstacle in front of it 
                        isThereACollision = driverPtr->detectCollision(road[PLAYER_ROW + 1], location);
                        // triggers William's ability if the player were to collide into an obstacle 
                        if (driverPtr->identifyDriver() == William && isThereACollision)
                        {
                            // determines if the player avoided a collision by swerving to the left or right 
                            isThereACollision = driverPtr->driverAbilityActivates(isThereACollision, location, 
                                road[PLAYER_ROW + 1]);
                            // returns the player's location after William's ability has been used
                            location = driverPtr->passLocation();
                            oldLocation = location;
                        }
                        // updates player row
                        if (!isThereACollision)
                        {
                            // is there still time left on the clock to increase the score multiplier?
                            if (activateMultiplier && (clock() - multiplierStartingTime) / (double)CLOCKS_PER_SEC 
                                >= MULTIPLIER_TIME_LIMIT)
                            {
                                activateMultiplier = false;
                                multiplierCount = 0;
                            }
                            if (oldLocation != OFFROAD)
                            {
                                if (road[loopCounter + 1][location] == 'c' || road[loopCounter + 1][location] == '$')
                                {
                                    // starts the timer. The player can increase the score multiplier before the time runs 
                                    // out 
                                    if (!activateMultiplier)
                                        multiplierStartingTime = clock();
                                    // extends the time limit. The player now has more time to increase the score multiplier
                                    else
                                    {
                                        multiplierStartingTime = clock();
                                        ++multiplierCount;
                                    }
                                    activateMultiplier = true;
                                    multiplierLocator[loopCounter + 1] = false;
                                }
                                road[loopCounter + 1].replace(location, 1, L"v");
                                road[loopCounter] = road[loopCounter + 1];
                            }
                            else
                                road[loopCounter] = road[loopCounter + 1];
                        }
                        // vehicle crashes
                        else
                        {
                            // records the player's end time 
                            time_t playerEndTime = (clock() - playerStartingTime) / CLOCKS_PER_SEC;

                            road[loopCounter + 1].replace(location, 1, L"*");
                            road[loopCounter] = road[loopCounter + 1];
                        }
                    }
                    // updates the final row
                    else if (loopCounter == road.size() - 1)
                    {
                        // removes old hazards 
                        road[loopCounter] = L"| : : : : |";
                        // a dollar multiplier is added to the final row
                        if (hasAMultiplier[SEARCH_FOR_MULTIPLIER] && changeMultiplier)
                        {
                            road[loopCounter].replace(multiplierLocation, 1, L"$");
                            multiplierLocator[loopCounter] = true;
                        }
                        // a cent multiplier is added to the final row
                        else if (hasAMultiplier[SEARCH_FOR_MULTIPLIER])
                        {
                            road[loopCounter].replace(multiplierLocation, 1, L"c");
                            multiplierLocator[loopCounter] = true;
                        }
                    }
                    // updates row
                    else
                        road[loopCounter] = road[loopCounter + 1];
                    // updates the information on which rows have hazards. The final row is excluded 
                    if (loopCounter != road.size() - 1)
                    {
                        multiplierLocator[loopCounter] = multiplierLocator[loopCounter + 1];
                        obstacleLocator[loopCounter] = obstacleLocator[loopCounter + 1];
                    }
                    else
                    {
                        // the final row will not have a multiplier
                        if (!hasAMultiplier[SEARCH_FOR_MULTIPLIER])
                            multiplierLocator[loopCounter] = false;
                        // the final row will not have any hazards
                        obstacleLocator[loopCounter] = false;
                    }
                }
            }
        }

        // resets the cursor's position to the top of the console screen. This forces the new update to overwrite the old 
        // update, giving the illusion of movement
        SetConsoleCursorPosition(stdHandle, startingCoordinates);

        // did the player score? Will the game end?
        if (!isThereACollision)
        {
            const int ROW_BEHIND_PLAYER = 4; // the row directly behind the player's vehicle 

            // the player's score increases for every line of obstacles that the player's vehicle passes
            if (obstacleLocator[ROW_BEHIND_PLAYER])
            {
                const double CARL_BONUS = .25;    // obstacles are worth more when playing as Carl
                const double CENT_BONUS = 2.75;   // extra points are awarded if a cent multiplier was recently acquired
                const double DOLLAR_BONUS = 6.75; // extra points are awarded if a dollar multiplier was recently acquired
                const int OBSTACLE_REWARD = 1;    // added to the player's score every time the player's vehicle passes a 
                                                  // line of obstacles

                double multiplierBonus = 0; // obtaining a multiplier will temporarily raise the bonus

                if (activateMultiplier && !changeMultiplier)
                    multiplierBonus = CENT_BONUS;
                else if (activateMultiplier)
                    multiplierBonus = DOLLAR_BONUS;
                if (driverPtr->identifyDriver() == Carl)
                    score += OBSTACLE_REWARD + CARL_BONUS + multiplierBonus + multiplierCount;
                else
                    score += OBSTACLE_REWARD + multiplierBonus + multiplierCount;
            }
            
            // displays the updated road and the player's updated location  
            showTrack(stdHandle, leftOffroad, rightOffroad, road, multiplierLocator, changeMultiplier);
            wcout << "       Score: " << score << "\n";
            // displays the playing time and game messages   
            showPlayingTime(stdHandle, playerStartingTime, message1StartingTime, textAttributeStartingTime, 
                displayTenTenthsMessage, textColor);
        }
        // vehicle crashes
        else
        { 
            // displays the crash location in a different color and removes all multipliers from the road
            for (size_t loopCounter = 0; loopCounter < road.size(); ++loopCounter)
            {
                for (size_t loopCntr = 1; loopCntr < road[loopCounter].size(); ++++loopCntr)
                {
                    if (road[loopCounter][loopCntr] == L'c' || road[loopCounter][loopCntr] == L'$')
                        road[loopCounter][loopCntr] = L' ';
                }
                if (loopCounter == PLAYER_ROW)
                {
                    wcout << setw(35) << " ";
                    for (size_t loopCntr = 0; loopCntr < road[PLAYER_ROW].size(); ++loopCntr)
                    {
                        if (loopCntr == location)
                        {
                            CONSOLE_SCREEN_BUFFER_INFO bufferInfo; // structure that stores information about the console 
                                                                   // screen buffer

                            // obtains buffer settings 
                            GetConsoleScreenBufferInfo(stdHandle, &bufferInfo);
                            // changes the color of the crash location
                            SetConsoleTextAttribute(stdHandle, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
                            road[PLAYER_ROW].replace(location, 1, L"*");
                            wcout << road[PLAYER_ROW][location];
                            // restores the old buffer settings
                            SetConsoleTextAttribute(stdHandle, bufferInfo.wAttributes);
                        }
                        else
                            wcout << road[PLAYER_ROW][loopCntr];
                    }
                    wcout << "\n";
                }
                else
                    wcout << setw(46) << road[loopCounter] << "\n";
            }
        }
    }

    // restores the old cursor settings
    SetConsoleCursorInfo(stdHandle, &cursorInfoSave);
}

// sets member pointer to character class object
void
TenTenths::Track::setDriver(Driver *driverPtr)
{
    this->driverPtr = driverPtr;
}

// sets member pointer to multiplier class object
void
TenTenths::Track::setMultiplier(Multiplier *multiplierPtr)
{
    this->multiplierPtr = multiplierPtr;
}

// sets member pointer to obstacle class object 
void
TenTenths::Track::setObstacles(Obstacles *obstaclesPtr)
{
    this->obstaclesPtr = obstaclesPtr;
}

// sets member pointer to spacing class object
void
TenTenths::Track::setSpacing(Spacing *spacingPtr)
{
    this->spacingPtr = spacingPtr;
}

// sets member pointer to spectacle class object
void
TenTenths::Track::setSpectacle(Spectacle *spectaclePtr)
{
    this->spectaclePtr = spectaclePtr;
}

// displays the playing time and game messages 
void 
TenTenths::Track::showPlayingTime(HANDLE stdHandle, time_t playerStartingTime, time_t message1StartingTime, 
    time_t &textAttributeStartingTime, bool displayTenTenthsMessage, int &textColor) const
{
    const int MESSAGE_TIME_LIMIT = 5;
    const int SECONDS_PER_HOUR = 3600;
    const int SECONDS_PER_MINUTE = 60;

    // records the seconds that have elapsed since the game started   
    time_t playingTime = (clock() - playerStartingTime) / CLOCKS_PER_SEC;

    int hours = (int)playingTime / SECONDS_PER_HOUR;
    int remainder = (int)playingTime % SECONDS_PER_HOUR;
    int minutes = remainder / SECONDS_PER_MINUTE;
    int seconds = remainder % SECONDS_PER_MINUTE;

    if (displayTenTenthsMessage && (clock() - message1StartingTime) / CLOCKS_PER_SEC <= MESSAGE_TIME_LIMIT)
    {
        const double COLOR_TIME_LIMIT = .2;

        time_t latestTxtAttrTime = clock();
        // number of seconds that have elapsed since the previous time recording 
        double seconds = (latestTxtAttrTime - textAttributeStartingTime) / (double)CLOCKS_PER_SEC;

        CONSOLE_SCREEN_BUFFER_INFO bufferInfo; // structure that stores information about the console screen buffer

        // obtains buffer settings
        GetConsoleScreenBufferInfo(stdHandle, &bufferInfo);
        if (seconds > COLOR_TIME_LIMIT)
        {
            ++textColor;
            textAttributeStartingTime = latestTxtAttrTime;
        }
        // changes the color of the game message
        switch (textColor)
        {
            case 0:
                SetConsoleTextAttribute(stdHandle, FOREGROUND_RED | FOREGROUND_INTENSITY);
                break;
            case 1:
                SetConsoleTextAttribute(stdHandle, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
                break;
            case 2:
                SetConsoleTextAttribute(stdHandle, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
                break;
            case 3:
                SetConsoleTextAttribute(stdHandle, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
                break;
            case 4:
                SetConsoleTextAttribute(stdHandle, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
                break;
            case 5:
                SetConsoleTextAttribute(stdHandle, FOREGROUND_RED | FOREGROUND_INTENSITY);
                textColor = 0;
            break;
        }
        wcout << "       TENTENTHS";
        // restores the old buffer settings
        SetConsoleTextAttribute(stdHandle, bufferInfo.wAttributes);
    }
    else if (minutes != 0 || seconds > 0)
        wcout << "       " << setfill(L'0') << setw(2) << hours << ":" << setw(2) << minutes << ":" << setw(2) << seconds 
            << setfill(L' ') << " ";
}

// displays the updated road and the player's updated location
void 
TenTenths::Track::showTrack(HANDLE stdHandle, vector<wstring> &leftOffroad, vector<wstring> &rightOffroad, 
    vector<wstring> &road, vector<bool> &multiplierLocator, bool changeMultiplier) const
{
    for (size_t loopCounter = 0; loopCounter < road.size(); ++loopCounter)
    {
        if (!changeMultiplier && multiplierLocator[loopCounter])
        {
            wstring multiplierRow; // this section of the road has a cent multiplier

            multiplierRow = road[loopCounter];
            // replaces the multiplier character with the cent symbol 
            for (size_t loopCntr = 1; loopCntr < multiplierRow.size(); ++++loopCntr)
            {
                if (multiplierRow[loopCntr] == 'c')
                    multiplierRow.replace(loopCntr, 1, L"\u00A2");
            }

            const DWORD NUMBER_OF_CHARS_TO_WRITE = 11; // the # of characters to be written to the console screen buffer
 
            LPCWSTR multiplierRowPtr = multiplierRow.c_str();  // pointer to a null-terminated array 
            DWORD numOfCharsWritten;                           // receives the # of characters written to the screen buffer
            LPDWORD numOfCharsWrittenPtr = &numOfCharsWritten;

            wcout << setw(35) << leftOffroad[loopCounter];
            // writes the string to the console screen buffer
            WriteConsole(stdHandle, multiplierRowPtr, NUMBER_OF_CHARS_TO_WRITE, numOfCharsWrittenPtr, NULL);
            wcout << rightOffroad[loopCounter] << "\n";
        }
        else
            wcout << setw(35) << leftOffroad[loopCounter] << road[loopCounter] << rightOffroad[loopCounter] << "\n";
    }
}
