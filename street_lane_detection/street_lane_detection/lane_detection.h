//
//  lane_detection.h
//  opencv
//
//  Created by Markus Haug on 12.01.18.
//  Copyright © 2018 Haug Markus. All rights reserved.
//
#include "opencv2/opencv.hpp"
#include <iostream>
using namespace cv;
using namespace std;
#ifndef lane_detection_h
#define lane_detection_h


#endif /* lane_detection_h */


Mat grayscale(Mat frame){
    Mat output;
    cvtColor(frame, output, COLOR_BGR2GRAY); //This will return an image with only one color channel
    return output;
}

Mat canny(Mat frame, int low_threshold, int high_treshold){
    /*
     * Applies the Canny transform
     * - This will detect any edges in the picture. (Depends on the tresholds)
     */
    Mat output;
    Canny(frame,output, low_threshold, high_treshold);
    return output;
}


Mat gaussian_blur(Mat image, int kernel_size){
    Mat output;
    GaussianBlur(image,output,Size(kernel_size, kernel_size), 0);
    return output;
}

Mat region_of_interest(Mat frame, int vertices = 0){
    /* Applies an image mask
     
     In this step, we will only keep the region of the image defined by the polygon formed from the vertices. the rest will be blackened.
     */
    int width = frame.cols;
    int heigth = frame.rows;
    
    /// What happens here????
    Rect roi(0,width/3,width-1,heigth- width/3); // set the ROI for the image
    
    Mat output = frame(roi);
    
    return output;
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
