/*
  Name:         TrackerDriver.cpp
  Author:       Kip Warner (Kip@TheVertigo.com)
  Description:  Driver for the worm tracker class...
  Quick Debug: g++ -I/home/varun/Projects/slither/Source `pkg-config --cflags opencv4` `wx-config --cflags` TrackerDriver.cpp ../Source/WormTracker.cpp ../Source/Worm.cpp ../Source/SlitherMath.cpp -g3 -o TrackerDriver -Wall -Werror `pkg-config --libs opencv4` `wx-config --libs`

*/

// Includes...
#include "../Source/WormTracker.h"
#include "../Source/SlitherMath.h"
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui_c.h>
#include <opencv2/imgcodecs.hpp>
#include <iostream>
#include <cstdlib>
#include <cmath>

// Using the standard namespace...
using namespace std;

/* Globals...
IplImage   *pGrayImage  = NULL;

// Do the two rectangles have a non-zero intersection area?
bool IsRectanglesIntersect(CvRect const &RectangleOne,
                           CvRect const &RectangleTwo)
{
    return (RectangleOne.x < RectangleTwo.x + RectangleTwo.width) && 
           (RectangleOne.y < RectangleTwo.y + RectangleTwo.height) &&
           (RectangleOne.x + RectangleOne.width > RectangleTwo.x) && 
           (RectangleOne.y + RectangleOne.height > RectangleTwo.y);
}

// Mouse left click callback...
void OnMouse(int nEvent, int x, int y, int nFlags, void *pParameter)
{
    static int          nPoint          = 0;
    static CvRect       FirstRect       = {0, 0, 0, 0};
    static CvRect       SecondRect      = {0, 0, 0, 0};

    if(!pGrayImage)
        return;

    switch(nEvent)
    {
        case CV_EVENT_RBUTTONDOWN:
            cvZero(pGrayImage);
            nPoint = 0;
            cvShowImage("Rectangle", pGrayImage);
            break;
            
        case CV_EVENT_LBUTTONDOWN:
        {
            switch(nPoint)
            {
                case 0:

                    FirstRect.x = x;
                    FirstRect.y = y;

                    nPoint++;                
                    break;
                
                case 1:

                    FirstRect.width = abs(x - FirstRect.x);
                    FirstRect.height = abs(y - FirstRect.y);

                    cvRectangle(pGrayImage, 
                                cvPoint(FirstRect.x, FirstRect.y), 
                                cvPoint(FirstRect.x + FirstRect.width,
                                        FirstRect.y + FirstRect.height),
                                CV_RGB(0xff, 0xff, 0xff), 1);

                    cvShowImage("Rectangle", pGrayImage);

                    nPoint++;
                    break;

                case 2:
                
                    SecondRect.x = x;
                    SecondRect.y = y;
                
                    nPoint++;
                    break;

                default:

                    SecondRect.width = abs(x - SecondRect.x);
                    SecondRect.height = abs(y - SecondRect.y);

                    cvRectangle(pGrayImage, 
                                cvPoint(SecondRect.x, SecondRect.y), 
                                cvPoint(SecondRect.x + SecondRect.width,
                                        SecondRect.y + SecondRect.height),
                                CV_RGB(0xff, 0xff, 0xff), 1);

                    cvShowImage("Rectangle", pGrayImage);
                    
                    if(IsRectanglesIntersect(FirstRect, SecondRect))
                        cout << "Intersection..." << endl;
                    else
                        cout << "No intersection..." << endl;
                    
                    nPoint = 0;
                    break;
            }
            break;
        }
    }
}

// Entry point...
int main(int nArguments, char *ppszArguments[])
{   
    CvSize Size = {800, 600};
    
    pGrayImage = cvCreateImage(Size, IPL_DEPTH_8S, 1);
    cvZero(pGrayImage);
    
    cvNamedWindow("Rectangle", CV_WINDOW_AUTOSIZE);
    cvSetMouseCallback("Rectangle", OnMouse);
    cvShowImage("Rectangle", pGrayImage);

    // Wait for key press...
    while(true)
    {
        int nKey = cvWaitKey(500);

        if(nKey == 27)      // Escape exits...
        {
            // Cleanup...
            cvDestroyAllWindows();
            
            // Done...
            break;
        }

        else                // Anything else is ignored...
            continue;
    }

    return 0;
}*/

// Entry point...
int main(int nArguments, char *ppszArguments[])
{
    // Objects...
    WormTracker TestTracker;
    cv::Mat pGrayImage; 
    cv::Mat pThinkingImage;

    TestTracker.SetFieldOfViewDiameter(5.0f);

    // Print usage...
    if(nArguments <= 1)
    {
        cout << "Usage: TrackerDriver [frame1.jpg] [frame2.jpg] ... [frameN.jpg]"
             << endl << "\t...such that each frame is black and white."
             << endl << endl;
        return 0;
    }
    
    // Create windows...
    cv::namedWindow("Tracker", cv::WINDOW_AUTOSIZE);

    // Process and display each image in sequence...
    for(int nCurrentFrame = 0; 
        nCurrentFrame < (nArguments - 1); 
      ++nCurrentFrame)
    {
        // Alert user...
        cout << "Processing " << ppszArguments[nCurrentFrame + 1] << endl;

        // Load the image...
        pGrayImage = cv::imread(ppszArguments[nCurrentFrame + 1],
                                 cv::IMREAD_GRAYSCALE);

	cv::imshow("Tracker", pGrayImage);

        // Advance tracker frame...
        IplImage grayImg = cvIplImage(pGrayImage);
	TestTracker.Advance(grayImg);

        // Show some information on the tracker...
        cout << "Tracker reports: " << TestTracker << endl;

        // Show it...
        //cvShowImage("Tracker", TestTracker.GetThinkingImage());
	//CvSize Size = cvGetSize(TestTracker.GetThinkingImage());
	//printf("Image size: %d x %d", Size.width, Size.height);
	//pThinkingImage = cv::cvarrToMat(TestTracker.GetThinkingImage());
	//cv::imshow("Tracker", pThinkingImage);
	//CvSize Size = cvGetSize(TestTracker.GetThinkingImage());
        //cvResizeWindow("Tracker", Size.width * 2, Size.height * 2);
	
        // Wait for key press...
        while(true)
        {
            int nKey = cvWaitKey(500);

            if(nKey == 27)      // Escape exits...
            {
                // Cleanup...
		cv::destroyAllWindows();
                
                // Done...
                return 0;
            }
            else if(nKey == 32) // Space is next frame...
                break;
            else                // Anything else is ignored...
                continue;
        }
        
        // Cleanup...
        //cvReleaseImage(&pGrayImage);
    }
    
    // Cleanup...
    cv::destroyAllWindows();
    
    // Done...
    return 0;
}

