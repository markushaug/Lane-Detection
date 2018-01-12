# street_lane_detection (currently in development)
Lane recognition system for robotic systems

In this block I will give you a very compact introduction to digital image processing.
 
 Since a picture is an analog signal, we have to convert it into a digital signal. This is due to two main reasons:
 -   Unlimited memory space is required to store an analog signal.
 -   Digital processing is simpler.
 
 When your camera captures an image, the CCD array in your camera captures the photons (light) with its sensors.
 Each sensor/pixel calculates the light intensity of the input photon and then insert them into a two-dimensional array (matrix).
 Thus, each sensor in the array represents one pixel.
 
 Well... now you "know" the fundamentals.
 
 In order to work with an image, we have to put it into a matrix (type Mat). In the following comments/lines I will use the word frame instead of image.
 
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
