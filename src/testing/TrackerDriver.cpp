/*
  Name:         TrackerDriver.cpp
  Author:       Kip Warner (Kip@TheVertigo.com)
  Description:  Driver for the worm tracker class...
  Quick Debug:  g++ TrackerDriver.cpp ../WormTracker.cpp ../Worm.cpp -g3 -o TrackerDriver -Wall -Werror -lcxcore -lcv -lhighgui -lcvaux && insight --readnow --args ./TrackerDriver TrackerFrame1.png TrackerFrame2.png TrackerFrame3.png TrackerFrame4.png
*/

// Includes...
#include "../WormTracker.h"
#include <opencv/cv.h>
#include <opencv/highgui.h>
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
    WormTracker     TestTracker;
    IplImage       *pGrayImage      = NULL;

    // Print usage...
    if(nArguments <= 1)
    {
        cout << "Usage: TrackerDriver [frame1.jpg] [frame2.jpg] ... [frameN.jpg]"
             << endl << "\t...such that each frame is black and white."
             << endl << endl;
        return 0;
    }
    
    // Create windows...
    cvNamedWindow("Tracker", 0);//, CV_WINDOW_AUTOSIZE);

    // Process and display each image in sequence...
    for(int nCurrentFrame = 0; 
        nCurrentFrame < (nArguments - 1); 
      ++nCurrentFrame)
    {
        // Alert user...
        cout << "Processing " << ppszArguments[nCurrentFrame + 1] << endl;

        // Load the image...
        pGrayImage = cvLoadImage(ppszArguments[nCurrentFrame + 1],
                                 CV_LOAD_IMAGE_GRAYSCALE);

        assert(pGrayImage);
                                           
        // Advance tracker frame...
        TestTracker.AdvanceNextFrame(*pGrayImage);

        // Show some information on the tracker...
        cout << TestTracker << endl;

        // Show it...
        cvShowImage("Tracker", TestTracker.GetThinkingImage());
//        cvResizeWindow("Tracker", 720, 480);

        // Wait for key press...
        while(true)
        {
            int nKey = cvWaitKey(500);

            if(nKey == 27)      // Escape exits...
            {
                // Cleanup...
                cvDestroyAllWindows();
                
                // Done...
                return 0;
            }
            else if(nKey == 32) // Space is next frame...
                break;
            else                // Anything else is ignored...
                continue;
        }
        
        // Cleanup...
        cvReleaseImage(&pGrayImage);
    }
    
    // Cleanup...
    cvDestroyAllWindows();
    
    // Done...
    return 0;
}

