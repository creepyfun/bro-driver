#include <iostream>

#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include "tools.hpp"
#include "EveWindow.hpp"
#include "EveIngameWindow.hpp"

using std::wcout;
using std::endl;

bool EveIngameWindow::needReFindWindow_()
{
    bool needFind = false;

    if ( refreshCount_ % refreshDevider_ == 0 )
        needFind = true;
    refreshCount_++;

    return needFind;
}

void EveIngameWindow::findWindowLocation_()
{
    recognitionProblem_ = false;

    cv::Point fragmentTopLeftLocation;
    fragmentTopLeftLocation = getFragmentLocation ( eveWindow_->get_ewImagePtr(), &fragmentTopLeft_ );

    if ( x_ != fragmentTopLeftLocation.x || y_ != fragmentTopLeftLocation.y )
    {
        x_ = fragmentTopLeftLocation.x;
        y_ = fragmentTopLeftLocation.y;
    }

    if ( x_ == -1 || y_ == -1 )
    {
        recognitionProblem_ = true;
        return;
    }

    int clippingZoneWigth  = eveWindow_->get_ewImagePtr()->cols / 3,
        clippingZoneHeight = eveWindow_->get_ewImagePtr()->rows - y_;

    if ( (x_ + clippingZoneWigth) > eveWindow_->get_ewImagePtr()->cols )
        clippingZoneWigth = eveWindow_->get_ewImagePtr()->cols - x_;

    cv::Mat clippingZone( *eveWindow_->get_ewImagePtr(), cv::Rect( x_, y_, clippingZoneWigth, clippingZoneHeight ) );
    cv::Point fragmentBottomRightLocation;
    fragmentBottomRightLocation = getFragmentLocation ( &clippingZone, &fragmentBottomRight_ );

    if ( fragmentBottomRightLocation.x == -1 || fragmentBottomRightLocation.y == -1 )
    {
        recognitionProblem_ = true;
        return;
    }
    width_  = fragmentBottomRightLocation.x + fragmentBottomRight_.cols;
    height_ = fragmentBottomRightLocation.y + fragmentBottomRight_.rows;

    if ( width_ < 192 || height_ < 192 )
    {
        recognitionProblem_ = true;
        return;
    }
}

EveIngameWindow::EveIngameWindow( EveWindow* eveWindow,
                                  std::string fileNameFragmentTopLeft,
                                  std::string fileNameFragmentBottomRight )
{
    eveWindow_ = eveWindow;

    fragmentTopLeft_ = cv::imread( getResourcePath() + fileNameFragmentTopLeft );
    if( fragmentTopLeft_.empty() )
    {
        wcout << " FATAL: Can't load image " << fileNameFragmentTopLeft.c_str() << endl;
        epicFail();
    }

    fragmentBottomRight_ = cv::imread( getResourcePath() + fileNameFragmentBottomRight );
    if( fragmentBottomRight_.empty() )
    {
        wcout << " FATAL: Can't load image " << fileNameFragmentBottomRight.c_str() << endl;
        epicFail();
    }

}
