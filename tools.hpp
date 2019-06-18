#include <sys/types.h>
#include <dirent.h>

#include <iostream>

#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>

std::string getResourcePath( char* argv0 = nullptr );
cv::Point getFragmentLocation( cv::Mat* image, cv::Mat* fragment,
                               double requiredCorrelationLevel = 0.8,
                               double* obtainedCorrelationLevel = nullptr );
cv::Scalar randomColorSet();
void playSoundFile( std::string soundFileName ); // Linux only
void epicFail();
void fillFileList( std::vector <std::string>* fileList, // Linux only
                   std::string* path,
                   std::string* filter = nullptr );
void epicFail();
bool debugMode( bool revertFlag = false );

