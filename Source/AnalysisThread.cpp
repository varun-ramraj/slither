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
    Frame.Tracker.Reset(0);
    
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
        if(!MediaFile.IsOk()) {
            int a = 10;
	    void *myVoidPtr = &a;
	    return myVoidPtr; // 2020/06/10 - need to return a void pointer 
	}
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
    wxString            sTemp;

    // Reset the tracker, if not already...
    Frame.Tracker.Reset(0);

    // Load the image...
    //  2020/06/10 - updating to cv::IMREAD_* constants
    // and general C++ best practices
    //pGrayImage = cvLoadImage(sPath.mb_str(), CV_LOAD_IMAGE_GRAYSCALE);
    //pGrayImage = cvLoadImage(sPath.mb_str(), cv::IMREAD_GRAYSCALE);
    
    string sPathStr(sPath.mb_str());

    cv::Mat matImage = cv::imread(sPathStr, cv::IMREAD_GRAYSCALE);
    IplImage copyImg = cvIplImage(matImage);
    IplImage *pGrayImage = &copyImg;
     

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
//  2020/06/10 - updated to use renamed functions
// in OpenCV 4
void AnalysisThread::AnalyzeVideo(wxString sPath)
{
    // Variables...
    IplImage           *pGrayImage              = NULL;
    wxString            sTemp;
    cv::VideoCapture	slitherCap;

    // Initialize capture from AVI...
    //  2020/06/10 - renamed function for OpenCV4
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

    // Reset the tracker, if not already...
    //  2020/06/10 - I think the new constant needs to be used
    Frame.Tracker.Reset((unsigned int) 
        cvGetCaptureProperty(pCapture, cv::CAP_PROP_FRAME_COUNT));

    // Start the analysis stop watch...
    StatusUpdateStopWatch.Start();

    // Keep showing media until there is nothing left or cancel requested...
    while(!TestDestroy())
    {
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
	    //  2020/06/10 - updating to new constants in OpenCV4
            int const nCurrentFrame = (int) 
                cvGetCaptureProperty(pCapture, cv::CAP_PROP_POS_FRAMES);

            // Get total number of frames...
            int const nTotalFrames = (int) 
                cvGetCaptureProperty(pCapture, CV_CAP_PROP_FRAME_COUNT);

            // Reached the end...
            if(nCurrentFrame + 1 == nTotalFrames)
                break;

        #endif

	// 2020/06/10 - converting to C++ API using cv::Mat
        // The tracker prefers grayscale 8-bit unsigned format, prepare...
	
	    cv::Mat pOriginalMatImage = cv::cvarrToMat(pOriginalImage); 
	    cv::Mat pGrayMatImage(cvGetSize(pOriginalImage), CV_8UC1);
	    //convert to grayscale
	    cv::cvtColor(pOriginalMatImage, pGrayMatImage, CV_8UC1);
            // Allocate blank grayscale image...
            //pGrayImage = 
            //    cvCreateImage(cvGetSize(pOriginalImage), IPL_DEPTH_8U, 1);

            // Convert original to grayscale...
            //cvConvertImage(pOriginalImage, pGrayImage);
	    
	      
	    //replace pGrayImage
	    IplImage copyGrayImg = cvIplImage(pGrayMatImage);
	    pGrayImage = &copyGrayImg;
        
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

