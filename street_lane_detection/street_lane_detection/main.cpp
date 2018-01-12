//
//  main.cpp
//  Lane recognition system for robotic systems
//
//  Created by Markus Haug on 12.01.18.
//  Copyright © 2018 Haug Markus. All rights reserved.
//
//  The main purpose why I write this, is to explain you the needs for an "simple" lane recognition system.
//  You can use this code for your projects, of course. But advertise for me: haugmarkus.de :)

#include <iostream>
#include "lane_detection.h" // I outsourced all my helper functions in this header
using namespace std;
/*
 In this block I will give you a very compact introduction to digital image processing.
 
 Since a picture is an analog signal, we have to convert it into a digital signal. This is due to two main reasons:
 -   Unlimited memory space is required to store an analog signal.
 -   Digital processing is simpler.
 
 When your camera captures an image, the CCD array in your camera captures the photons (light) with its sensors.
 Each sensor/pixel calculates the light intensity of the input photon and then insert them into a two-dimensional array (matrix).
 Thus, each sensor in the array represents one pixel.
 
 Well... now you "know" the fundamentals.
 
 In order to work with an image, we have to put it into a matrix (type Mat). In the following comments/lines I will use the word frame instead of image. */

int main()
{
    // Okay enough for now, let's dive into it!
    
    // Initialization of a few matrices
    Mat frame, grayScaled, gaussian_blurr_frame, canny_frame, region_of_interest, hough_frame;
    int width, height, channels; // Integers for the images meta-data
    
    // So here use imread() to return the matrix of the image
    frame = imread("path/to/image/solidWhiteRight.jpg");
    
    // Now we can obtain informations about the matrix
    width = frame.cols; // Coloumns of the matrix represents the width of the image
    height = frame.rows; // Rows of the matrix represents the height of the image
    channels = frame.channels(); // Channels of the image
    
    // Creating windows for the output
    namedWindow("cam", CV_WINDOW_AUTOSIZE); // Creating an window for the cam output
    
    // Endless loop
    for(;;)
    {
        /*
         
         Oh hey, you've reached the main body of this algorithm.
         
         Basically, how does lane detection work?
         
         Hmm..... I will explain it to you:
         
         
         Assume that you have a picture of a street.
         
         1. Grayscale tranformation
         
         Since the lane lines are colored white, you can perform a grayscale transformation, to simplify further processing.
         Once you have done this step, you will have a gray scaled frame.
         
         2. Gaussian blur
         
         In the second step we perform the gaussian blur algorithm.
         
         So what is the Gaussian blur?
         -   This algorithm picks up each pixel and compares it with its neighboring pixels. Similar pixels blurs together. This makes them optically blurred.
         
         3. Canny edge detection
         
         A Gaussian transformation is necessary for the canny edge detection.
         In this step, the canny edge detection algorithm visualizes all the edges in the frame.
         Since we have blurred the image, it makes it easier to detect edges than without blurring.
         
         4. Defining the region of interest
         
         The region of interest is the area in which the lanes are generally in the frame.
         As a human being, you are aware where to look in to see the lane lines, the computer does not.
         Therefore, we must determine an area in which the lanes are located.
         
         5. Hough line detection
         
         Finally, we apply the Hough transform to the respective region of interest.
         The Hough algorithm detects gradients (lane lines are gradients) and visualizes the most dominating gradients detected in the frame.
         
         
         So... in the end we will then put the region-of-interest mask beyond the initial frame.
         And vollaaah the lines are visualized graphically in seconds!
         
         */
        
        
        grayScaled = grayscale(frame); // This function returns an one channel image (grey in this case)
        gaussian_blurr_frame = gaussian_blur(grayScaled, 5); // Blurr the frame with an kernel size of 5
        canny_frame = canny(gaussian_blurr_frame, 100, 200); // Detecting edges
        region_of_interest = region_of_interest(canny_frame); // Defining the region of interest (roi)
        hough_frame = hough(region_of_interest); // Perform the hough algorithm
        
        imshow("cam", frame); // display the frame
        if(waitKey(30) >= 0) break;
    }
    
    // the camera will be deinitialized automatically in VideoCapture destructor
    return 0;
}
