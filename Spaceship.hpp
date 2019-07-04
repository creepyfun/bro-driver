#include <iostream>

#include <opencv2/opencv.hpp>

class EveWindow;
class ImageLibrary;
class ShipModule;

class Spaceship
{
public:
    Spaceship( EveWindow* eveWindow );
    ~Spaceship();

    bool recognitionProblemPresent() { return recognitionProblem_; }
    bool docked() { return inDock_; }

    int getHudX() { return hudX_; }
    int getHudY() { return hudY_; }
    int getShieldStatus() { return percentShield_; }
    int getArmorStatus() { return percentArmor_; }
    int getStructureStatus() { return percentStructure_; }

    void refresh();
//        void undock();

private:
    bool recognitionProblem_ = false;

    unsigned int refreshCounter_ = 2;
    unsigned int refreshDevider_ = 3;

    int hudX_ = -1;
    int hudY_ = -1;
    int const hudWidth_ = 700; // размер области HUD - 700x200
    int const hudHeight_ = 200;
    int const offsetDetectorX_ = -250; // huddetetector.bmp - 25x45,
    int const offsetDetectorY_ = -80;  // первая точка должна детектиться в (250;80) HUD'а
    int undockButtonX_ = -1;
    int undockButtonY_ = -1;
    int undockButtonWidth_ = -1;
    int undockButtonHeight_ = -1;
    bool inDock_ = true;
    cv::Mat undockButtonImage_;
    cv::Mat hudDetectorImage_;
    std::string undockButtonFileName_ = "undockbutton.bmp";
    std::string hudDetectorFileName_ = "huddetector.bmp";

    int percentShield_ = -1;
    int percentArmor_ = -1;
    int percentStructure_ = -1;

    EveWindow* eveWindow_ = nullptr;
    int const digitX_[9] = { 19, 13,  7,   // координаты верхних левых углов цифр
                             19, 13,  7,   // процента остатка щитов, брони и структуры
                             19, 13,  7 }; // в HUD
    int const digitY_[9] = { 25, 25, 25,
                             38, 38, 38,
                             51, 51, 51 };
    int const digitWidth_ = 9;   //7
    int const digitHeight_ = 12; //10

    ImageLibrary* digitsImageLibrary_ = nullptr;
    ImageLibrary* modulesImageLibrary_ = nullptr;
    ShipModule* module_[8];

    void findHud_();
    int detectDigit_( int pos );
};

