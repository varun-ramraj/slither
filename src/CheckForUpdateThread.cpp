/*
  Name:         CheckForUpdateThread.cpp (implementation)
  Author:       Kip Warner (Kip@TheVertigo.com)
  Description:  CheckForUpdateThread class...
*/

// Includes...
#include "CheckForUpdateThread.h"
#include "MainFrame.h"

// Thread constructor locks UI...
CheckForUpdateThread::CheckForUpdateThread(MainFrame &_Frame, bool _bSilent)
    :  wxThread(wxTHREAD_JOINABLE),
       Frame(_Frame),
       bSilent(_bSilent)
{

}

// Thread entry point...
void *CheckForUpdateThread::Entry()
{
    // Variables...
    wxString sMessage;
    char     szTemp[128] = {0};

    // Initialize event we will use to inform main thread...
    wxCommandEvent Event(wxEVT_COMMAND_BUTTON_CLICKED, 
                         MainFrame::ID_CHECK_FOR_UPDATE_DONE);

    // Set the URL...
    wxURL ServerURL;
    
    // Set the URL...
    ServerURL.SetURL(wxT("http://slither:slither@slither.thevertigo.com/update.txt"));
    
        // Check for URL error... (shouldn't ever happen on client machine)
        if(ServerURL.GetError() != wxURL_NOERR ||
           !ServerURL.IsOk())
        {
            // Format message...
            sMessage.Printf(wxT("URL error: %d\n"), ServerURL.GetError());
            
            // This is an important message...
            Event.SetInt(true);
            
            // Pass along the update message...
            Event.SetString(sMessage);

            // Send in a thread-safe way...
            wxPostEvent(&Frame, Event);

            // Abort...
            return NULL;
        }

    // Initialize input stream...
    wxInputStream &InputStream = *ServerURL.GetInputStream();

    // Static sized buffer to hold file...
    char szUpdateFile[1024];
    memset(szUpdateFile, 0, sizeof(szUpdateFile));
    
    // Grab file...
    InputStream.Read(szUpdateFile, sizeof(szUpdateFile) - 1);

    // Compare our version to theirs...
    
        // Format ours...
        int nOurMajor, nOurMinor, nOurRelease = 0;
        sscanf(PACKAGE_VERSION, "%d.%d.%d", &nOurMajor, &nOurMinor, 
               &nOurRelease);
        
        // Format theirs...
        int nTheirMajor, nTheirMinor, nTheirRelease = 0;
        sscanf(szUpdateFile, "%d.%d.%d", &nTheirMajor, &nTheirMinor, 
               &nTheirRelease);

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
                                    " available, version %d.%d.%d."));
                
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

