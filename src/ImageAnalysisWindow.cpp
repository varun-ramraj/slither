/*
  Name:         ImageAnalysisWindow.cpp (implementation)
  Author:       Kip Warner (Kip@TheVertigo.com)
  Description:  The image analysis window...
*/

// Includes...
#include "SlitherApp.h"
#include "MainFrame.h"
#include "ImageAnalysisWindow.h"
#include "resources/slither.xpm"

// Constructor...
ImageAnalysisWindow::ImageAnalysisWindow(MainFrame *Parent)
    :   ImageAnalysisWindow_Base(Parent),
        Bitmap(slither_xpm)
{
    // For now, resize window for default image
    SetSize(Bitmap.GetWidth(), Bitmap.GetHeight());
}

// Close command event handler...
void ImageAnalysisWindow::OnClose(wxCloseEvent &Event)
{
    // We can't veto, let application kill us...
    if(!Event.CanVeto())
        return;

    // Just hide the frame... 
    Show(false);
    
    // Untoggle in main frame's menu item...
    ((MainFrame *) GetParent())->GetMenuBar()->
        Check(MainFrame::ID_TOGGLE_IMAGE_ANALYSIS_WINDOW, false);
}

// Repaint time...
void ImageAnalysisWindow::OnPaint(wxPaintEvent &Event)
{
    // Prepare device context...
    wxPaintDC DeviceContext(this);

    // Draw our bitmap...
    DeviceContext.DrawBitmap(Bitmap, 0, 0, true);
}

// Set the image to show...
void ImageAnalysisWindow::SetImage(IplImage const &IntelImage)
{
    // Variables...
    unsigned char          *pRawImageData       = NULL;
    CvSize                  ImageSize           = cvSize(0, 0);

    // Get the frame's raw data...
    cvGetRawData(&IntelImage, &pRawImageData, NULL, &ImageSize);

    // Convert the raw data into something wxWidgets understands...
    wxImage Image = wxImage(IntelImage.width, IntelImage.height, pRawImageData, 
                            true);

    // Turn the image into a bitmap, scaled to the panel's dimensions...
    Bitmap = wxBitmap(Image);
    
    // Resize the window...
    SetSize(Bitmap.GetWidth(), Bitmap.GetHeight());
    
    // Repaint when we get a chance...
    Refresh();
}

// Deconstructor...
ImageAnalysisWindow::~ImageAnalysisWindow()
{

}

