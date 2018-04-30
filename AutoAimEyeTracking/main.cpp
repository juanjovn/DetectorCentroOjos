#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <iostream>

//GLOBAL VARIABLES
cv::String cascadePath = "./cascades/haarcascade_eye.xml";
cv::CascadeClassifier cascade;
cv::String windowTitle = "Auto Aim Eye Tracking";

int main(int argc, const char * argv[]) {
    
    cascade.load(cascadePath);
    if (cascade.empty())
    {
        std::cout << "Error loading cascade\n";
        return -1;
    }
    
    cv::namedWindow (windowTitle, CV_WINDOW_NORMAL);
    cv::VideoCapture camera;
    
    
    return 0;
}
