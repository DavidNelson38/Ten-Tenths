// Track.h
//
// Class object displays the player's vehicle, road, hazards, and multipliers.
//

#ifndef TENTENTHS_TRACK_H
#define TENTENTHS_TRACK_H

#include "Driver.h"
using TenTenths::Driver;

#include "RNG.h"
using TenTenths::Multiplier;
using TenTenths::Obstacles;
using TenTenths::Spacing;
using TenTenths::Spectacle;

namespace TenTenths
{
    class Track
    {
    public:
        Track();
        ~Track();
        // continually updates the road and the player's location
        void layTrack();
        // sets member pointer to character class object
        void setDriver(Driver *driverPtr);
        // sets member pointer to multiplier class object
        void setMultiplier(Multiplier *multiplierPtr);
        // sets member pointer to obstacle class object 
        void setObstacles(Obstacles *obstaclesPtr);
        // sets member pointer to spacing class object
        void setSpacing(Spacing *spacingPtr);
        // sets member pointer to spectacle class object
        void setSpectacle(Spectacle *spectaclePtr);
        // displays the playing time and game messages   
        void showPlayingTime(HANDLE stdHandle, time_t playerStartingTime, time_t message1StartingTime, 
            time_t &textAttributeStartingTime, bool displayTenTenthsMessage, int &textColor) const;
        // displays the updated road and the player's updated location
        void showTrack(HANDLE stdHandle, vector<wstring> &leftOffroad, vector<wstring> &rightOffroad, vector<wstring> &road, 
            vector<bool> &multiplierLocator, bool changeMultiplier) const;
    private:
        vector<wstring> leftOffroad;
        vector<wstring> rightOffroad;
        vector<wstring> road;
        Driver *driverPtr;
        Multiplier *multiplierPtr;
        Obstacles *obstaclesPtr;
        Spacing *spacingPtr;
        Spectacle *spectaclePtr;
        int oldLocation;              // player's last location
    };
}

#endif
