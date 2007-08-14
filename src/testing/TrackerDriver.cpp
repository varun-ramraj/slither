/*
  Name:         TrackerDriver.cpp
  Author:       Kip Warner (Kip@TheVertigo.com)
  Description:  Driver for the worm tracker class...
  Quick Debug:  g++ TrackerDriver.cpp ../Tracker.cpp -g3 -o TrackerDriver -Wall -Werror -lcxcore -lcv -lhighgui -lcvaux && insight --readnow --args ./TrackerDriver TrackerFrame1.png TrackerFrame2.png TrackerFrame3.png TrackerFrame4.png
*/

// Includes...
#include "../Tracker.h"
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <iostream>
#include <cstdlib>
#include <cmath>

// Using the standard namespace...
using namespace std;

// Entry point...
int main(int nArguments, char *ppszArguments[])
{
    // Variables...
    CvContour  *pFirstContour   = NULL;
    CvContour  *pCurrentContour = NULL;

    // Print usage...
    if(nArguments <= 1)
    {
        cout << "Usage: TrackerDriver [frame1.jpg] [frame2.jpg] ... [frameN.jpg]"
             << endl << "\t...such that each frame is black and white."
             << endl << endl;
        return 0;
    }
    
    // Create some base storage...
    CvMemStorage *pStorage  = cvCreateMemStorage(0);
    
    // Create windows...
    cvNamedWindow("Tracker", CV_WINDOW_AUTOSIZE);

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



            // Show it...
            cvShowImage("Tracker", pGrayImage);

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

