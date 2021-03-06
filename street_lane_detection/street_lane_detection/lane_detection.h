#include <opencv2/core/core.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
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
 Scalar colorBlue = Scalar(255, 150, 0);
    
public:
    Mat currFrame; //stores the upcoming frame
    
    LaneDetector(Mat startFrame)
    {
        currFrame = Mat(newHeight,newWidth,CV_8UC1,0.0);
        this->originalHeight = startFrame.rows;
        
        // resize the input to required size for faster processing
        resize(startFrame, currFrame, currFrame.size());
        
        getLaneLines();
    }
    ~LaneDetector()
    {
        
    }
    
    void
    setupDetector(int _newHeight, int _newWidth, int _houghVote, bool _showOriginal,
                  bool _showCanny, bool _showHough, bool _showHoughP)
    {
        this->newHeight = _newHeight;
        this->newWidth = _newWidth;
        this->houghVote = _houghVote;
        this->showOriginal = _showOriginal;
        this->showCanny = _showCanny;
        this->showHough = _showHough;
        this->showHoughP  = _showHoughP;
    }
    
    
    void
    preProcess(Mat &imgIn, Mat &imgRoi, Mat &contours)
    {
        int height, width;
        height = currFrame.rows;
        width = currFrame.cols;
        // Input triangle
        vector <Point2f> points;
        points.push_back(Point2f(0,height-1));
        points.push_back(Point2f(width-1,height-1));
        points.push_back(Point2f(width/2-1,height/2-1));
        
        // Warp all pixels inside input triangle to output triangle
        crateRegionOfInterest(imgIn, imgRoi, points);
    
        // cv::polylines needs vector of type Point and not Point2f
        vector<Point> triInInt, triOutInt;
        for(int i(0); i < 3; i++){
            triInInt.push_back( Point(points[i].x, points[i].y ));
        }
        // Draw the polylines
        polylines(imgIn, triInInt, true, colorBlue, 2, 16);
        polylines(imgRoi, triInInt, true, colorBlue, 2, 16);
        
        if(showOriginal){
            namedWindow("Original Image");
            imshow("Original Image", currFrame);
        }
        
        cvtColor(imgRoi, imgRoi, COLOR_BGR2GRAY);
        GaussianBlur(imgRoi ,imgRoi,Size(5, 5), 0);
        
        // Canny algorithm
        Canny(imgRoi, contours, 50,150);
        Mat contoursInv;
        threshold(contours, contoursInv, 128, 255,THRESH_BINARY_INV);
        
        if(showCanny){
            namedWindow("Contours");
            imshow("Contours", contours);
        }
    }
    
    void
    getLaneLines()
    {
        Mat imgIn = currFrame,
            imgRoi = Mat::ones(imgIn.size(), imgIn.type()),
            contours;
        
        imgRoi = Scalar(0.0,0.0,0.0);
        
        // Define Region and interest and find contours
        preProcess(imgIn, imgRoi, contours);
        
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
    
        Mat result(imgRoi.size(), CV_8U, Scalar(255));
        imgRoi.copyTo(result);
        
        // Draw the lines
        vector<Vec2f>::const_iterator it=lines.begin();
        Mat hough(imgRoi.size(), CV_8U, 255);
        
        while(it!=lines.end()){
            float rho = (*it)[0];
            float theta = (*it)[1];
            if((theta > 0.09 && theta < 1.48) || (theta < 3.14 && theta > 2.66)){
                // so we actually removed vertical and horizontal lines
                
                // point of intersection of the line with first row
                Point pt1(rho/cos(theta),0);
                // point of intersection of the line with last row
                Point pt2((rho-result.rows*sin(theta))/cos(theta), result.rows);
                cout << "drawing line for P1(" <<pt1.x <<"," << pt1.y
                     << ") and P2(" << pt2.x<<","<<pt2.y<<")"<<endl;
                // draw a line
                
               /* points.push_back(Point2f(0,height-1));
                points.push_back(Point2f(width-1,height-1));
                points.push_back(Point2f(width/2-1,height/2-1));
                */
                    line(result, pt1, pt2, colorBlue, 4);
                    line(hough, pt1, pt2, colorBlue,4);
                
           }
            cout << "line: (" << rho << "," << theta << ")" << endl;
            ++it;
        };
        
        if(showHough){
            namedWindow("Detected lines with Hough");
            imshow("Detected lines with Hough", result);
        }
        
        // probabilistic Hough
        // TODO
        
        
    } // end main
    
    void
    nextFrame(Mat &nxt)
    {
        resize(nxt ,currFrame, currFrame.size());// resizing the input image for faster processing
        getLaneLines();
    }
    
    void
    crateRegionOfInterest(Mat &img1, Mat &img2, vector<Point2f> tri)
    {
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
        warpAffine( img1Cropped, img2Cropped, warpMat, img2Cropped.size(),
                   INTER_LINEAR, BORDER_REFLECT_101);
        
        // Get mask by filling triangle
        Mat mask = Mat::zeros(r2.height, r2.width, CV_8UC3);
        fillConvexPoly(mask, tri2CroppedInt, Scalar(1.0, 1.0, 1.0), 16, 0);
        
        // Copy triangular region of the rectangular patch to the output image
        multiply(img2Cropped,mask, img2Cropped);
        img2 = img2Cropped;
   }
};//end of class LaneDetector
