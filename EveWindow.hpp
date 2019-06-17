#include <opencv2/opencv.hpp>
#include <X11/Xlib.h>

class EveWindow //Linux only
{
public:
    EveWindow( char* windowName );
    ~EveWindow();

    void refresh();
    cv::Mat* get_ewImagePtr() { return ewImage_; }
    unsigned char getSubpixR( int x, int y ) { return ewImage_->at<cv::Vec3b>( cv::Point( x, y ) )[2]; }
    unsigned char getSubpixG( int x, int y ) { return ewImage_->at<cv::Vec3b>( cv::Point( x, y ) )[1]; }
    unsigned char getSubpixB( int x, int y ) { return ewImage_->at<cv::Vec3b>( cv::Point( x, y ) )[0]; }
    void setSubpixR( int x, int y, unsigned char val ) { ewImage_->at<cv::Vec3b>( cv::Point( x, y ) )[2] = val; }
    void setSubpixG( int x, int y, unsigned char val ) { ewImage_->at<cv::Vec3b>( cv::Point( x, y ) )[1] = val; }
    void setSubpixB( int x, int y, unsigned char val ) { ewImage_->at<cv::Vec3b>( cv::Point( x, y ) )[0] = val; }

private:
    Display* display_ = NULL;
    Window windowId_ = 0;
    XWindowAttributes xWindowAttributes_;
    XImage* xWindowImage_ = NULL;

    int width_ = -1;
    int height_ = -1;
    cv::Mat* ewImage_ = NULL;

    Window findEveWindow_( char* windowName );
    char* getWindowName_( Window win );
};

