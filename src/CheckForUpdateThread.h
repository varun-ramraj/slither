/*
  Name:         CheckForUpdateThread.h (definition)
  Author:       Kip Warner (Kip@TheVertigo.com)
  Description:  CheckForUpdateThread class...
*/

// Multiple include protection...
#ifndef _CHECKFORUPDATETHREAD_H_
#define _CHECKFORUPDATETHREAD_H_

// Includes...

    // wxWidgets...
    #include <wx/wx.h>
    #include <wx/url.h>

// Forward declarations...
class MainFrame;

// CheckForUpdateThread class...
class CheckForUpdateThread : public wxThread
{
    // Public methods...
    public:

        // Thread constructor...
        CheckForUpdateThread(MainFrame &_Frame, bool _bSilent);

        // Thread entry point...
        virtual void *Entry();
        
        // Are we silent?
        bool IsSilent() const;

    // Protected members...
    protected:

        // Pointer to main frame...
        MainFrame          &Frame;
        
        // Silent mode flag...
        bool                bSilent;

};

#endif

