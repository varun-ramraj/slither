/*
  Name:         AnalysisThread.h (definition)
  Author:       Kip Warner (Kip@TheVertigo.com)
  Description:  AnalysisThread class...
*/

// Multiple include protection...
#ifndef _ANALYSISTHREAD_H_
#define _ANALYSISTHREAD_H_

// Includes...

    // wxWidgets...
    #include <wx/wx.h>
    #include <wx/stopwatch.h>
    
    // OpenCV...
    #include <opencv/cv.h>
    #include <opencv/highgui.h>
    
    // Worm tracker...
    #include "WormTracker.h"

// Forward declarations...
class MainFrame;

// AnalysisThread class...
class AnalysisThread : public wxThread
{
    // Friends...
    friend class MainFrame;

    // Public methods...
    public:

        // Analysis thread constructor...
        AnalysisThread(MainFrame &_Frame);

        // Analyze single image...
        void AnalyzeImage(wxString sPath);

        // Analyze video...
        void AnalyzeVideo(wxString sPath);

        // Thread entry point...
        virtual void *Entry();

        // Analysis thread exit callback...
        void OnExit();

    // Protected members...
    protected:

        // Pointer to main frame to render on...
        MainFrame          &Frame;

        // Capture handle...
        CvCapture          *pCapture;
        
        // Status update stop watch...
        wxStopWatch         StatusUpdateStopWatch;
};

#endif

