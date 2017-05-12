// Ten-Tenths.cpp
//
// A console game that takes advantage of multithreaded race conditions. The player selects a driver and tries to avoid 
// obstacles on an endless road.    
//

#include <iomanip>
using std::setw;

#include <iostream>
using std::wcin;
using std::wcout;

#include <thread>
using std::thread;

#include <windows.h>

#include "Driver.h"
using TenTenths::CompetitiveCarl;
using TenTenths::HardyHannah;
using TenTenths::WeavingWilliam;
using TenTenths::OffroadingOmar;
using TenTenths::GlobetrottingGiselle;

#include "RNG.h"
using TenTenths::Obstacles;
using TenTenths::Spectacle;
using TenTenths::Spacing;
using TenTenths::Multiplier;

#include "Track.h"
using TenTenths::Track;

int main()
{
    wcout << "\n\n" << setw(44) << "TEN TENTHS\n\n" << setw(46) << "game written\n" << setw(56)
        << "by David Nelson\n\n\n\n\n" << "Ten-tenths : To push a car to its absolute limits.\n\n\n\n"
        << "    Change Lanes --->  1 2 3 4 5\n" << "    By Entering       | | | | | |\n"
        << "    1-5               | | | | | |\n" << setw(46) << "    | | | | |v| <--- Player\n" << setw(34)
        << "| | | | | |\n" << setw(58) << "| | | |*| | <--- Avoid Obstacles\n\n\n\n" << "Hit Enter to Continue...";

    while (wcin.get() != '\n');

    bool startGame = false;
    void *playerPtr;
    
    // select game character
    while (!startGame)
    {
        wstring userInput;

        wcout << "\n\nSelect Driver (1-3): \n\n1. CompetitiveCarl    Points Earned Are Doubled\n"
            "2. HardyHannah        Can Get Hit Twice\n"
            "3. WeavingWilliam     Vehicle Will Swerve to the Left or Right" << setw(63) << "To Prevent Collisions\n\n";

        wcin >> userInput;
        if (userInput == L"1")
        {
            playerPtr = new CompetitiveCarl(3);
            startGame = true;
        }
        else if (userInput == L"2")
        {
            playerPtr = new HardyHannah(3);
            startGame = true;
        }
        else if (userInput == L"3")
        {
            playerPtr = new WeavingWilliam(3);
            startGame = true;
        }
        else if (userInput == L"4")
        {
            playerPtr = new OffroadingOmar(3);
            startGame = true;
        }
        else if (userInput == L"5")
        {
            playerPtr = new GlobetrottingGiselle(3);
            startGame = true;
        }
    }

    HANDLE stdHandle;                      // handle to console buffer 
    CONSOLE_SCREEN_BUFFER_INFO bufferInfo; // structure recieving information on console buffer
    DWORD cellNum;                         // number of character cells that a character will be written to 
    DWORD characterNum;                    // pointer to a variable that will receive the number of characters written 
    COORD startingCoordinates = { 0, 0 };  // coordinates of the first character cell that a character will be written to

    stdHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    // retrieve information on console buffer
    GetConsoleScreenBufferInfo(stdHandle, &bufferInfo);
    cellNum = bufferInfo.dwSize.X * bufferInfo.dwSize.Y;
    // clear screen to begin game
    FillConsoleOutputCharacter(stdHandle, (TCHAR)' ', cellNum, startingCoordinates, &characterNum);
    // resets the cursor's position to the top of the console screen
    SetConsoleCursorPosition(stdHandle, startingCoordinates);

    Obstacles hazards;
    Spectacle spectacle;
    Spacing spacing;
    Multiplier multiplier(16);
    Track lanes;

    lanes.setDriver((Driver *)playerPtr);
    lanes.setObstacles(&hazards);
    lanes.setSpectacle(&spectacle);
    lanes.setSpacing(&spacing);
    lanes.setMultiplier(&multiplier);

    // a thread object is created and will begin executing the function specified in the constructor 
    thread driverThread(&Driver::moveCar, (Driver *)playerPtr);
    thread trackThread(&Track::layTrack, lanes);

    // forces calling thread to suspend activity until the thread object has finished
    driverThread.join();
    trackThread.join();

    // cleanup
    delete playerPtr;

    return(EXIT_SUCCESS);
}
