// Driver.cpp
//
// Classes for the game characters. Each driver has a unique ability. 
//

#include <iostream>
using std::wcin;

#include <random>
using std::random_device;
using std::uniform_int_distribution;

#include <windows.h>

#include "Driver.h"
using TenTenths::Driver;
using TenTenths::CompetitiveCarl;
using TenTenths::HardyHannah;
using TenTenths::WeavingWilliam;
using TenTenths::OffroadingOmar;
using TenTenths::GlobetrottingGiselle;

TenTenths::Driver::Driver(int driverLocation) : driverLocation(driverLocation)
{
}

TenTenths::Driver::~Driver()
{
}

void 
TenTenths::Driver::setDriverLocation(int location)
{
    driverLocation = location;
}

int TenTenths::Driver::passLocation() const
{
    return 0;
}

// determines if the player's vehicle crashed into an obstacle 
bool
TenTenths::Driver::detectCollision(wstring &road, int driverLocation) const
{
    enum
    {
        Hannah = 1
    };

    for (size_t loopCounter = 1; loopCounter < road.size(); ++++loopCounter)
    {
        if (road[loopCounter] == '*' && driverLocation == loopCounter)
        {
            // Getting hit by a hazard does not necessarily end the game if the player chose Hannah. Instead, Hannah loses 
            // her ability, which can be earned back by scoring a certain amount of points
            if (identifyDriver() == Hannah)
                return (false);

            return (true);
        }
    }

    return (false);
}

// converts user input to an integer that can be used by the program
int
TenTenths::Driver::convertUserInput(wstring userInput) const
{
    const int ASCII_TO_INTEGER = 48;

    int convertedUserInput = (int)userInput[0] - ASCII_TO_INTEGER;

    return (convertedUserInput);
}

// player can move to a new location
void
TenTenths::Driver::moveCar()
{
    const int QUIT_COMMAND = 65; // player leaves the game

    DWORD modeSave; // saves buffer mode
    DWORD mode;     // stores buffer mode

    // returns the handle associated with the console input buffer
    HANDLE stdHandle = GetStdHandle(STD_INPUT_HANDLE); 

    // saves the console input buffer's current mode
    GetConsoleMode(stdHandle, &modeSave);
    // obtains the console input buffer's current mode
    GetConsoleMode(stdHandle, &mode);
    // disables console setting; characters are no longer written to the input buffer as they are read
    SetConsoleMode(stdHandle, mode & ~ENABLE_ECHO_INPUT);
    // receives player input
    while (driverLocation != QUIT_COMMAND)
    {
        wstring userInput;

        wcin >> userInput;
        // converts user input to an integer that can be used by the program
        driverLocation = convertUserInput(userInput);
    }
    // restores the old buffer mode
    SetConsoleMode(stdHandle, modeSave);
}

int TenTenths::Driver::getDriverLocation() const
{
    return (driverLocation);
}

TenTenths::CompetitiveCarl::CompetitiveCarl(int driverLocation) : Driver(driverLocation)
{
}

TenTenths::CompetitiveCarl::~CompetitiveCarl()
{
}

bool TenTenths::CompetitiveCarl::driverAbilityActivates(bool isThereACollision, int driverLocation, wstring &road)
{
    return (false);
}

// return value indicates that the player selected Carl
int 
TenTenths::CompetitiveCarl::identifyDriver() const
{
    enum
    {
        Carl = 0
    };

    return (Carl);
}

TenTenths::HardyHannah::HardyHannah(int driverLocation) : Driver(driverLocation)
{
}

TenTenths::HardyHannah::~HardyHannah()
{
}

bool 
TenTenths::HardyHannah::driverAbilityActivates(bool isThereACollision, int driverLocation, wstring & road) 
{
    return (false);
}

// return value indicates that the player selected Hannah
int 
TenTenths::HardyHannah::identifyDriver() const
{
    enum
    {
        Hannah = 1
    };

    return (Hannah);
}

TenTenths::WeavingWilliam::WeavingWilliam(int driverLocation) : Driver(driverLocation)
{
}

TenTenths::WeavingWilliam::~WeavingWilliam()
{
}

// triggers character's special ability
bool 
TenTenths::WeavingWilliam::driverAbilityActivates(bool isThereACollision, int location, wstring &road)  
{
    uniform_int_distribution<int> distribution(1, 2);
    random_device randomNumGenerator;
    // the value produced by the random number generator determines if the player's vehicle moves to the left or to the 
    // right
    int laneChange = distribution(randomNumGenerator);

    // player's vehicle moves to the left if there is an existing lane
    if (laneChange == 1 && location > 1)
    {
        location -= 2;
        if (road[location] != '*')
            isThereACollision = false;
    }
    // player's vehicle moves to the right if there is an existing lane
    else if (laneChange == 2 && location < 9)
    {
        location += 2;
        if (road[location] != '*')
            isThereACollision = false;
    }
    alteredLocation = location;
    

    return (isThereACollision);
}

// triggers character's special ability
int 
TenTenths::WeavingWilliam::identifyDriver() const
{
    enum
    {
        WilliamAbility = 2
    };

    return (WilliamAbility);
}

// returns the character's new location
int
TenTenths::WeavingWilliam::passLocation() const
{
    return (alteredLocation);
}

TenTenths::OffroadingOmar::OffroadingOmar(int location) : Driver(location)
{
}

TenTenths::OffroadingOmar::~OffroadingOmar()
{
}

bool 
TenTenths::OffroadingOmar::driverAbilityActivates(bool isThereACollision, int location, wstring &road) 
{
    road.replace(location, 1, L" ");

    return(true);
}

int 
TenTenths::OffroadingOmar::identifyDriver() const
{
    enum
    {
        Omar = 3
    };

    return (Omar);
}

TenTenths::GlobetrottingGiselle::GlobetrottingGiselle(int location) : Driver(location)
{
}

TenTenths::GlobetrottingGiselle::~GlobetrottingGiselle()
{
}

bool 
TenTenths::GlobetrottingGiselle::driverAbilityActivates(bool isThereACollision, int location, wstring & road) 
{
    return false;
}

int 
TenTenths::GlobetrottingGiselle::identifyDriver() const
{
    enum
    {
        GiselleAbility = 4
    };

    return (GiselleAbility);
}
