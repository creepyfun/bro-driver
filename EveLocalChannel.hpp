#include <opencv2/opencv.hpp>
#include "EveIngameWindow.hpp"

class EveWindow;

class EveLocalChannel : public EveIngameWindow
{
public:
    EveLocalChannel( EveWindow* eveWindow,
                     std::string fileNameFragmentTopLeft,
                     std::string fileNameFragmentBottomRight,
                     int refreshCount, int refreshDevider );

    void refresh();
    bool dangerDetected() { return dangerInLocal_; }

private:
    bool dangerInLocal_ = true;

    cv::Mat enemySign_[4];
    static const int numImg_ = 4;
    std::string imageFileNames_[numImg_] = {"Local-minus.bmp",
                                            "Local-neutral.bmp",
                                            "Local-fracwar.bmp",
                                            "Local-offwar.bmp" };    
};
