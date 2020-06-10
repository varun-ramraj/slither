/*
  Name:         CaptureThread.h (definition)
  Author:       Kip Warner (Kip@TheVertigo.com)
  Description:  CaptureThread class...
*/

// Multiple include protection...
#ifndef _CAPTURETHREAD_H_
#define _CAPTURETHREAD_H_

// Includes...

    // wxWidgets...
    #include <wx/wx.h>
    
    // OpenCV...
    #include <opencv2/opencv.hpp>

// Forward declarations...
class MainFrame;

// CaptureThread class...
class CaptureThread : public wxThread
{
    // Public methods...
    public:

        // Construct thread...
        CaptureThread(MainFrame *_pMainFrame);
        
        // Thread entry point...
        virtual void *Entry();
        
        // Perform post processing...
        void PerformPostProcessing(IplImage *pIntelImage);

        // Show the OSD over the frame...
        void ShowOnScreenDisplay(IplImage *pIntelImage);

    // Private stuff...
    private:

        // Pointer to main frame to render on...
        MainFrame      *pMainFrame;
};

#endif
