/*
  Name:         WormMetrics.cpp (implementation)
  Author:       Kip Warner (Kip@TheVertigo.com)
  Description:  Tests out the Worm class...
  Quick Start:  g++ WormMetrics.cpp ../Worm.cpp -o WormMetrics -Wall -Werror 
                    -lcxcore -lcv -lhighgui -lcvaux
*/

// Includes...
#include "../Worm.h"
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <cassert>
#include <iostream>

// Using the standard namespace...
using namespace std;

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

        // Show it...
        cvShowImage("Original", pGrayImage);

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
        for(CvContour *pCurrentContour = pFirstContour; 
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
                     << " vertices... ";

            // Discover worm's new state...
            Nematode.Discover(*pCurrentContour, *pGrayImage);
            cout << "discovery complete" << endl;

            // Pick a random colour for the contour outline...
            CvScalar Color = CV_RGB(0xFF, 0xFF, 0xFF);
            
            // Draw the contour...
            cvDrawContours(pThresholdImage, (CvSeq *) pCurrentContour, 
                Color,                  /* external colour */
                Color,                  /* hole colour */
                0,                      /* maximum level */
                2,                      /* thickness */
                8,                      /* line type */
                cvPoint(0, 0));         /* offset */

            /*
                TODO: Run the analysis here.
            */

            // Show it...
            cvShowImage("Analysis", pThresholdImage);
            cvWaitKey(0);
        }
        
        // Cleanup...
        cvReleaseImage(&pThresholdImage);
        cvReleaseImage(&pGrayImage);
    }
    
    // Cleanup...
    cvReleaseMemStorage(&pStorage);
    
    // Done...
    return 0;
}
