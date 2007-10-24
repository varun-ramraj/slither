/*
  Name:         CheckForUpdateThread.cpp (implementation)
  Author:       Kip Warner (Kip@TheVertigo.com)
  Description:  CheckForUpdateThread class...
*/

// Includes...
#include "CheckForUpdateThread.h"
#include "MainFrame.h"
#include <wx/fs_inet.h>
#include <sstream>

// Thread constructor locks UI...
CheckForUpdateThread::CheckForUpdateThread(MainFrame &_Frame, bool _bSilent)
    :  wxThread(wxTHREAD_JOINABLE),
       Frame(_Frame),
       bSilent(_bSilent)
{
    // Disabled for now...
    assert(false);
}

// Thread entry point...
void *CheckForUpdateThread::Entry()
{
    // Variables...
    wxString    sMessage;
    char        cDummy      = '\x0';
    char        szTemp[128] = {0};

    // Incase we were invoked on startup, wait until main loop is running...
    while(!::wxGetApp().IsMainLoopRunning())
        wxThread::Sleep(50);

    // Initialize event we will use to inform main thread...
    wxCommandEvent Event(wxEVT_COMMAND_BUTTON_CLICKED, 
                         MainFrame::ID_CHECK_FOR_UPDATE_DONE);

    // Add internet I/O file system handler...
    wxFileSystem::AddHandler(new wxInternetFSHandler);
    
    // Initialize file system...
    wxFileSystem    FileSystem;
    
    // Prepare download...
    wxFSFile *pDownload = FileSystem.OpenFile(
        wxT("http://slither:slither@slither.thevertigo.com/update.txt"));

        // Check for error... (e.g. 404 or timeout)
        if(!pDownload)
        {
            // This is an important message...
            Event.SetInt(true);
            
            // Pass along the update message...
            Event.SetString(wxT("Unable to check the internet for an update. "
                                "Either the server or your connection may be "
                                "down."));

            // Send in a thread-safe way...
            wxPostEvent(&Frame, Event);

            // Abort...
            return NULL;
        }

    // Initialize input stream...
    wxInputStream *pInputStream = pDownload->GetStream();
    
        // Check for error...
        if(!pInputStream || !pInputStream->IsOk())
        {
            // This is an important message...
            Event.SetInt(true);
            
            // Pass along the update message...
            Event.SetString(wxT("A problem occured while reading the response "
                                "from the update server."));

            // Send in a thread-safe way...
            wxPostEvent(&Frame, Event);

            // Abort...
            return NULL;
        }

    // Static sized buffer to hold file...
    char szUpdateFile[1024];
    memset(szUpdateFile, 0, sizeof(szUpdateFile));
    
    // Grab file...
    pInputStream->Read(szUpdateFile, sizeof(szUpdateFile) - 1);

    // Cleanup...
    delete pDownload;

    // Compare our version to theirs...
    
        // Format ours...
        int nOurMajor, nOurMinor, nOurRelease = 0;
        std::istringstream OurStringStream(PACKAGE_VERSION);
        OurStringStream >> nOurMajor >> cDummy >> nOurMinor >> cDummy 
                        >> nOurRelease;

        
        // Format theirs...
        int nTheirMajor, nTheirMinor, nTheirRelease = 0;
        std::istringstream TheirStringStream(szUpdateFile);
        TheirStringStream >> nTheirMajor >> cDummy >> nTheirMinor >> cDummy 
                          >> nTheirRelease;

        // Compare...
        
            // Format ours to integral value...
            unsigned int nOurs, nTheirs = 0;
            sprintf(szTemp, "%.2d%.2d%.2d", nOurMajor, nOurMinor, nOurRelease);
            nOurs = atoi(szTemp);
            
            // Format theirs to integral value...
            sprintf(szTemp, "%.2d%.2d%.2d", nTheirMajor, nTheirMinor, 
                    nTheirRelease);
            nTheirs = atoi(szTemp);
            
            // Ours is newer...
            if(nOurs > nTheirs)
            {
                // Format message...
                sMessage = wxT("You are using a newer version of Slither than"
                               " what is publicly available.");
                
                // This is not an important message...
                Event.SetInt(false);
            }

            // Ours is up to date...
            else if(nOurs == nTheirs)
            {
                // Format message...
                sMessage = wxT("You are using the latest version of Slither.");
                
                // This is not an important message...
                Event.SetInt(false);
            }
                
            // A newer version is available...
            else
            {
                // Format message...
                sMessage.Printf(wxT("A newer version of Slither is now"
                                    " available, version %d.%d.%d."),
                                nTheirMajor, nTheirMinor, nTheirRelease);
                
                // This is an important message...
                Event.SetInt(true);
            }

    // Inform main thread in a thread safe way that check for update is done...

        // Pass along the update message...
        Event.SetString(sMessage);

        // Send in a thread-safe way...
        wxPostEvent(&Frame, Event);

    // Done...
    return NULL;
}

// Are we silent?
bool CheckForUpdateThread::IsSilent() const
{
    // Return it...
    return bSilent;
}

