#include <sys/types.h>
#include <dirent.h>

#include <iostream>

#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>

std::string getResourcePath( const char* argv0 = nullptr );
cv::Point getFragmentLocation( const cv::Mat* image, const cv::Mat* fragment,
                               const double  requiredCorrelationLevel = 0.8,
                               double* obtainedCorrelationLevel = nullptr );
cv::Scalar randomColorSet();
void playSoundFile( const std::string soundFileName ); // Linux only
void epicFail();
void fillFileList( std::vector <std::string>* fileList, // Linux only
                   const std::string* path,
                   const std::string* filter = nullptr );
void epicFail();
bool debugMode( const bool revertFlag = false );

