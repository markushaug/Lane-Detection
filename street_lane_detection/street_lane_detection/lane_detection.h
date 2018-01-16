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
    int houghVote = 150;
    bool showOriginal = 1;
    bool showCanny = 1;
    bool showHough = 1;
    bool showHoughP = 1;
    int newHeight = 320,
        newWidth = 480,
        originalHeight{0},
        originalWidth{0};
    
public:
    Mat currFrame; //stores the upcoming frame
    
    
    LaneDetector(Mat startFrame)
    {
        
        currFrame = Mat(newHeight,newWidth,CV_8UC1,0.0);                        //initialised the image size to 320x480
        this->originalHeight = startFrame.rows;
            resize(startFrame, currFrame, currFrame.size());             // resize the input to required size
        
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
        
        //GaussianBlur(currFrame ,currFrame,Size(5, 5), 0);
        
        // Canny algorithm
        Mat contours;
        Canny(currFrame, contours , 100,200);
        Mat contoursInv;
        threshold(contours, contoursInv, 128, 255,THRESH_BINARY_INV);
        
        
        /* test */
        Mat imgIn = currFrame;
        imgIn.convertTo(imgIn, CV_32FC3, 1/255.0);
        
        // Output image is set to black
        Mat imgROI = Mat::ones(imgIn.size(), imgIn.type());
        imgROI = Scalar(0.0,0.0,0.0);
        
        int height, width;
        height = currFrame.rows;
        width = currFrame.cols;
        // Input triangle
        vector <Point2f> points;
        points.push_back(Point2f(0,height-1));
        points.push_back(Point2f(width-1,height-1));
        points.push_back(Point2f(width/2-1,height/2-1));
        
        // Warp all pixels inside input triangle to output triangle
        warpTriangle(imgIn, imgROI, points);
        
        // Convert back to unit because OpenCV antialiasing does not work on image of type CV_32FC3
        imgIn.convertTo(imgIn, CV_8UC3, 255.0);
        imgROI.convertTo(imgROI, CV_8UC3, 0.0);
        
        // Draw the triangle using this color
        Scalar color = Scalar(255, 150, 0);
        
        // cv::polylines needs vector of type Point and not Point2f
        vector<Point> triInInt, triOutInt;
        for(int i(0); i < 3; i++){
            triInInt.push_back( Point(points[i].x, points[i].y ));
        }
        // Draw the polylines
        polylines(imgIn, triInInt, true, color, 2, 16);
        polylines(imgROI, triInInt, true, color, 2, 16);
        
       
        if(showOriginal){
            namedWindow("Original Image");
            imshow("Original Image", currFrame);
        }
        
        /*//GaussianBlur(imgROI ,imgROI,Size(5, 5), 0);
        
        // Canny algorithm
        Mat contours;
        Canny(imgROI, contours, 100,200);
        Mat contoursInv;
        threshold(contours, contoursInv, 128, 255,THRESH_BINARY_INV);
        
        if(showCanny){
            namedWindow("Contours");
            imshow("Contours", contours);
        }
        */
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
        Mat hough(imgROI.size(), CV_8U, Scalar(255));
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
                line(result, pt1, pt2, Scalar(0,0,255), 4);
                line(hough, pt1, pt2, Scalar(0,0,255),4);
            }
            cout << "line: (" << rho << "," << theta << ")" << endl;
            ++it;
        };
        
        if(showHough){
            namedWindow("Detected lines with Hough");
            imshow("Detected lines with Hough", result);
        }
        
        // probabilistic Hough
        
        
        
    } // end main
    
    void nextFrame(Mat &nxt)
    {
        resize(nxt ,currFrame, currFrame.size()); //resizing the input image for faster processing
        getLaneLines();
    }
    
    void warpTriangle(Mat &img1, Mat &img2, vector<Point2f> tri){
        // Find bounding rectangle for each triangle
        Rect r1 = boundingRect(tri);
        Rect r2 = boundingRect(tri);
        
        // Offset points by left top corner of the respective rectangles
        vector<Point2f> tri1Cropped, tri2Cropped;
        vector<Point> tri2CroppedInt;
        for(int i = 0; i < 3; i++)
        {
            tri1Cropped.push_back( Point2f( tri[i].x - r1.x, tri[i].y -  r1.y) );
            tri2Cropped.push_back( Point2f( tri[i].x - r2.x, tri[i].y - r2.y) );
            
            // fillConvexPoly needs a vector of Point and not Point2f
            tri2CroppedInt.push_back( Point((int)(tri[i].x - r2.x), (int)(tri[i].y - r2.y)) );
            
        }
        
        // Apply warpImage to small rectangular patches
        Mat img1Cropped;
        img1(r1).copyTo(img1Cropped);
        
        // Given a pair of triangles, find the affine transform.
        Mat warpMat = getAffineTransform( tri1Cropped, tri2Cropped );
        
        // Apply the Affine Transform just found to the src image
        Mat img2Cropped = Mat::zeros(r2.height, r2.width, img1Cropped.type());
        warpAffine( img1Cropped, img2Cropped, warpMat, img2Cropped.size(), INTER_LINEAR, BORDER_REFLECT_101);
        
        // Get mask by filling triangle
        Mat mask = Mat::zeros(r2.height, r2.width, CV_32FC3);
        fillConvexPoly(mask, tri2CroppedInt, Scalar(1.0, 1.0, 1.0), 16, 0);
        
        // Copy triangular region of the rectangular patch to the output image
        multiply(img2Cropped,mask, img2Cropped);
        imshow("Region of interest", img2Cropped);
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
