/*
  Name:         CaptureThread.cpp (implementation)
  Author:       Kip Warner (Kip@TheVertigo.com)
  Description:  CaptureThread class...
*/

// Includes...
#include "CaptureThread.h"
#include "MainFrame.h"
#include "Experiment.h"
#include <wx/dcbuffer.h>

    #include <cstdio>

// Capture thread constructor...
CaptureThread::CaptureThread(MainFrame *_pMainFrame)
    : pMainFrame(_pMainFrame)
{

}
        
// Thread entry point...
void *CaptureThread::Entry()
{
    // Variables...
    IplImage   *pOriginalIntelImage = NULL;
    IplImage   *pClonedIntelImage   = NULL;

    /* TODO: Allow user to pick which camera, if more than one detected.
             http://opencvlibrary.sourceforge.net/faq#head-3921717fef168800c43a822b03ca241ce8e9cc6d */

    // Initialize live capture from camera, then re-enable capture button...
    CvCapture *pCapture = cvCreateCameraCapture(CV_CAP_ANY);
    pMainFrame->GetToolBar()->EnableTool(MainFrame::ID_CAPTURE, true);

        // Failed to connect to camera...
        if(!pCapture)
        {
            // Alert...
            printf("cvCreateCameraCapture failed (%d)\n", cvGetErrStatus());
            wxLogError(wxT("I can't capture from your camera. Check to make"
                           " sure that it is plugged in, powered on, and the"
                           " drivers are installed and working.\n\n"
                           
                           "If you have successfully captured from the camera"
                           " already, it may just be a buggy driver."));

            // Cleanup, abort...
            pMainFrame->CaptureTimer.Stop();
            pMainFrame->GetToolBar()->ToggleTool(MainFrame::ID_CAPTURE, false);
            return NULL;
        }

    // Keep showing live feed as long there are frames and the capture button
    //  remains toggled...
    while(cvGrabFrame(pCapture) &&
          pMainFrame->GetToolBar()->GetToolState(MainFrame::ID_CAPTURE))
    {
/*
printf("%.2f\tCV_CAP_PROP_POS_MSEC\n"
       "%.2f\tCV_CAP_PROP_POS_FRAMES\n"
       "%.2f\tCV_CAP_PROP_POS_AVI_RATIO\n"
       "%.2f\tCV_CAP_PROP_POS_WIDTH\n"
       "%.2f\tCV_CAP_PROP_POS_HEIGHT\n"
       "%.2f\tCV_CAP_PROP_POS_FPS\n"
       "%.2f\tCV_CAP_PROP_FRAME_COUNT\n"
       "%.2f\tCV_CAP_PROP_POS_FOURCC\n\n",
       cvGetCaptureProperty(pCapture, CV_CAP_PROP_POS_MSEC),
       cvGetCaptureProperty(pCapture, CV_CAP_PROP_POS_FRAMES),
       cvGetCaptureProperty(pCapture, CV_CAP_PROP_POS_AVI_RATIO),
       cvGetCaptureProperty(pCapture, CV_CAP_PROP_FRAME_WIDTH),
       cvGetCaptureProperty(pCapture, CV_CAP_PROP_FRAME_HEIGHT),
       cvGetCaptureProperty(pCapture, CV_CAP_PROP_FPS),
       cvGetCaptureProperty(pCapture, CV_CAP_PROP_FRAME_COUNT),
       cvGetCaptureProperty(pCapture, CV_CAP_PROP_FOURCC));
*/
        // Retrieve the captured frame we just grabbed and check for error...
        pOriginalIntelImage = cvRetrieveFrame(pCapture);

        // Make our own internal copy to modify...
        pClonedIntelImage = cvCloneImage(pOriginalIntelImage);

        // Perform post processing...
        PerformPostProcessing(pClonedIntelImage);
        
        // Show the OSD over the frame...
        //ShowOnScreenDisplay(pClonedIntelImage);
        
        // Add frame to capture frame buffer...
        pMainFrame->CaptureFrameBuffer.push_back(pClonedIntelImage);
    }

    // Flush the capture frame queue...
    while(!pMainFrame->CaptureFrameBuffer.empty())
    {
        // Deallocate oldest...
        pClonedIntelImage = (IplImage *) pMainFrame->CaptureFrameBuffer.front();
        pMainFrame->CaptureFrameBuffer.pop_front();
        cvReleaseImage(&pClonedIntelImage);
    }

    // Release the capture source...
    cvReleaseCapture(&pCapture);

    // Repaint the capture image panel...
    wxMutexGuiEnter(); 
    pMainFrame->CaptureImagePanel->Refresh();
    wxMutexGuiLeave(); 

    // Stop the timer...
    pMainFrame->CaptureTimer.Stop();

    // Untoggle the capture button...
    pMainFrame->GetToolBar()->ToggleTool(MainFrame::ID_CAPTURE, false);

    // Done...
    return NULL;
}

// Perform post processing...
void CaptureThread::PerformPostProcessing(IplImage *pIntelImage)
{
    // Stubbed...
}

// Show the OSD over the frame...
void CaptureThread::ShowOnScreenDisplay(IplImage *pIntelImage)
{
    // Variables...
    CvFont  Font;
    float   fHorizontalScale    = 1.0;
    float   fVerticalScale      = 1.0;
    int     nLineWidth          = 1;
    
    // Create font...
    cvInitFont(&Font, CV_FONT_HERSHEY_SIMPLEX, fHorizontalScale, fVerticalScale,
               0, nLineWidth);

    // Write status...
    cvPutText(pIntelImage, "Live capture", cvPoint(10, 30), &Font, 
              cvScalar(243, 0, 0));
}

