// Driver.h
//
// Classes for the game characters. Each driver has a unique ability. 
//

#ifndef TENTENTHS_DRIVER_H
#define TENTENTHS_DRIVER_H

#include <string>
using std::wstring;

namespace TenTenths
{
    class Driver
    {
    public:
        Driver(int driverLocation);
        ~Driver();
        void setDriverLocation(int location);
        virtual int passLocation() const;
        // determines if the player's vehicle crashed into an obstacle 
        bool detectCollision(wstring &road, int driverLocation) const;
        // converts user input to an integer that can be used by the program
        int convertUserInput(wstring userInput) const;
        // player can move to a new location
        virtual void moveCar();
        virtual bool driverAbilityActivates(bool isThereACollision, int driverLocation, wstring &road) = 0;
        virtual int getDriverLocation() const;
        virtual int identifyDriver() const = 0;
    private:
        int driverLocation; // the location last inputted by the player
    };

    class CompetitiveCarl : public Driver
    {
    public:
        CompetitiveCarl(int driverLocation);
        ~CompetitiveCarl();
        virtual bool driverAbilityActivates(bool isThereACollision, int driverLocation, wstring &road);
        // return value indicates that the player selected Carl
        virtual int identifyDriver() const;
    };

    class HardyHannah : public Driver
    {
    public:
        HardyHannah(int driverLocation);
        ~HardyHannah();
        virtual bool driverAbilityActivates(bool isThereACollision, int driverLocation, wstring &road);
        // return value indicates that the player selected Hannah
        virtual int identifyDriver() const;
    };

    class WeavingWilliam : public Driver
    {
    public:
        WeavingWilliam(int driverLocation);
        ~WeavingWilliam();
        // use character's special ability
        bool driverAbilityActivates(bool isThereACollision, int location, wstring &road);
        // triggers character's special ability
        virtual int identifyDriver() const;
        // returns the character's new location
        virtual int passLocation() const;
    private:
        int alteredLocation; // player's location after William's ability has been used
    };

    class OffroadingOmar : public Driver
    {
    public:
        OffroadingOmar(int location);
        ~OffroadingOmar();
        // use character's special ability
        bool driverAbilityActivates(bool isThereACollision, int location, wstring &road);
        // triggers character's special ability
        virtual int identifyDriver() const;
    };

    class GlobetrottingGiselle : public Driver
    {
    public:
        GlobetrottingGiselle(int location);
        ~GlobetrottingGiselle();
        // use character's special ability
        bool driverAbilityActivates(bool isThereACollision, int location, wstring &road);
        // triggers character's special ability
        virtual int identifyDriver() const;
    };
}

#endif
