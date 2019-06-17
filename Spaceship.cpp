#include <iostream>
#include <regex>

#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include "tools.hpp"
#include "ImageLibrary.hpp"
#include "EveWindow.hpp"
#include "ShipModule.hpp"
#include "Spaceship.hpp"

using std::wcout;
using std::endl;

void Spaceship::findHud_()
{
    double resHudCorrelationLevel = -1;
    double resUndockButtonCorrelationLevel = -1;
    cv::Point hudDetectorLocation(-1,-1);
    cv::Point undockButtonLocation(-1,-1);

    hudDetectorLocation = getFragmentLocation( eveWindow_->get_ewImagePtr(), &hudDetectorImage_, 0.6, &resHudCorrelationLevel );
    undockButtonLocation = getFragmentLocation( eveWindow_->get_ewImagePtr(), &undockButtonImage_, 0.6, &resUndockButtonCorrelationLevel );
    if ( ( hudDetectorLocation.x == -1 || hudDetectorLocation.y == -1 ) && ( undockButtonLocation.x == -1 || undockButtonLocation.y == -1 ) )
    {
        wcout << " WARN: Can't find HUD and Undock button simultaneously. Strange situation..." << endl
              << " DEBUG: Correlation level for HUD:" << resHudCorrelationLevel << endl
              << " DEBUG: Correlation level for Undock Button:" << resUndockButtonCorrelationLevel << endl;
        recognitionProblem_ = true;
        return;
    }
    recognitionProblem_ = false;
    if ( resUndockButtonCorrelationLevel > resHudCorrelationLevel )
    {
        undockButtonX_ = undockButtonLocation.x;
        undockButtonY_ = undockButtonLocation.y;
        inDock_ = true;
        return;
    }
    hudX_ = hudDetectorLocation.x + offsetDetectorX_;
    hudY_ = hudDetectorLocation.y + offsetDetectorY_;
    inDock_ = false;
}

int Spaceship::detectDigit_( int pos )
{

    cv::Mat sector( *eveWindow_->get_ewImagePtr(),
                    cv::Rect( hudX_+digitX_[pos], hudY_+digitY_[pos],
                              digitWidth_, digitHeight_) );

    double resCorrelationLevel = -1,
           maxCorrelationLevel = -1;
    unsigned int imageNum;

    for( unsigned int i = 0; i < digitsImageLibrary_->getQuanty(); i++ )
    {
        getFragmentLocation( &sector, digitsImageLibrary_->getImagePtr(i), 0.6 , &resCorrelationLevel);
        if ( resCorrelationLevel > maxCorrelationLevel)
        {
            maxCorrelationLevel = resCorrelationLevel;
            imageNum = i;
        }
    }

    if ( maxCorrelationLevel < 0.6 )
        return 0;

    //файлы названы 0.bmp, 1.bmp и т.д.
    std::string clearedFileName = std::regex_replace( digitsImageLibrary_->getImageFileName( imageNum ),
                                     std::regex( digitsImageLibrary_->getFileNameFilter() ), "" );
    return std::stoi( clearedFileName );
}

void Spaceship::refresh() {

    if ( refreshCount_ % refreshDevider_ == 0 )
        findHud_();
    refreshCount_++;

    if ( !docked() )
    {

    int pShield, pArmor, pStructure, pSum;

        pShield = detectDigit_( 2 ) * 100
                + detectDigit_( 1 ) * 10
                + detectDigit_( 0 );

        pArmor = detectDigit_( 5 ) * 100
               + detectDigit_( 4 ) * 10
               + detectDigit_( 3 );

        pStructure = detectDigit_( 8 ) * 100
                   + detectDigit_( 7 ) * 10
                   + detectDigit_( 6 );

        pSum = pShield + pArmor + pStructure;

        if ( pSum > 0 && pSum < 301)
        {
            percentShield_ = pShield;
            percentArmor_ = pArmor;
            percentStructure_ = pStructure;
        }
    }

    // дело за малым - придумать как добыть данные капаситора :)

    if ( debugMode() )
    {
        cv::rectangle( *eveWindow_->get_ewImagePtr(),
                       cv::Point( hudX_ , hudY_ ),
                       cv::Point( hudX_+hudWidth_ , hudY_+hudHeight_ ),
                       cv::Scalar( 0, 255, 255 ) );

        wcout << " DEBUG: Shield/Armor/Structure = "
              << percentShield_  << "/"
              << percentArmor_ << "/"
              << percentStructure_ << endl;
        if ( docked() )
            wcout << " DEBUG: ship docked!" << endl;
    }

    for( int i = 0; i < 8; i++)
        module_[i]->refresh();
}

Spaceship::Spaceship(EveWindow* eveWindow)
{
    eveWindow_ = eveWindow;

    hudDetectorImage_ = cv::imread( getResourcePath() + hudDetectorFileName_ );
    if ( hudDetectorImage_.empty() )
    {
        wcout << " FATAL: Can't load image " << hudDetectorFileName_.c_str() << endl;
        epicFail();
    }

    undockButtonImage_ = cv::imread( getResourcePath() + undockButtonFileName_ );
    if ( undockButtonImage_.empty() )
    {
        wcout << " FATAL: Can't load image " << undockButtonFileName_.c_str() << endl;
        epicFail();
    }
    undockButtonWidth_ = undockButtonImage_.cols;
    undockButtonHeight_ = undockButtonImage_.rows;

    findHud_();

    wcout << " INFO: HUD coordinates (" << hudX_ << ", " << hudY_ << "),"
          << " size " << hudWidth_ << "x" << hudHeight_ << "." << endl ;
    if ( docked() )
        wcout << " INFO: Spaceship in dock." << endl ;

    digitsImageLibrary_ = new ImageLibrary( "huddigits" );

    modulesImageLibrary_ = new ImageLibrary( "modules" );
    for( int i = 0; i < 8; i++)
        module_[i] = new ShipModule( eveWindow_, this, i, modulesImageLibrary_ );
    refresh();
}

Spaceship::~Spaceship()
{

    delete digitsImageLibrary_;

    for( int i = 0; i < 8; i++ )
        delete module_[i];

    delete modulesImageLibrary_;
}
