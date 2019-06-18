#include <iostream>
#include <thread>
#include <chrono>

#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include "tools.hpp"
#include "EveWindow.hpp"
#include "EveLocalChannel.hpp"
#include "Spaceship.hpp"


std::mutex mDataAccess;
bool stopAllThreads = false;

using std::cout;
using std::endl;

void refreshData( EveWindow* eveWindow, EveLocalChannel* eveLocalChannel = nullptr, Spaceship* spaceship = nullptr )
{
    while( true )
    {
        mDataAccess.lock();
        eveWindow->refresh();
        if ( eveLocalChannel != nullptr ) eveLocalChannel->refresh();
        if ( spaceship != nullptr ) spaceship->refresh();
        mDataAccess.unlock();

        if ( debugMode() )
        {
            cv::imshow("Debug", *eveWindow->get_ewImagePtr());
            cv::waitKey(50);
        }
        std::this_thread::sleep_for( std::chrono::milliseconds( 250 ) );
        if ( ::stopAllThreads ) break;
    }  
}

void soundAlarm( EveLocalChannel* elc = nullptr , Spaceship* ship = nullptr )
{
    bool warningsPresent = true;
    bool enemyInLocal = true;

    std::this_thread::sleep_for( std::chrono::milliseconds( 1000 ) );
    while( true )
    {
        warningsPresent = false;

        mDataAccess.lock();
        if ( elc != nullptr )
        {
            enemyInLocal = elc->dangerDetected();
            if ( elc->recognitionProblemPresent() )
                warningsPresent = true;
        }
        if ( ship != nullptr )
            if ( ship->recognitionProblemPresent() )
                warningsPresent = true;
        mDataAccess.unlock();

        if ( warningsPresent ) playSoundFile( "warning.wav" );
        if ( enemyInLocal ) playSoundFile( "enemy_detected.wav" );

        std::this_thread::sleep_for( std::chrono::milliseconds( 1000 ) );
        if ( ::stopAllThreads ) break;
    }
}

int main( int argc, char** argv )
{

    setlocale( LC_ALL, "" );
    cout << std::fixed << endl;

    // debugMode( true );

    if ( argc != 2 )
    {
        cout << "Usage: " << argv[0] << " 'EVE window title' " << endl;
        return -1;
    }

    getResourcePath( argv[0] );
    EveWindow* ew = new EveWindow( argv[1] );
    EveLocalChannel* elc = new EveLocalChannel( ew, "Local-TopLeft.bmp", "Local-BottomRight.bmp", 1, 3 );
    Spaceship* ship = new Spaceship( ew );

    std::thread refreshDataThread( refreshData, ew, elc, ship );
    std::thread alarmThread( soundAlarm, elc, ship );

    cv::namedWindow("Debug", cv::WINDOW_AUTOSIZE );

    cout << "Press Enter to exit..." << endl;
    std::cin.get();

    cv::destroyWindow("Debug");

    ::stopAllThreads = true;
    alarmThread.join();
    refreshDataThread.join();

    delete elc;
    delete ew;

    cout << " INFO: Finished!" << endl << endl;

    return 0;
}

