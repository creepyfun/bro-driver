#include <iostream>
#include <regex>

#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include "tools.hpp"
#include "EveWindow.hpp"
#include "ImageLibrary.hpp"
#include "Spaceship.hpp"
#include "ShipModule.hpp"

using std::cout;
using std::endl;

void ShipModule::moduleDetection_()
{
    hudX = spaceship_->getHudX();
    hudY = spaceship_->getHudY();

    moduleImage_ = new cv::Mat( *eveWindow_->get_ewImagePtr(),
                               cv::Rect( hudX+offsetX_+stepX_*modulePosition_,
                                         hudY+offsetY_,
                                         moduleAreaSize_,
                                         moduleAreaSize_ )
                                );

    double resCorrelationLevel = -1,
           maxCorrelationLevel = -1;

    for( int i = 0; i < modulesImageLibrary_->getQuanty(); ++i )
    {
        getFragmentLocation( moduleImage_, modulesImageLibrary_->getImagePtr(i), 0.7 , &resCorrelationLevel);
        if ( resCorrelationLevel > maxCorrelationLevel)
        {
            maxCorrelationLevel = resCorrelationLevel;
            imageNum_ = i;
        }
    }

    if ( maxCorrelationLevel > 0.7 )
    {
        moduleStatus_ = inactive;
        moduleName_ = std::regex_replace( modulesImageLibrary_->getImageFileName( imageNum_ ),
                                         std::regex( modulesImageLibrary_->getFileNameFilter() ),
                                         "" );
    }
    else {
        moduleStatus_ = absent;
        moduleName_ = "Empty";
        imageNum_ = -1;
    }

    delete moduleImage_;
}

void ShipModule::refresh()
{

    if ( refreshCounter_ % refreshDevider_ == 0 ) moduleDetection_();
    ++refreshCounter_;

    if ( moduleStatus_ == absent ) return;

    int moduleX = hudX+offsetX_+stepX_*modulePosition_,
        moduleY = hudY+offsetY_;

    if ( debugMode() )
    {
        cv::rectangle( *eveWindow_->get_ewImagePtr(),
                       cv::Point( moduleX-1, moduleY-1 ),
                       cv::Point( moduleX+moduleAreaSize_+1, moduleY+moduleAreaSize_+1 ),
                       cv::Scalar( 255, modulePosition_*32, 255-modulePosition_*32 ) );
    }

   int activitySensorX = moduleX + moduleAreaSize_/2 - 1,
       activitySensorY = moduleY + moduleAreaSize_ ,
       statusSensorX = activitySensorX + 2,
       statusSensorY = activitySensorY;

   int activitySensorGlow = eveWindow_->getSubpixR( activitySensorX , activitySensorY )
                          + eveWindow_->getSubpixG( activitySensorX , activitySensorY )
                          + eveWindow_->getSubpixB( activitySensorX , activitySensorY ) ;

   int statusSensorRed   = eveWindow_->getSubpixR ( statusSensorX, statusSensorY ),
       statusSensorGreen = eveWindow_->getSubpixG ( statusSensorX, statusSensorY );

    if ( activitySensorGlow < 280 ) ++statusVote_[inactive];
    else
    {
        if ( statusSensorGreen > statusSensorRed ) ++statusVote_[activeGreen];
        else ++statusVote_[activeRed];
    }

    int max = 0, maxpos = 0;
    for ( int i = 1; i < 4; ++i )
    {
        if ( statusVote_[i] > max )
        {
            max = statusVote_[i];
            maxpos = i;
        }
    }
    if ( max > maxVotes_ )
    {
        for ( int i = 1; i < 4; ++i)
            statusVote_[i] = 0;

        cout << "Module in position " << modulePosition_ << " now ";
        switch (maxpos) {
        case 1:
            moduleStatus_ = inactive;
            cout << "inactive";
            break;

        case 2:
            moduleStatus_ = activeGreen;
            cout << "active and green";
            break;

        case 3:
            moduleStatus_ = activeRed;
            cout << "active and RED";
            break;

        default:
            cout << "OMG! Fuckup in switch O_o";
            epicFail();
            break;
        }
        cout << endl;
    }
    
}

ShipModule::ShipModule(EveWindow *eveWindow, Spaceship* spaceship, int modulePosition, ImageLibrary* imageLibrary )
{
    eveWindow_ = eveWindow;
    spaceship_ = spaceship;
    modulePosition_ = modulePosition;
    modulesImageLibrary_ = imageLibrary;

    refreshCounter_ = modulePosition_ + 1;

    moduleDetection_();
    if ( moduleStatus_ != absent) cout << " INFO: " << moduleName_.c_str()
                                       << " found at position " << modulePosition_
                                       << endl;
    refresh();
}
