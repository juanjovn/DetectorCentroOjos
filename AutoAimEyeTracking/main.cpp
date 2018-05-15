#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <iostream>
#include <cmath>

#include "findEyeCenter.h"

//GLOBAL VARIABLES
cv::String cascadePath = "./cascades/haarcascade_eye.xml";
cv::CascadeClassifier cascade;
cv::String windowTitle = "Auto Aim Eye Tracking";
cv::Rect cropArea(477, 150, 200, 300);
int camNum = 0;

int main(int argc, const char * argv[]) {
    
    cascade.load(cascadePath);
    cv::Mat frame;
    cv::Mat cropped;
    cv::Mat gray;
    cv::Mat blurred;
    cv::Mat zoomed;
    cv::Point drawPoint, centerPoint;
    int xCoord, yCoord = 0;
    std::string coords;
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
        cv::arrowedLine(cropped, cv::Point(100,300), cv::Point(100,0), cv::Scalar(0, 255, 255));
        cv::arrowedLine(cropped, cv::Point(0,150), cv::Point(200,150), cv::Scalar(0, 255, 255));
        cv::putText(cropped, "X", cv::Point(0, 145), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 255, 255),2);
        cv::putText(cropped, "Y", cv::Point(103, 295), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 255, 255),2);
        cv::GaussianBlur(gray, blurred, cv::Size(11, 25), 0, 0);
        cascade.detectMultiScale(blurred, eyes, 1.1, 1, 0 |cv::CASCADE_SCALE_IMAGE, cv::Size(75, 100));
        if (eyes.size()> 0)
        {
        //std::cout << eyes[0].x << std::endl;
        cv::rectangle(cropped, eyes[0], cv::Scalar(0,255,0), 2);
        centerPoint = findEyeCenter(blurred, eyes[0]);
        //Point where the circle will be drawed
        drawPoint.x = centerPoint.x + eyes[0].x - 3; //The numbers subtracted at the end is only for manual correction as the point appears to be a little bit shifted to the right
        drawPoint.y = centerPoint.y + eyes[0].y + 3;
        cv::circle(cropped, drawPoint, 4, cv::Scalar(255, 0, 255), 2);
        xCoord = abs(drawPoint.x) - 100;
        yCoord = 150 - abs(drawPoint.y);
        coords = std::to_string(xCoord);
        coords += ", ";
        coords += std::to_string(yCoord);
        cv::putText(cropped, coords, cv::Point(0, 290), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 255),2);
        //cv::rectangle(gray, eyes[1], cv::Scalar(0,255,0), 2);
        }
        //cv::resize(cropped, zoomed, cv::Size(540, 540));
        imshow(windowTitle, cropped);
        if (cv::waitKey(1) >= 0)
            break;
    }
    
    
    std::cout << "*** END OF PROGRAM ***\n*** Press ENTER to exit ***\n";
    std::cin.get();
    
    
    
    return 0;
}
