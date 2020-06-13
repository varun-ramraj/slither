/*
  Name:         ImageAnalysisWindow.h (definition)
  Author:       Kip Warner (Kip@TheVertigo.com)
  Description:  The image analysis window...
*/

// Multiple include protection...
#ifndef _IMAGEANALYSISWINDOW_H_
#define _IMAGEANALYSISWINDOW_H_

// Includes...
#include "Resources.h"
#include "opencv2/opencv.hpp"

// 2020/06/10 - Deprecated, switching to
//new highgui header
//#include "opencv/highgui.h"
#include "opencv2/highgui.hpp"

// Forward declarations...
class MainFrame;

// Image analysis dialog class...
class ImageAnalysisWindow : public ImageAnalysisWindow_Base
{
    // Public stuff...
    public:

        // Methods...

            // Default constructor...
            ImageAnalysisWindow(MainFrame *Parent);
            
            // Set the image to display...
            void SetImage(IplImage const &IntelImage);
            
            // Deconstructor...
           ~ImageAnalysisWindow();

    // Protected stuff...
    protected:

        // Methods...

            // Close command event handler...
            void OnClose(wxCloseEvent &Event);

            // Repaint time...
            void OnPaint(wxPaintEvent &Event);
        
        // Attributes...
        
            // The bitmap to render...
            wxBitmap    Bitmap;
};

#endif

