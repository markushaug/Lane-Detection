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
#include <regex>
#include "lane_detection.h" // I outsourced all my helper functions in this header

using namespace std;
using namespace cv;

bool isInteger(string);
void processInput(VideoCapture);

int main(int argc, char*argv[]){
    if(argc > 1){
        if(isInteger(*(argv+1))){
            VideoCapture cap(atoi(*(argv+1)));
            processInput(cap);
        } else {
            VideoCapture cap(*(argv+1));
            processInput(cap);
        }
    } else {
        VideoCapture cap("/Users/markushaug/Desktop/wandaor.mov");
        //VideoCapture cap("/Users/markushaug/Desktop/LaneDetection.avi");
        processInput(cap);
    }
    cout << "press any key to close all windows";
    waitKey(0); // holds the video on the end
    cvDestroyAllWindows();
    return 0;
}


bool isInteger(string x){
    regex digit("([a-zA-Z])");
    if (regex_match (x, digit))
        return true;
    return false;
}

void processInput(VideoCapture cap){
    
    Mat frame;
    
    if(!cap.isOpened()){
       perror("Cannot open the input file");
       exit(1);
    }
    
    double fps = cap.get(CV_CAP_PROP_FPS); // Get the frames per seconds of the video
    cout << "Frames per second: " << fps << endl;
    
    cap.read(frame);
    if(frame.empty()){
        perror("frame is empty");
        exit(1);
    }
    LaneDetector detect(frame);
    
    for(;;){

        // Read a frame from the input video
        if(!cap.read(frame)){
            perror("Cannot read the video frame");
            break;
        }
        
        // Process next frame
        detect.nextFrame(frame);
        
        if(waitKey(10) == 27)
        {
            cout << "video paused!, press q to quit, any other key to continue"<<endl;
            if(waitKey(0) == 'q'){
                cout << "terminated by user" << endl;
                break;
            }
        }
        
    }
}
