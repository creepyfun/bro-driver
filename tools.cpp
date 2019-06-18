#include <sys/types.h>
#include <dirent.h>

#include <iostream>
#include <regex>

#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

using std::cout;
using std::endl;

std::string getResourcePath( char* argv0 = nullptr )
{
    char path[4096] = { 0 };
    char const* subdir = "res";
    static std::string resourceLibPath = "";

    if ( argv0 != nullptr )
    {
    strcpy( path, argv0 );

        int pathLen = strlen( path );
        while ( path[pathLen] != '/'  &&  path[pathLen] != '\\'  &&  pathLen != 0 )
        {
            path[pathLen] = 0;
            --pathLen;
        }

        resourceLibPath.append( path ).append( subdir ).append( &path[pathLen] );
        cout << " INFO: Resource library path: " << resourceLibPath.c_str() << endl;
    }

    return resourceLibPath;
}

cv::Point getFragmentLocation( cv::Mat* image, cv::Mat* fragment,
                               double requiredCorrelationLevel = 0.8,
                               double* obtainedCorrelationLevel = nullptr )
{

    double minVal = 0, maxVal = 0;
    cv::Point minLoc( -1, -1), maxLoc( -1, -1), failMatch( -1, -1);
    cv::Mat matchResult;


    cv::matchTemplate( *image, *fragment, matchResult, cv::TM_CCOEFF_NORMED );
    cv::minMaxLoc( matchResult, &minVal, &maxVal, &minLoc, &maxLoc );
//    cout << maxVal << std::endl;

    if ( obtainedCorrelationLevel != nullptr ) *obtainedCorrelationLevel = maxVal;
    if ( maxVal < requiredCorrelationLevel ) return failMatch;
    return maxLoc;
}

cv::Scalar randomColorSet()
{
    return cv::Scalar( rand()%256, rand()%256, rand()%256 );
}

void playSoundFile ( std::string soundFileName )
{
    std::string syscmd = "aplay -q ";
    std::system( syscmd.append( getResourcePath() ).append( soundFileName ).c_str() );
}

void epicFail()
{
    playSoundFile("fatal_error.wav");
    abort();
}

void fillFileList( std::vector <std::string>* fileList, std::string* path, std::string* filter = nullptr )
{
    std::regex *reFilter;
    if ( filter == nullptr ) reFilter = new std::regex(".*");
    else reFilter = new std::regex( filter->c_str() );

    DIR *dir = opendir( path->c_str() );
    if ( dir == nullptr )
    {
        cout << " FATAL: Can't open '" << path << "' !" << endl;
        epicFail();
    }

    dirent64 *currentDirEntry = nullptr;
    while ( (currentDirEntry = readdir64( dir )) != nullptr )
    {
        std::string currentFileName(currentDirEntry->d_name);
        if ( std::regex_search( currentFileName.begin(), currentFileName.end(), *reFilter ) )
            fileList->push_back( currentFileName );
    }
 delete reFilter;
}

bool debugMode( bool revertFlag = false )
{
    static bool mode = false;
    if ( revertFlag ) mode = !mode;
    return mode;
}
