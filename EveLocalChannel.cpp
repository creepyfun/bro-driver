#include <iostream>

#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include "tools.hpp"
#include "EveWindow.hpp"
#include "EveLocalChannel.hpp"

using std::wcout;
using std::endl;



void EveLocalChannel::refresh()
{
    if ( needReFindWindow_() ) findWindowLocation_();

    if ( x_ == -1 || y_ == -1 ) return;

    if ( debugMode() )
    {
        cv::rectangle( *eveWindow_->get_ewImagePtr(),
                       cv::Point(x_, y_),
                       cv::Point(x_+width_, y_+height_),
                       randomColorSet() );
    }

    ingameWindowImage_ = new cv::Mat( *eveWindow_->get_ewImagePtr(), cv::Rect( x_, y_, width_, height_ ) );

    cv::Point enemySignLoc(0,0);

    int i = 0;
    for (; i < 4; i++ )
    {
        enemySignLoc = getFragmentLocation ( ingameWindowImage_, &enemySign_[i], 0.97 );
            if ( enemySignLoc.x != -1 || enemySignLoc.y != -1 )
            {
                dangerInLocal_ = true;
                break;
            }
     }
     if ( i == 4 ) dangerInLocal_ = false;
     delete ingameWindowImage_;
}


EveLocalChannel::EveLocalChannel( EveWindow* eveWindow,
                                  std::string fileNameFragmentTopLeft,
                                  std::string fileNameFragmentBottomRight,
                                  int refreshCount, int refreshDevider )
                                :
                                  EveIngameWindow (eveWindow, fileNameFragmentTopLeft, fileNameFragmentBottomRight )
{

    refreshCount_ = refreshCount;
    refreshDevider_ = refreshDevider;

    for ( int i = 0; i<numImg_; i++)
    {
        enemySign_[i] = cv::imread( getResourcePath() + imageFileNames_[i] );
        if( enemySign_[i].empty() )
        {
            wcout << " FATAL: Can't load image " << imageFileNames_[i].c_str() << endl;
            epicFail();
        }
    }
    findWindowLocation_();
    wcout << " INFO: Local channel area size " << width_ << "x"<< height_ << "." << endl;
    refresh();
}

