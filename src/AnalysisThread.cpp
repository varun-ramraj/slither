/*
  Name:         AnalysisThread.cpp (implementation)
  Author:       Kip Warner (Kip@TheVertigo.com)
  Description:  AnalysisThread class...
*/

// Includes...
#include "AnalysisThread.h"
#include "MainFrame.h"
#include "Experiment.h"

// Analysis thread constructor locks UI...
AnalysisThread::AnalysisThread(MainFrame &_Frame)
    : wxThread(wxTHREAD_DETACHED),
      Frame(_Frame),
      pCapture(NULL)
{
    // Reset the tracker, if not already...
    Frame.Tracker.Reset();
    
    // Initiate the analysis timer...
    Frame.AnalysisTimer.Start(100, wxTIMER_CONTINUOUS);
}

// Thread entry point...
void *AnalysisThread::Entry()
{
    // Get the complete path to the media to analyze...

        // Find the row selected...
        wxArrayInt SelectedRows = Frame.MediaGrid->GetSelectedRows();
        int nRow = SelectedRows[0];
        
        // Generate complete path...
        wxString sPath = Frame.pExperiment->GetCachePath() +
                         wxT("/media/") +
                         Frame.MediaGrid->GetCellValue(nRow, MainFrame::TITLE);

    // Find the media...
    wxFileName MediaFile(sPath);

        // Failed...
        if(!MediaFile.IsOk())
            return false;

    // Get the file extension...
    wxString sExtension = MediaFile.GetExt().Lower();

    // It is a movie...
    if(sExtension == wxT("mov")   ||
       sExtension == wxT("avi")   ||
       sExtension == wxT("mpg")   ||
       sExtension == wxT("mpeg"))
        AnalyzeVideo(sPath);

    // It is an image...
    else
        AnalyzeImage(sPath);
        
    // Done...
    return NULL;
}

// Analyze single image...
void AnalysisThread::AnalyzeImage(wxString sPath)
{
    // Variables...
    IplImage           *pGrayImage              = NULL;
    wxString            sTemp;

    // Load the image...
    pGrayImage = cvLoadImage(sPath.mb_str(), CV_LOAD_IMAGE_GRAYSCALE);

        // Failed to load media...
        if(!pGrayImage)
        {
            // Alert...
            wxLogError(wxT("Unable to load image. It may be in an unrecognized"
                           " format."));
            
            // Abort...
            return;
        }

    // Feed into tracker...
    Frame.Tracker.Advance(*pGrayImage);
    
    // Cleanup gray image...
    cvReleaseImage(&pGrayImage);
}

// Analyze video...
void AnalysisThread::AnalyzeVideo(wxString sPath)
{
    // Variables...
    IplImage           *pGrayImage              = NULL;
    wxString            sTemp;

    // Initialize capture from AVI...
    pCapture = cvCreateFileCapture(sPath.fn_str());

        // Failed to load media...
        if(!pCapture)
        {
            // Alert...
            wxLogError(wxT("Your system does not appear to have an suitable"
                           " codec installed to read this media."));
            
            // Abort...
            return;
        }

    // Start the analysis stop watch...
    StatusUpdateStopWatch.Start();

    // Keep showing media until there is nothing left or cancel requested...
    while(!TestDestroy())
    {
        // Depending on processor throttle setting, idle...
        if(Frame.ProcessorThrottle->GetValue() < 100)
        {
            // Get the throttle value...
            int const nThrottle = Frame.ProcessorThrottle->GetValue();
            
            // Compute sleep time from slider value... [1,2000ms]
            int nSleepTime = ((100 - nThrottle) * 20);

            // Give up rest of time slice to system for other threads...
            Yield();
            
            // Sleep...
            wxThread::Sleep(nSleepTime);
            
            // Throttle was at zero, don't bother doing any processing...
            if(nThrottle == 0)
                continue;
        }

        // Retrieve the captured image...
        IplImage const *pOriginalImage = cvQueryFrame(pCapture);
        
            // There are no more...
            if(!pOriginalImage)
                break;

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

        // The tracker prefers grayscale 8-bit unsigned format, prepare...

            // Allocate blank grayscale image...
            pGrayImage = 
                cvCreateImage(cvGetSize(pOriginalImage), IPL_DEPTH_8U, 1);

            // Convert original to grayscale...
            cvConvertImage(pOriginalImage, pGrayImage);

        // Feed into tracker...
        Frame.Tracker.Advance(*pGrayImage);
        
        // Cleanup gray image...
        cvReleaseImage(&pGrayImage);
    }

    // Release the capture source...
    cvReleaseCapture(&pCapture);
}

// Analysis thread exitting callback...
void AnalysisThread::OnExit()
{
    // Stop the analysis timer...
    Frame.AnalysisTimer.Stop();

    // Inform main thread in a thread safe way that analysis has terminated...
        
        // Initialize event...
        wxCommandEvent Event(wxEVT_COMMAND_BUTTON_CLICKED, 
                             MainFrame::ID_ANALYSIS_ENDED);
        Event.SetInt(true);

        // Send in a thread-safe way...
        wxPostEvent(&Frame, Event);
}

