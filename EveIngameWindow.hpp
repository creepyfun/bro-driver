#include <opencv2/opencv.hpp>

class EveWindow;

class EveIngameWindow
{
public:
    EveIngameWindow(EveWindow* eveWindow,
                    std::string fileNameFragmentTopLeft,
                    std::string fileNameFragmentBottomRight);

    bool recognitionProblemPresent() { return recognitionProblem_; }

protected:
    bool recognitionProblem_ = false;

    unsigned int refreshCounter_ = 1;
    unsigned int refreshDevider_ = 3;

    int x_ = -1;
    int y_ = -1;
    int width_ = -1;
    int height_ = -1;

    EveWindow* eveWindow_ =  nullptr;
    cv::Mat* ingameWindowImage_ = nullptr;

    cv::Mat fragmentTopLeft_;
    cv::Mat fragmentBottomRight_;

    bool needReFindWindow_();
    void findWindowLocation_();
};
