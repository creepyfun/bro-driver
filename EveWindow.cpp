#include <iostream>
#include <cstring>

#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include <X11/Xlib.h>
#include <X11/Xatom.h>

#include "tools.hpp"
#include "EveWindow.hpp"

using std::cout;
using std::endl;

void EveWindow::refresh()
{
    if ( ewImage_ != nullptr )
    {
        delete ewImage_;
        ewImage_ = nullptr;
    }

    if ( xWindowImage_ != nullptr )
    {
        XFree( xWindowImage_->data );
        XFree( xWindowImage_ );
        xWindowImage_ = nullptr;
    }

    xWindowImage_ = XGetImage( display_, windowId_, 0, 0, width_, height_, AllPlanes, ZPixmap );
    if ( xWindowImage_ == nullptr )
    {
        cout << " FATAL: XGetImage failed! Aborting execution..." << endl;;
        epicFail();
    }

    // Каждый 4-й байт в windowImage->data - канал прозрачности, его необходимо убрать
    for( unsigned long i = 3, j = 1; i< width_*height_*3 ; ++i )
    {
        j =  i/3;
        xWindowImage_->data[i] = xWindowImage_->data[i+j];
    }

    ewImage_ = new cv::Mat( height_, width_, CV_8UC3, xWindowImage_->data );
}

EveWindow::~EveWindow()
{
    delete ewImage_;
    XFree( xWindowImage_->data );
    XFree( xWindowImage_ );
    XCloseDisplay( display_ );
}

EveWindow::EveWindow( char* windowName )
{
    display_ = XOpenDisplay( nullptr );
    if ( display_ == nullptr )
    {
        cout << " FATAL: Can't open display!" << endl;
        epicFail();
    }

    windowId_ = findEveWindow_ ( windowName );
    if ( windowId_ == 0)
    {
        cout << " FATAL: Can't find window '" << windowName << "'!" << endl;
        epicFail();
    }

    if( !XGetWindowAttributes( display_, windowId_, &xWindowAttributes_ ) )
    {
        cout << " FATAL: Can't get attributes for window '" << windowName << "'!" << endl;
        epicFail();
    }

    width_ = xWindowAttributes_.width;
    height_ = xWindowAttributes_.height;

    cout << " INFO: Use window " << std::hex << windowId_ << std::dec << " with geometry "
          << width_ << "x" << height_ << " for further work" << endl;

    refresh();
    cout << " INFO: Color depth - " << xWindowAttributes_.depth << ", "
          << xWindowImage_->bits_per_pixel << " bits per pixel" << endl;
}

char* EveWindow::getWindowName_( Window win ) {

    Atom actualType;
    int format;
    unsigned long count, bytesAfter;
    unsigned char* name = nullptr;
    Status status = XGetWindowProperty( display_, win, XInternAtom( display_, "WM_NAME", False ),
                                            0L, ~0L, False, AnyPropertyType,
                                            &actualType, &format, &count, &bytesAfter, &name );

    if( status != Success ) return nullptr;

    return (char*) name;
}

Window EveWindow::findEveWindow_( char* windowName )
{
    Window eveWindow = 0;
    unsigned long  winCounter = 0;
    Atom actualType;
    int format;
    unsigned long bytesAfter;
    unsigned char* list = nullptr;


    Status status = XGetWindowProperty( display_, DefaultRootWindow( display_ ),
                                        XInternAtom( display_, "_NET_CLIENT_LIST", False ),
                                        0L, ~0L, False, XA_WINDOW, &actualType, &format,
                                        &winCounter, &bytesAfter, &list );

    if( status != Success )
    {
        cout << " ERROR: Can't get root window property!" << endl;
        return 0;
    }

    cout << " INFO: Searching window with title '" << windowName << "'..."<< endl;

    Window* windowsList = (Window*) list;
    Window  currWindow = 0;
    char*  currWinName = nullptr;

        for ( unsigned long i = 0; i < winCounter; ++i )
        {
            currWindow = windowsList[i];
            cout << " INFO: Window "<< std::hex << currWindow << std::dec;

            currWinName = getWindowName_( currWindow );
            if( currWinName != nullptr )
            {
                cout << " -> '" << currWinName  << "'";

                if( strcmp( windowName, currWinName ) == 0  )
                {
                        cout << " <- MATCH FOUND!";
                        eveWindow = currWindow;
                }

                XFree( currWinName );
            }
        cout << endl;
        }
  return eveWindow;
}
