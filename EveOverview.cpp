#include <iostream>

#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include "tools.hpp"
#include "EveWindow.hpp"
#include "EveOverview.hpp"

using std::wcout;
using std::endl;

void EveOverview::refresh()
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

    // тут быстренько детектим и мастерим список объектов в овервью

    delete ingameWindowImage_;
}

