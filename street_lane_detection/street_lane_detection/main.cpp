//
//  main.cpp
//  Lane recognition system for robotic systems
//
//  Created by Markus Haug on 12.01.18.
//  Copyright © 2018 Haug Markus. All rights reserved.
//
#include <regex>
#include "lane_detection.h"
using namespace std;
using namespace cv;

bool isInteger(string);
void processInput(VideoCapture);

int
main(int argc, char*argv[]){
    if(argc > 1){
        if(isInteger(*(argv+1))){
            // process camera input
            VideoCapture cap(atoi(*(argv+1)));
            processInput(cap);
        } else {
            // process file
            VideoCapture cap(*(argv+1));
            processInput(cap);
        }
    } else {
        // Challenge video
        VideoCapture cap("/Users/markushaug/Desktop/challenge.mp4");
        processInput(cap);
    }
    cout << "press any key to close all windows";
    waitKey(0); // holds the video on the end
    cvDestroyAllWindows();
    return 0;
}


bool
isInteger(string x){
    regex digit("(^\\d{1,10}$)");
    if (regex_match (x, digit))
        return true;
    return false;
}


void
processInput(VideoCapture cap){
    
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
