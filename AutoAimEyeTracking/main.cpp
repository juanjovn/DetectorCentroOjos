//AutoAim project
//Marksmanship Technology
//Juanjo Valiño @juanjovn
//Marcos Sabarís @kildos19
//www.aimsteady.com
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/core/core.hpp"

#include <iostream>
#include <cmath>
#include <chrono>

#include "findEyeCenter.h"

//GLOBAL VARIABLES
cv::String cascadePath = "./cascades/haarcascade_eye.xml";
cv::CascadeClassifier cascade;
cv::String windowTitle = "AutoAim Eye Tracking";
cv::String debugWindowTitle = "Detecting eye";
cv::String eyeCenterWindowTitle = "Algorithm eye center";
cv::Size frameSize (200, 300);
cv::Rect cropArea(477, 150, frameSize.width, frameSize.height);
int camNum, frameRate = 0;
double factorScale = 1.5;

//Do the conversion of time value and returns it in milliseconds
double clockToMilliseconds(clock_t ticks){
    // units/(units/time) => time (seconds) * 1000 = milliseconds
    return (ticks/(double)CLOCKS_PER_SEC)*1000.0;
}

int main(int argc, const char * argv[]) {
    
    cascade.load(cascadePath);
    cv::Mat frame;
    cv::Mat cropped;
    cv::Mat gray;
    cv::Mat blurred;
    cv::Mat zoomed;
    cv::Point drawPoint, centerPoint, prevPoint;
    int xCoord, yCoord = 0;
    std::string coords;
    std::vector<cv::Rect> eyes;
    clock_t deltaTime = 0;
    int frames = 0;
    
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
    
    //Display and move windows to the center
    cv::namedWindow(windowTitle);
    cv::namedWindow(debugWindowTitle);
    cv::namedWindow(eyeCenterWindowTitle);
    cv::moveWindow(windowTitle, 275 , 180);
    cv::moveWindow(eyeCenterWindowTitle, 590 , 180);
    cv::moveWindow(debugWindowTitle, 905 , 180);
    
    
    while (true)
    {
        clock_t beginFrame = clock();
        camera.read(frame);
        cv::resize(frame, cropped, cv::Size(1024, 576));
        cropped = cropped(cropArea);
        cv::cvtColor(cropped, gray, CV_RGB2GRAY);
        
        cv::GaussianBlur(gray, blurred, cv::Size(11, 25), 0, 0);
        
        cascade.detectMultiScale(blurred, eyes, 1.1, 1, 0 |cv::CASCADE_SCALE_IMAGE, cv::Size(75, 100));
        if (eyes.size()> 0)
        {
            //std::cout << eyes[0].x << std::endl;
            cv::rectangle(cropped, eyes[0], cv::Scalar(0,255,0), 2);
            centerPoint = findEyeCenter(blurred, eyes[0]);
            //------by Marcos----
            bool drawRequired=false;
            cv::Point prueba;
            prueba.x=eyes[0].x;
            prueba.y=eyes[0].y;
            cv::circle(cropped,prueba,4,cv::Scalar(255,0,0),2);
            //------end by Marcos
            //////////////////////////////////////////
            double distX = abs(prevPoint.x - centerPoint.x);
            double distY = abs(prevPoint.y - centerPoint.y);
            /*if( distX <= 10 && distY <= 10 && ((centerPoint.y+eyes[0].y) - eyes[0].y)>5 )
            {
                centerPoint = prevPoint;
                drawRequired=true;
            }
            else 
            {
                prevPoint = centerPoint;
                drawRequired=false;
            }*/
            //std::cout << "Y ojo = " << centerPoint.y+eyes[0].y << " Y test = " << prueba.y <<" Distancia = " << (centerPoint.y - prueba.y) << std::endl;
            
            //std::cout << distX << ", " << distY << std::endl;
            //////////////////////////////////////////
//            if(drawRequired){
                drawPoint.x = centerPoint.x + eyes[0].x - 2; //The numbers subtracted at the end is only for manual correction as the point appears to be a little bit shifted to the right
                drawPoint.y = centerPoint.y + eyes[0].y - 3;
                //Point where the circle will be drawed
                cv::circle(cropped, drawPoint, 4, cv::Scalar(255, 0, 255), 2);
//            }
            //cv::circle(cropped, drawPoint, 10, cv::Scalar(0, 0, 255), 2); //testing
            xCoord = abs(drawPoint.x) - 100;
            yCoord = 150 - abs(drawPoint.y);
            coords = std::to_string(xCoord);
            coords += ", ";
            coords += std::to_string(yCoord);
            //cv::rectangle(gray, eyes[1], cv::Scalar(0,255,0), 2);
        }
        //cv::resize(cropped, zoomed, cv::Size(540, 540));
        cv::resize(cropped, cropped, cv::Size(frameSize.width * factorScale, frameSize.height * factorScale));
        cv::resize(blurred, blurred, cv::Size(frameSize.width * factorScale, frameSize.height * factorScale));
        cv::resize(gray, gray, cv::Size(frameSize.width * factorScale, frameSize.height * factorScale));
        //Testing clocks and time for frame counting and rendering time
        cv::putText(cropped, std::to_string(frameRate), cv::Point(180, 295) * factorScale, cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 255),2);
        cv::arrowedLine(cropped, cv::Point(100,300) * factorScale, cv::Point(100,0) * factorScale, cv::Scalar(0, 255, 255));
        cv::arrowedLine(cropped, cv::Point(0,150) * factorScale, cv::Point(200,150) * factorScale, cv::Scalar(0, 255, 255));
        cv::putText(cropped, "X", cv::Point(0, 145) * factorScale, cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 255, 255),2);
        cv::putText(cropped, "Y", cv::Point(103, 295) * factorScale, cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 255, 255),2);
        cv::putText(cropped, coords, cv::Point(0, 290) * factorScale, cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 255),2);
        imshow(windowTitle, cropped);
        clock_t endFrame = clock();
        deltaTime += endFrame - beginFrame;
        frames ++;
        
        if( clockToMilliseconds(deltaTime)>1000.0){ //every second
            frameRate = (double)frames*0.5 +  frameRate*0.5; //more stable
            std::cout << frameRate << " <------------- FPS" << std::endl;
            frames = 0;
            deltaTime -= CLOCKS_PER_SEC;
        }
        
        imshow(debugWindowTitle, blurred);
        imshow(eyeCenterWindowTitle, gray);
        
        if (cv::waitKey(1) >= 0)
            break;
    }
    
    
    std::cout << "*** END OF PROGRAM ***\n*** Press ENTER to exit ***\n";
    std::cin.get();
    
    
    
    return 0;
}
