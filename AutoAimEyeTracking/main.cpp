#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <iostream>

//GLOBAL VARIABLES
cv::String cascadePath = "./cascades/haarcascade_eye.xml";
cv::CascadeClassifier cascade;
cv::String windowTitle = "Auto Aim Eye Tracking";
cv::Rect cropArea(277, 150, 470, 276);
int camNum = 0;

int main(int argc, const char * argv[]) {
    
    cascade.load(cascadePath);
    cv::Mat frame;
    cv::Mat cropped;
    cv::Mat gray;
    cv::Mat blurred;
    std::vector<cv::Rect> eyes;
    if (cascade.empty())
    {
        std::cout << "Error loading cascade\n";
        return -1;
    }
    
    cv::namedWindow (windowTitle, CV_WINDOW_NORMAL);
    cv::VideoCapture camera(0);
    if (!camera.isOpened())
    {
        camera.open(1);
        camNum=1;
        if (!camera.isOpened())
        {
            camera.open(2);
            camNum=2;
            if (!camera.isOpened())
            {
                std::cout << "*** UNABLE TO OPEN ANY CAM ***\n*** Press ENTER to exit ***\n";
                std::cin.get();
                return -1;
            }
        }
        
    }
    
    while (true){
        camera.read(frame);
        cv::resize(frame, cropped, cv::Size(1024, 576));
        cropped = cropped(cropArea);
        cv::cvtColor(cropped, gray, CV_RGB2GRAY);
        cv::GaussianBlur(gray, blurred, cv::Size(11, 25), 0, 0);
        cascade.detectMultiScale(blurred, eyes, 1.1, 1, 0 |cv::CASCADE_SCALE_IMAGE, cv::Size(75, 100));
        if (eyes.size()> 0)
        {
        //std::cout << eyes[0].x << std::endl;
        cv::rectangle(gray, eyes[0], cv::Scalar(0,255,0), 2);
        //cv::rectangle(gray, eyes[1], cv::Scalar(0,255,0), 2);
        }
        imshow(windowTitle, gray);
        if (cv::waitKey(1) >= 0)
            break;
    }
    
    
    std::cout << "*** END OF PROGRAM ***\n*** Press ENTER to exit ***\n";
    std::cin.get();
    
    
    
    return 0;
}
