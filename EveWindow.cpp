#include <QString> //TODO: надо избавиться от него, переписав EveWindow::findEveWindow и EveWindow::getWindowName
#include <iostream>

#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include <X11/Xlib.h>
#include <X11/Xatom.h>

#include "tools.hpp"
#include "EveWindow.hpp"

using std::wcout;
using std::endl;

void EveWindow::refresh()
{
    if ( ewImage_ != NULL )
    {
        delete ewImage_;
        ewImage_ = NULL;
    }

    if ( xWindowImage_ != NULL )
    {
        XFree( xWindowImage_->data );
        XFree( xWindowImage_ );
        xWindowImage_ = NULL;
    }

    xWindowImage_ = XGetImage( display_, windowId_, 0, 0, width_, height_, AllPlanes, ZPixmap );
    if ( xWindowImage_ == NULL )
    {
        wcout << " FATAL: XGetImage failed! Aborting execution..." << endl;;
        epicFail();
    }

    // Каждый 4-й байт в windowImage->data - канал прозрачности, его необходимо убрать
    for( unsigned long i = 3, j = 1; i< width_*height_*3 ; i++ )
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
    display_ = XOpenDisplay( NULL );
    if ( display_ == NULL )
    {
        wcout << " FATAL: Can't open display!" << endl;
        epicFail();
    }

    windowId_ = findEveWindow_ ( windowName );
    if ( windowId_ == 0)
    {
        wcout << " FATAL: Can't find window '" << windowName << "'!" << endl;
        epicFail();
    }

    if( !XGetWindowAttributes( display_, windowId_, &xWindowAttributes_ ) )
    {
        wcout << " FATAL: Can't get attributes for window '" << windowName << "'!" << endl;
        epicFail();
    }

    width_ = xWindowAttributes_.width;
    height_ = xWindowAttributes_.height;

    wcout << " INFO: Use window " << std::hex << windowId_ << std::dec << " with geometry "
          << width_ << "x" << height_ << " for further work" << endl;

    refresh();
    wcout << " INFO: Color depth - " << xWindowAttributes_.depth << ", "
          << xWindowImage_->bits_per_pixel << " bits per pixel" << endl;
}

char* EveWindow::getWindowName_( Window win ) {

    Atom actualType;
    int format;
    unsigned long count, bytesAfter;
    unsigned char* name = NULL;

    Status status = XGetWindowProperty( display_, win, XInternAtom( display_, "_NET_WM_NAME", False ),
                                        0L, ~0L, False, XInternAtom( display_, "UTF8_STRING", False ),
                                        &actualType, &format, &count, &bytesAfter, &name );
    if( status != Success ) {

        return NULL;
    }

    if( name == NULL ) {

        Status status = XGetWindowProperty( display_, win, XInternAtom( display_, "WM_NAME", False ),
                                            0L, ~0L, False, AnyPropertyType,
                                            &actualType, &format, &count, &bytesAfter, &name );

        if( status != Success ) {

            return NULL;
        }
    }

    return (char*) name;
}

Window EveWindow::findEveWindow_( char* windowName )
{
    QString eveWindowName( windowName );
    Window eveWindow = 0;
    unsigned long  winCounter = 0;
    Atom actualType;
    int format;
    unsigned long bytesAfter;
    unsigned char* list = NULL;


    Status status = XGetWindowProperty( display_, DefaultRootWindow( display_ ),
                                        XInternAtom( display_, "_NET_CLIENT_LIST", False ),
                                        0L, ~0L, False, XA_WINDOW, &actualType, &format,
                                        &winCounter, &bytesAfter, &list );

    if( status != Success )
    {
        wcout << " ERROR: Can't get root window property!" << endl;
        return 0;
    }

    wcout << " INFO: Searching window with title '" << windowName << "'..."<< endl;

    Window* windowsList = (Window*) list;

        for ( ulong i = 0; i < winCounter; i++ )
        {
            Window currWindow = windowsList[i];
            wcout << " INFO: Window "<< std::hex << currWindow << std::dec;

            if( char* name = getWindowName_( currWindow ) )
            {
                wcout << " -> '" << QString::fromUtf8( name ).toStdWString() << "'";

                if( eveWindowName == QString::fromUtf8( name ) )
                {
                        wcout << " <- MATCH FOUND!";
                        eveWindow = currWindow;
                }

                XFree( name );
            }

        wcout << endl;
        }
  return eveWindow;
}
