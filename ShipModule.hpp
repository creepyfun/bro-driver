#include <iostream>

#include <opencv2/opencv.hpp>

class Spaceship;
class EveWindow;
class ImageLibrary;

enum ShipModuleStatus {absent, inactive, activeGreen, activeRed};

class ShipModule
{
public:
    ShipModule (EveWindow* eveWindow, Spaceship* spaceship, int modulePosition, ImageLibrary* imageLibrary );

    bool getModuleStatus() { return moduleStatus_; }
    std::string getModuleName() { return moduleName_; }

    void refresh();
//        void activate();
//        void deactivate();

private:
    ImageLibrary* modulesImageLibrary_;
    int imageNum_ = -1;

    unsigned int refreshCount_ = 1;
    unsigned int refreshDevider_ = 10;

    EveWindow* eveWindow_ =  NULL;
    Spaceship* spaceship_ = NULL;
    cv::Mat* moduleImage_ = NULL;
    std::string moduleName_ = "";

    int hudX = -1;
    int hudY = -1;

    int const offsetX_ = 256; // [256;34] - координаты первого модуля в HUD
    int const offsetY_ = 34;
    int const moduleAreaSize_ = 48; // Область модуля - квадрат 48x48
    int const stepX_ = 51; // каждый следующий модуль смещен на 51

    int modulePosition_ = -1;
    ShipModuleStatus moduleStatus_ = absent;
    int statusVote_[4] = { 0 };
    int const maxVotes_ = 2;

    void moduleDetection_();
};

