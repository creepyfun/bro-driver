#include <iostream>
#include <vector>

#include "tools.hpp"
#include "ImageLibrary.hpp"

ImageLibrary::ImageLibrary( std::string subdir, std::string filter)
{

    libPath_ = getResourcePath();
    if ( subdir != "")
        libPath_ = libPath_ + subdir + libPath_[libPath_.length()-1];

    if (filter != "")
        fileNameFilter_ = filter;

    fillFileList( &imageFileNames_, &libPath_, &fileNameFilter_);

    for( int i = 0; i < imageFileNames_.size(); i++)
    {
        loadableImages_.push_back( cv::imread( libPath_ + imageFileNames_[i] ) );
        if ( loadableImages_[i].empty() )
        {
            std::wcout << " FATAL: Can't load image " << imageFileNames_[i].c_str() << std::endl;
            epicFail();
        }
    }
}
