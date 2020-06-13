/*
  Name:         MediaGridDropTarget.h (definition)
  Author:       Kip Warner (Kip@TheVertigo.com)
  Description:  MediaGridDropTarget class...
*/

// Multiple include protection...
#ifndef _MEDIAGRIDDROPTARGET_H_
#define _MEDIAGRIDDROPTARGET_H_

// Includes...

    // For interacting with UI...
    #include "MainFrame.h"
    
    // For moving videos around into the experiment...
    #include "Experiment.h"
    
    // wxWidgets...
    #include <wx/dnd.h>
    #include <wx/grid.h>

// MediaGridDropTarget class...
class MediaGridDropTarget : public wxFileDropTarget
{
    // Public stuff...
    public:

        // Constructor...
        MediaGridDropTarget(MainFrame *_pMainFrame);

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
