#include <opencv2/opencv.hpp>
#include "EveIngameWindow.hpp"

class EveWindow;

class EveOverview : public EveIngameWindow
{
public:
    EveOverview( EveWindow* eveWindow,
                 std::string fileNameFragmentTopLeft,
                 std::string fileNameFragmentBottomRight,
                 int refreshCount, int refreshDevider );

    void refresh();


private:


};
