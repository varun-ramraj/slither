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
    
    // OpenCV...
    #include <opencv/cv.h>
    #include <opencv/highgui.h>

// Forward declarations...
class MainFrame;

// AnalysisThread class...
class AnalysisThread : public wxThread
{
    // Public methods...
    public:

        // Construct thread...
        AnalysisThread(MainFrame &_Frame);

        /* Analyze captured frame and return the analyzed version...
        IplImage *AnalyzeFrame(IplImage *pIntelImage);*/

        // Thread entry point...
        virtual void *Entry();
        
        // Convert OpenCV units of area to square micrometres...
        float AreaFromIntelUnitsToMicrometres(float fIntelArea) const;
        
        // Convert OpenCV units of length to micrometres...
        float LengthFromIntelUnitsToMicrometres(float fIntelLength) const;

    // Private members...
    private:

        // Pointer to main frame to render on...
        MainFrame  &Frame;
        
        // Capture handle...
        CvCapture  *pCapture;

    // Private classes...
    private:
    
        // CAnalysisAutoLock class...
        class CAnalysisAutoLock
        {
            // Public...
            public:

                // Constructor locks UI...
                CAnalysisAutoLock(MainFrame &_Frame);
                
                // Deconstructor unlocks UI...
               ~CAnalysisAutoLock();

            // Private...
            private:
            
                // MainFrame location...
                MainFrame &Frame;
        };
};

#endif

