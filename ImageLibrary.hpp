#include <iostream>
#include <vector>

#include <opencv2/opencv.hpp>

class ImageLibrary
{
public:
    ImageLibrary( std::string subdir="", std::string filter="" );
    int getQuanty() { return imageFileNames_.size(); }
    std::string getImageFileName( int num ) { return imageFileNames_[num]; }
    cv::Mat* getImagePtr( int num ) { return &loadableImages_[num]; }
    std::string getFileNameFilter() { return fileNameFilter_; }

private:
    std::string libPath_;
    std::string fileNameFilter_ = "\\.bmp$";
    std::vector <cv::Mat> loadableImages_;
    std::vector <std::string> imageFileNames_;
};
