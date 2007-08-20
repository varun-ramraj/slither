/*
  Name:         AnalysisThread.cpp (implementation)
  Author:       Kip Warner (Kip@TheVertigo.com)
  Description:  AnalysisThread class...
*/

// Includes...
#include "AnalysisThread.h"
#include "MainFrame.h"
#include "Experiment.h"

// Analysis thread constructor...
AnalysisThread::AnalysisThread(MainFrame &_Frame)
    : Frame(_Frame),
      AnalysisLock(_Frame),
      pCapture(NULL)
{

}

// Thread entry point...
void *AnalysisThread::Entry()
{
    // Variables...
    IplImage           *pGrayImage              = NULL;
    wxString            sTemp;

    // Get the complete path to the video to analyze...

        // Find the row selected...
        wxArrayInt SelectedRows = Frame.VideosGrid->GetSelectedRows();
        int nRow = SelectedRows[0];
        
        // Generate complete path...
        wxString sPath = Frame.pExperiment->GetCachePath() +
                         wxT("/videos/") +
                         Frame.VideosGrid->GetCellValue(nRow, 
                                                              MainFrame::TITLE);

    // Initialize capture from AVI...
    pCapture = cvCreateFileCapture(sPath.fn_str());

        // Failed to load video...
        if(!pCapture)
        {
            // Alert...
            wxLogError(wxT("Your system does not appear to have an suitable"
                           " codec installed to read this video."));
            
            // Abort...
            return NULL;
        }

    // Start the analysis stop watch...
    StatusUpdateStopWatch.Start();

    // Keep showing video until there is nothing left or cancel requested...
    while(!Frame.bExiting && !TestDestroy())
    {
        // The Quicktime backend appears to be buggy in that it keeps cycling
        //  through the video even after we have all frames. A temporary hack
        //  is to just break the analysis loop when we have both current frame, 
        //  total frame, and they are equal...
        #ifdef __APPLE__

            // Get current position...
            int const nCurrentFrame = (int) 
                cvGetCaptureProperty(pCapture, CV_CAP_PROP_POS_FRAMES);

            // Get total number of frames...
            int const nTotalFrames = (int) 
                cvGetCaptureProperty(pCapture, CV_CAP_PROP_FRAME_COUNT);

            // Reached the end...
            if(nCurrentFrame + 1 == nTotalFrames)
                break;

        #endif

        // Retrieve the captured image...
        IplImage const &OriginalImage = *cvQueryFrame(pCapture);

        // The tracker prefers grayscale 8-bit unsigned format, prepare...

            // Allocate blank grayscale image...
            pGrayImage = cvCreateImage(cvGetSize(&OriginalImage), 
                                       IPL_DEPTH_8U, 1);

            // Convert original to grayscale...
            cvConvertImage(&OriginalImage, pGrayImage);

        // Feed into tracker...
        Tracker.AdvanceNextFrame(*pGrayImage);
        
        // Cleanup gray image...
        cvReleaseImage(&pGrayImage);
        
        // Depending on processor throttle setting, idle...
        if(Frame.ProcessorThrottle->GetValue() < 100)
        {
            // Compute sleep time from slider value... [1,1000ms]
            int nSleepTime = 
                ((100 - Frame.ProcessorThrottle->GetValue()) * 10);

            // Give up rest of time slice to system for other threads...
            Yield();
            
            // Sleep...
            wxThread::Sleep(nSleepTime);
        }
    }

    // Release the capture source...
    cvReleaseCapture(&pCapture);

    // Done...
    return NULL;
}

// Constructor locks UI...
AnalysisThread::AnalysisAutoLock::AnalysisAutoLock(MainFrame &_Frame)
    : Frame(_Frame)
{
    // Variables...
    wxString    sTemp;

    // Begin analysis button...
    Frame.BeginAnalysisButton->Disable();

    // Cancel analysis button...
    Frame.CancelAnalysisButton->Enable();

    // Microscope set...
    Frame.ChosenMicroscopeName->Disable();
    Frame.ChosenMicroscopeTotalZoom->Disable();
    Frame.FieldOfViewDiameter->Disable();

    // Analysis type...
    Frame.ChosenAnalysisType->Disable();

    // Analysis buttons...
    Frame.BeginAnalysisButton->Disable();
    Frame.CancelAnalysisButton->Enable();

    // Analysis gauge...
    Frame.AnalysisGauge->SetRange(100);
    Frame.AnalysisGauge->SetValue(0);

    // Clear the status list...
    Frame.AnalysisStatusList->Clear();

    // Alert user...
    sTemp = Frame.ChosenAnalysisType->GetString(
                Frame.ChosenAnalysisType->GetCurrentSelection()) + 
            wxT(" analysis is running...");
    Frame.AnalysisStatusList->Append(sTemp);

    // Refresh the main frame...
    Frame.Refresh();
    
    // Create analysis window...
    cvNamedWindow("Analysis", CV_WINDOW_AUTOSIZE);
}

// Deconstructor unlocks UI...
AnalysisThread::AnalysisAutoLock::~AnalysisAutoLock()
{
    // Begin analysis button...
    Frame.BeginAnalysisButton->Enable();

    // Cancel analysis button...
    Frame.CancelAnalysisButton->Disable();

    // Microscope set...
    Frame.ChosenMicroscopeName->Enable();
    Frame.ChosenMicroscopeTotalZoom->Enable();
    Frame.FieldOfViewDiameter->Enable();

    // Analysis type...
    Frame.ChosenAnalysisType->Enable();

    // Analysis buttons...
    Frame.BeginAnalysisButton->Enable();
    Frame.CancelAnalysisButton->Disable();

    // Analysis gauge...
    Frame.AnalysisGauge->SetRange(100);
    Frame.AnalysisGauge->SetValue(0);

    // Alert user...
    Frame.AnalysisStatusList->Append(wxT("Analysis ended..."));

    // Refresh the main frame...
    Frame.Refresh();
    
    // Destroy analysis window...
    cvDestroyWindow("Analysis");

    // Stop the analysis timer...
    Frame.AnalysisTimer.Stop();
}

