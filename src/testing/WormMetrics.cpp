/*
  Name:         WormMetrics.cpp (implementation)
  Author:       Kip Warner (Kip@TheVertigo.com)
  Description:  Tests out the Worm class...
  Quick Debug:  g++ WormMetrics.cpp ../Worm.cpp -g3 -o WormMetrics -Wall -Werror -lcxcore -lcv -lhighgui -lcvaux && insight --readnow --args ./WormMetrics WormFrame1.png WormFrame2.png WormFrame3.png WormFrame4.png
*/

// Includes...
#include "../Worm.h"
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <cassert>
#include <iostream>
#include <stdlib.h>
#include <math.h>

// Using the standard namespace...
using namespace std;

// Line segment...
typedef std::pair<CvPoint2D32f, CvPoint2D32f> LineSegment;


/*
    Test directed line segment length adjustment.
*/

/* Entry point...
int main()
{
    LineSegment A;
    
    while(true)
    {
        cout << "Line segment start: ";
        cin >> A.first.x >> A.first.y;
        cout << "Line segment end: ";
        cin >> A.second.x >> A.second.y;
        
        Worm::AdjustDirectedLineSegmentLength(A, 4.0f);
        
        cout << "New Line of length 4.0: (" 
             << A.first.x << ", " << A.first.y << ") ("
             << A.second.x << ", " << A.second.y << ")"
             << endl << endl;
    }
    
    return 0;
}*/

/* Mouse left click callback...
void OnMouse(int nEvent, int x, int y, int nFlags, void *pParameter)
{
    static bool         bGotFirstPoint      = false;
    static LineSegment  A;
    IplImage           *pOrthogonalImage    = (IplImage *) pParameter;

    switch(nEvent)
    {
        case CV_EVENT_RBUTTONDOWN:
            cvZero(pOrthogonalImage);
            cvShowImage("Orthogonal", pOrthogonalImage);
            break;
            
        case CV_EVENT_LBUTTONDOWN:
        {
            if(bGotFirstPoint)
            {
                A.second.x = x;
                A.second.y = y;
                bGotFirstPoint = false;

                cvCircle(pOrthogonalImage, cvPointFrom32f(A.second), 5, 
                         CV_RGB(255, 255, 255));
                
                cvLine(pOrthogonalImage, cvPointFrom32f(A.first), 
                       cvPointFrom32f(A.second), CV_RGB(255, 255, 255));

                LineSegment Orthogonal;
                GenerateOrthogonalToLineSegment(A, Orthogonal);

                AdjustDirectedLineSegmentLength(Orthogonal, 200.0f);

                cvLine(pOrthogonalImage, cvPointFrom32f(Orthogonal.first), 
                       cvPointFrom32f(Orthogonal.second),
                       CV_RGB(255, 255, 255));

                cvShowImage("Orthogonal", pOrthogonalImage);
            }

            else
            {
                A.first.x = x;
                A.first.y = y;
                bGotFirstPoint = true;
                
                cvCircle(pOrthogonalImage, cvPointFrom32f(A.first), 5, 
                         CV_RGB(255, 255, 255));
            }
            
            break;
        }
    }
}*/

/*
    Test orthogonal generation.
*/

/* Entry point...
int main(int nArguments, char *ppszArguments[])
{   
    CvSize Size = {800, 600};
    
    IplImage   *pOrthogonalImage = cvCreateImage(Size, 
                                                 IPL_DEPTH_8S, 1);
    cvZero(pOrthogonalImage);
    
    cvNamedWindow("Orthogonal", CV_WINDOW_AUTOSIZE);
    cvSetMouseCallback("Orthogonal", OnMouse, pOrthogonalImage);
    cvShowImage("Orthogonal", pOrthogonalImage);

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


/*
    Test Pinch-Shift and all the rest of it.
*/

    // Variables...
    CvContour *pCurrentContour  = NULL;

// Entry point...
int main(int nArguments, char *ppszArguments[])
{
    // Variables...
    CvContour  *pFirstContour   = NULL;

    // Print usage...
    if(nArguments <= 1)
    {
        cout << "Usage: WormMetrics [frame1.jpg] [frame2.jpg] ... [frameN.jpg]"
             << endl << "\t...such that each frame is black and white."
             << endl << endl;
        return 0;
    }
    
    // Create some base storage...
    CvMemStorage *pStorage  = cvCreateMemStorage(0);
    
    // Create a worm object...
    Worm Nematode;
    
    // Create windows...
    cvNamedWindow("Original", CV_WINDOW_AUTOSIZE);
    cvNamedWindow("Analysis", CV_WINDOW_AUTOSIZE);
//    cvSetMouseCallback("Analysis", OnMouse);

    // Process and display each image in sequence...
    for(int nCurrentFrame = 0; 
        nCurrentFrame < (nArguments - 1);
      ++nCurrentFrame)
    {
        // Alert user...
        cout << "Processing " << ppszArguments[nCurrentFrame + 1] << endl;

        // Load the image...
        IplImage *pGrayImage = cvLoadImage(ppszArguments[nCurrentFrame + 1],
                                           CV_LOAD_IMAGE_GRAYSCALE);

        // Create threshold...
        IplImage *pThresholdImage = cvCloneImage(pGrayImage);
        cvThreshold(pGrayImage, pThresholdImage, 150, 255, CV_THRESH_BINARY);

        // Find the contours in the threshold image...
        cvFindContours(pThresholdImage, pStorage, (CvSeq **) &pFirstContour, 
                       sizeof(CvContour), CV_RETR_LIST, CV_CHAIN_APPROX_NONE, 
                       cvPoint(0, 0));
        
        // Clear the threshold image...
        cvZero(pThresholdImage);
        
        // Examine each contour and make note of ones of possible worm size...
        for(pCurrentContour = pFirstContour; 
            pCurrentContour;
            pCurrentContour = (CvContour *) pCurrentContour->h_next)
        {
            // Too few vertices, discard...
            if(pCurrentContour->total < 6)
                continue;

            // Calculate the area of the worm...
            double dArea = fabs(cvContourArea(pCurrentContour));

                // Too small / too big to be a worm, discard...
                if((dArea < 400.0) || (1000.0 < dArea))
                    continue;

                cout << "\tFound a contour: " << pCurrentContour->total 
                     << " vertices... " << endl;
                flush(cout);

            // Discover worm's new state...
            Nematode.Discover(*pCurrentContour, *pGrayImage);

            // Pick a colour for the contour outline...
            CvScalar Color = CV_RGB(0xFF, 0xFF, 0xFF);
            
            // Draw the contour...
            cvDrawContours(pThresholdImage, (CvSeq *) pCurrentContour, 
                Color,                  // external colour
                Color,                  // hole colour
                0,                      // maximum level
                1,                      // thickness
                8,                      // line type
                cvPoint(0, 0));         // offset

            cvCircle(pThresholdImage, 
                     Nematode.Head(),
                     5,
                     CV_RGB(0xFF, 0xFF, 0xFF), -1, 2);
            cvCircle(pGrayImage, 
                     Nematode.Centre(),
                     4,
                     CV_RGB(0xFF, 0xFF, 0xFF), -1);
            cvCircle(pThresholdImage, 
                     Nematode.Tail(),
                     5,
                     CV_RGB(0xFF, 0xFF, 0xFF), 1);

            cout << Nematode << endl;


            // Show it...
            cvShowImage("Original", pGrayImage);
            cvShowImage("Analysis", pThresholdImage);
            cvMoveWindow("Analysis", 640, 0);
//            CvSize Size = cvGetSize(pThresholdImage);
//            cvResizeWindow("Analysis", int(Size.width * 1.5f), 
//                                       int(Size.height * 1.5f));
            
            // Wait for key press...
            while(true)
            {
                int nKey = cvWaitKey(500);

                if(nKey == 27)      // Escape exits...
                {
                    // Cleanup...
                    cvReleaseMemStorage(&pStorage);
                    cvDestroyAllWindows();
                    
                    // Done...
                    return 0;
                }
                else if(nKey == 32) // Space is next frame...
                    break;
                else                // Anything else is ignored...
                    continue;
            }
        }
        
        // Cleanup...
        cvReleaseImage(&pThresholdImage);
        cvReleaseImage(&pGrayImage);
    }
    
    // Cleanup...
    cvReleaseMemStorage(&pStorage);
    cvDestroyAllWindows();
    
    // Done...
    return 0;
}

