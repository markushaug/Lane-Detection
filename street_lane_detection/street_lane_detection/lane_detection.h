#include <opencv2/core/core.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <vector>
#include <algorithm>
#include <math.h>
#define PI 3.1415926
using namespace std;
using namespace cv;


class LaneDetector
{
private:
    int houghVote = 200;
    bool showOriginal = 0;
    bool showCanny = 1;
    bool showHough = 1;
    bool showHoughP = 1;
    int newHeight = 320;
    int newWidth = 480;
    
public:
    Mat currFrame; //stores the upcoming frame
    
    
    LaneDetector(Mat startFrame)
    {
        
        currFrame = Mat(newHeight,newWidth,CV_8UC1,0.0);                        //initialised the image size to 320x480
        resize(startFrame, currFrame, currFrame.size());             // resize the input to required size
        
        
        
        //namedWindow("Contours"); // Canny
        //namedWindow("Detected Lines with Hough");
        //namedWindow("Detected Lines with HoughP");
        
        getLaneLines();
    }
    
    void setupDetector(int _newHeight, int _newWidth, int _houghVote, bool _showOriginal, bool _showCanny, bool _showHough, bool _showHoughP){
        this->newHeight = _newHeight;
        this->newWidth = _newWidth;
        this->houghVote = _houghVote;
        this->showOriginal = _showOriginal;
        this->showCanny = _showCanny;
        this->showHough = _showHough;
        this->showHoughP  = _showHoughP;
    }
    
    void getLaneLines(){
        
        Mat gray;
        cvtColor(currFrame, gray, COLOR_BGR2GRAY);
        
        // Region of interest
        int left(0),
            top(320/2),
            width(newWidth),
            height(newHeight/2);
        
        Rect roi(left, top, width, height);
        Mat imgROI = currFrame(roi);
        Scalar val = Scalar(255,255,255);
        copyMakeBorder(imgROI, imgROI, 2, 2, 2, 2, BORDER_CONSTANT, val);
        
        if(showOriginal){
            namedWindow("Original Image");
            imshow("Original Image", imgROI);
        }
        
        //GaussianBlur(imgROI ,imgROI,Size(5, 5), 0);
        
        // Canny algorithm
        Mat contours;
        Canny(imgROI, contours, 100,200);
        Mat contoursInv;
        threshold(contours, contoursInv, 128, 255,THRESH_BINARY_INV);
        
        if(showCanny){
            namedWindow("Contours");
            imshow("Contours", contoursInv);
        }
        
        // Hough transform
        vector<Vec2f> lines;
        if(houghVote < 1 or lines.size() < 2){
            // if this condition evaulates to true, all lines were lost --> reset the houghVote
            houghVote = 300;
        } else {
            /*
             Increment the next image by 25 if we found a few lines.
             This is to prevent missing any other lines that might appear in the next frame.
             But at the same time, we don't want to restart the feedback loop from the beginning.
             */
            houghVote+=25;
        }
        while(lines.size() < 4 && houghVote > 0){
            HoughLines(contours, lines, 1, PI/180, houghVote);
            houghVote -=5;
        }
        cout << "houghVote: " << houghVote << endl;
        Mat result(imgROI.size(), CV_8U, Scalar(255));
        imgROI.copyTo(result);
        
        // Draw the lines
        vector<Vec2f>::const_iterator it=lines.begin();
        Mat hough(imgROI.size(), CV_8U, Scalar(0));
        while(it!=lines.end()){
            float rho = (*it)[0];
            float theta = (*it)[1];
            if((theta > 0.09 && theta < 1.48) || (theta < 3.14 && theta > 1.66)){
                // so we actually removed vertical and horizontal lines
                
                // point of intersection of the line with first row
                Point pt1(rho/cos(theta),0);
                // point of intersection of the line with last row
                Point pt2((rho-result.rows*sin(theta))/cos(theta), result.rows);
                // draw a line
                line(result, pt1, pt2, Scalar(255,255,255), 1);
                line(hough, pt1, pt2, Scalar(255,255,255),1);
            }
            cout << "line: (" << rho << "," << theta << ")" << endl;
            ++it;
        };
        
        if(showHough){
            namedWindow("Detected lines with Hough");
            imshow("Detected lines with Hough", hough);
        }
        
        // probabilistic Hough
        
    }
    
    void nextFrame(Mat &nxt)
    {
        resize(nxt ,currFrame, currFrame.size()); //resizing the input image for faster processing
        getLaneLines();
    }
    
    
    
    Mat hough(Mat frame){
        Mat dst, cdst;
        
        // STEP 1:  Apply the transform
        vector<Vec2f> lines; // A vector that will store the parameters (rho, theta) of the detected lines
        double rho = 1; // The resolution of the parameter r in pixels. We use 1 pixel.
        double theta = (CV_PI/180); // The resolution of the parameter theta in radians. We use 1 degree.
        int threshold = 0; // The minimum number of intersections to "detect" a line
        HoughLines(dst /* cannied */, cdst,  rho,  theta,  threshold);
        
        
        
        return cdst;
        
    }
    
};//end of class LaneDetector
