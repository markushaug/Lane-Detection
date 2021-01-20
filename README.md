<h1 align="center">
  Lane recognition system for robotic systems 
  <br>
</h1>

<h4 align="center">(No longer being developed.)</h4>

<p align="center">
  <a href="https://www.codacy.com/app/markushaug/Lane-Detection?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=markushaug/Lane-Detection&amp;utm_campaign=Badge_Grade"><img src="https://api.codacy.com/project/badge/Grade/804fd98a625b43869f597ea77ff0e37d"/></a>
 
</p>

[![Dashboard](https://imgur.com/6j3lfu0.png)](https://github.com/markushaug/Lane-Detection)

## Table of content

- [About](#about)
- [Key Features](#key-features)
- [How to use](#how-to-use)
- [Implementation](#implementation)
    - [Preprocessing](#preprocessing)
    - [Canny Edge filter](#canny-edge-filter)
    - [Hough Transform](#hough-transform)
- [TODO](#todo)
- [FAQ / CONTACT / TROUBLESHOOT](#faq--contact--troubleshoot)
- [Contributing](#contributing)


## ABOUT

C++ program using OpenCV to detect and track lanes. Can be used for autonomous driving functions, e.g. to determine whether the driver is coming out of the lane.

## KEY FEATURES

* Written in C++ with OpenCV
* IDE: Xcode
* Customizable parameters (Treshold, ROI Size, ...)
* Signal Processing:
  * Create ROI
  * Canny edge detection
  * Hough line detector

## HOW TO USE
I only worked with Xcode under OSX. For other IDEs or compilers I can not provide any information.

1. In Xcode is set up the library search paths for OpenCV. 
 - In my case it is /usr/local/Cellar/opencv/3.4.0/lib /usr/local/lib.
2. Set path to input file in `main.cpp` or pass arguments at programm start
 - Or use a single integer to select one of your web cams. (Webcam1 == 1, cam2 == 2, ...)
 - Or provide an pth to a file (video/image)
3. Configure options in `lane_detection.h`
 - `houghVote` determines the fidelity of the line recognition (higher means more lines kept)
 -  Further you can enable or disable windows of your choice
4. Run program
 - Use the xcode project file
 - You may have to change the library paths to your library folder

## Implementation

### Preprocessing
![ROI](https://imgur.com/oEP591n.png "ROI")

The Algorithm creates the ROI, performs the gaussian blur filter to smooth the frame in order to remove noise and perform the canny edge filter.

### Canny Edge filter
![CANNY](https://imgur.com/TMQ7wIk.png "Edge Detection")

Used to create a binary image of edges. The Gaussian blur is used, followed by a Sobel filter in the x and y direction to determine the size of the gradient. The non-maximum suppression is applied to remove pixels from an identified edge. To determine an edge, two thresholds are used. If a pixel gradient is greater than the upper threshold, the pixel is retained, and if the pixel is lower than the lower threshold, it is rejected. Pixels in the center of the two thresholds are retained when a neighboring pixel is considered part of an edge.

### Hough Transform
![HOUGH](https://imgur.com/6j3lfu0.png "Hough Transform")

Used to recognize straight lines where a line is represented by polar coordinates. When all x-y values are represented as polar coordinates, intersection points of the resulting sinusoidal waves show at least two points that share the same line, with the intersection point representing an additional point on the line. As a result, the filter tracks the intersections and declares a line only if the number of points on the line exceeds a certain threshold.

## TODO
- Apply a Bitwise-AND to put the Filter-Layer on top of the input stream.
- Optimization of the Algorithm (Intersection, Converting infinite lines to finite lines, ...)
- Probabilistic Hough
  -> Equivalent to the Hough transformation, but provides endpoint values for all lines determined.

## FAQ / CONTACT / TROUBLESHOOT
If you run into issues while using Lane-Detection, please use one of the following options:

- Use github's issue reporter on the right, so that other people can search these issues too
- Send me an email <a href="mailto:mh@haugmarkus.de">mh@haugmarkus.de</a> (might take a few days)

## CONTRIBUTING
I would appreciate it if you would contribute to this project.
Do not hesitate to contact me if you are interested. I can give you an introduction to the core and the main concepts of the program. (Intermediate) C++, OpenCV and Math skills are required.

---

> Homepage [haugmarkus.de](https://www.haugmarkus.de) &nbsp;&middot;&nbsp;
> GitHub [@markushaug](https://github.com/markushaug) &nbsp;&middot;&nbsp;

