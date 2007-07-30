/*
  Name:         VideosGridDropTarget.h (definition)
  Author:       Kip Warner (Kip@TheVertigo.com)
  Description:  VideosGridDropTarget class...
*/

// Multiple include protection...
#ifndef _VIDEOSGRIDDROPTARGET_H_
#define _VIDEOSGRIDDROPTARGET_H_

// Includes...

    // For interacting with UI...
    #include "MainFrame.h"
    
    // For moving videos around into the experiment...
    #include "Experiment.h"
    
    // wxWidgets...
    #include <wx/dnd.h>
    #include <wx/grid.h>

// VideosGridDropTarget class...
class VideosGridDropTarget : public wxFileDropTarget
{
    // Public stuff...
    public:

        // Constructor...
        VideosGridDropTarget(MainFrame *_pMainFrame);

        // We override here to receive dropped files...
        virtual bool OnDropFiles(wxCoord x, wxCoord y, 
                                 const wxArrayString& FileNames);

    // Private stuff...
    private:
    
        // Objects...
        
            // MainFrame pointer...
            MainFrame *pMainFrame;

};

#endif
