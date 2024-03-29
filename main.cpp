#include <iostream>
#include <thread>
#include <chrono>

#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include "eve-zam-tools.hpp"
#include "EveWindow.hpp"
#include "EveLocalChannel.hpp"
#include "ShipModule.hpp"
#include "Spaceship.hpp"


std::mutex mDataAccess;
bool stopAllThreads = false;

using std::wcout;
using std::endl;

void refreshData( EveWindow* eveWindow, EveLocalChannel* eveLocalChannel = NULL, Spaceship* spaceship = NULL )
{
    while( true )
    {
        //TODO: Надо замерить время выполнения критической секции (RefreshTime) и отправлять тред
        //TODO: в сон на (250ms - RefreshTime). Если RefreshTime > 250, выдавать предупреждение и
        //TODO: отправлять тред в сон на 50ms
        mDataAccess.lock();
        eveWindow->refresh();
        if ( eveLocalChannel != NULL ) eveLocalChannel->refresh();
        if ( spaceship != NULL ) spaceship->refresh();
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

void soundAlarm( EveLocalChannel* elc = NULL , Spaceship* ship = NULL )
{
    bool warningsPresent = true;
    bool enemyInLocal = true;

    std::this_thread::sleep_for( std::chrono::milliseconds( 1000 ) );
    while( true )
    {
        warningsPresent = false;

        mDataAccess.lock();
        if ( elc != NULL )
        {
            enemyInLocal = elc->dangerDetected();
            if ( elc->recognitionProblemPresent() )
                warningsPresent = true;
        }
        if ( ship != NULL )
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
    wcout << std::fixed << endl;

    // debugMode( true );

    if ( argc != 2 )
    {
        wcout << "Usage: " << argv[0] << " 'EVE window title' " << endl;
        return -1;
    }

    getResourcePath( argv[0] );
    EveWindow* ew = new EveWindow( argv[1] );
    EveLocalChannel* elc = new EveLocalChannel( ew, "Local-TopLeft.bmp", "Local-BottomRight.bmp", 1, 3 );
    Spaceship* ship = new Spaceship( ew );

    std::thread refreshDataThread( refreshData, ew, elc, ship );
    std::thread alarmThread( soundAlarm, elc, ship );

    cv::namedWindow("Debug", cv::WINDOW_AUTOSIZE );

    wcout << "Press Enter to exit..." << endl;
    std::cin.get();

    cv::destroyWindow("Debug");

    ::stopAllThreads = true;
    alarmThread.join();
    refreshDataThread.join();

    delete elc;
    delete ew;

    wcout << " INFO: Finished!" << endl << endl;

    return 0;
}

