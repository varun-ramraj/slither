/*
  Name:         VideosGridDropTarget.cpp (implementation)
  Author:       Kip Warner (Kip@TheVertigo.com)
  Description:  Our derivation of wxFileDropTarget...
*/

// Includes...
#include "VideosGridDropTarget.h"
#include <wx/longlong.h>

// Constructor...
VideosGridDropTarget::VideosGridDropTarget(MainFrame *_pMainFrame)
    : pMainFrame(_pMainFrame)
{

}

// We override here to receive dropped files...
bool VideosGridDropTarget::OnDropFiles(wxCoord x, wxCoord y, 
                                       const wxArrayString& FileNames)
{
    // Constants...
    const wxULongLong   ulMegaByte  = 1024 * 1024;
    
    // Variables...
    wxULongLong         ulTotalSize = 0;
    wxULongLong         ulFileSize  = 0;
    unsigned int        unProgress  = 0;
    int                 nRow        = 0;

    // We were dropped a Slither experiment...
    wxFileName ExperimentFile(FileNames[0]);
    if(ExperimentFile.GetExt().Lower() == wxT("sex"))
    {
        // Extract unnormalized file name...
        wxString sFileName = FileNames[0];

        // Win32 passes short form, normalize...
        wxFileName NormalizedFileName(sFileName);
        NormalizedFileName.Normalize(wxPATH_NORM_LONG | wxPATH_NORM_DOTS |
                                     wxPATH_NORM_TILDE | wxPATH_NORM_ABSOLUTE);

        // Store the experiment name...
      ::wxGetApp().sExperimentRequestedFromShell = 
            NormalizedFileName.GetFullPath();

        // Tell the main frame to try opening the experiment now...
        pMainFrame->ProcessCommand(wxID_OPEN);
        
        // Report drag and drop operation ok to shell...
        return true;
    }

    // Calculate total video size...
    for(unsigned int unIndex = 0; unIndex < FileNames.GetCount(); unIndex++)
    {
        // Find the video...
        wxFileName VideoFile(FileNames[unIndex]);

            // Failed...
            if(!VideoFile.IsOk())
                return false;

        // Verify it is of the write format...
        if(!(VideoFile.GetExt().Lower() == wxT("mov") ||
             VideoFile.GetExt().Lower() == wxT("avi") ||
             VideoFile.GetExt().Lower() == wxT("mpg") ||
             VideoFile.GetExt().Lower() == wxT("mpeg")))
        {
            // Log it...
            wxLogError(VideoFile.GetFullName() + 
                       wxT(" is not a valid video format..."));

            // Abort...
            return false;
        }

        // Update total size...
        ulTotalSize += VideoFile.GetSize();
    }

        // Nothing to add...
        if(ulTotalSize == 0)
            return false;

        // Convert bytes to megabytes...
        ulTotalSize /= ulMegaByte;

    // Alert user...
    if(wxMessageBox(wxString::Format(wxT("Add %s MB of video footage to your"
                                         " experiment?"),
                                     ulTotalSize.ToString().c_str()),
                    wxT("New Video Footage"), wxOK | wxCANCEL, 
                    pMainFrame) == wxCANCEL)
        return false;

    // Initialize progress dialog...
    wxProgressDialog ProgressDialog(wxT("Video"), 
                                    wxT("Adding new video content..."), 
                                    FileNames.GetCount(), NULL, 
                                    wxPD_APP_MODAL | wxPD_AUTO_HIDE | 
                                    wxPD_SMOOTH);

    // Set progress bar to zero...
    unProgress = 0;
    ProgressDialog.Update(unProgress);

    // Add the videos...
    for(unsigned int unIndex = 0; unIndex < FileNames.GetCount(); unIndex++)
    {
        // Find the video...
        wxFileName VideoFile(FileNames[unIndex]);

            // Failed...
            if(!VideoFile.IsOk())
                continue;

        // Check for duplicate name in cache...
        if(pMainFrame->IsExperimentContainVideo(VideoFile.GetFullName()))
        {
            // Prepare error message box...
            wxMessageDialog 
                Message(pMainFrame, 
                        wxT("Your experiment already contains a video by the"
                            " name of:\n\n\t") + VideoFile.GetFullName() + 
                            wxT(".\n\nYou might want to rename it and try"
                                " again."), wxT("Duplicate video"), 
                        wxICON_EXCLAMATION);
                            
            // Show it...
            Message.ShowModal();
            
            // Skip video...
            continue;
        }

        // Copy the file into the cache...

            // Create path to video to copy into cache...
            wxString sDestination = pMainFrame->pExperiment->GetCachePath() + 
                                wxT("/videos/") + VideoFile.GetFullName();

            // Copy into cache and check for error...
            if(!::wxCopyFile(FileNames[unIndex], sDestination, true))
            {
                // Alert user...
                wxLogError(wxT("Unable to copy file into experiment..."));

                // Skip...
                continue;
            }

            // Trigger need save, since experiment is now modified...
            pMainFrame->pExperiment->TriggerNeedSave();

            // Update progress, but check for user abort...
            if(!ProgressDialog.Update(unProgress++, 
                                      wxT("Please wait while adding ") + 
                                        FileNames[unIndex]))
            {
                // Alert user...
                wxMessageBox(wxT("The adding of new video was cancelled..."));
                
                // Cleanup...
                return false;
            }

        // Add new row to videos grid and check for error...
        /*nRow = pMainFrame->VideosGrid->YToRow(y);
        nRow = nRow == wxNOT_FOUND ? 0 : nRow;*/
        nRow = 0;
        if(!pMainFrame->VideosGrid->InsertRows(nRow))
            continue;

        // Update each column...
        
            // Title...
            pMainFrame->VideosGrid->SetCellValue(nRow, MainFrame::TITLE, 
                                                 VideoFile.GetFullName());
            
            // Date...
            wxDateTime LastModificationTime;
            VideoFile.GetTimes(NULL, &LastModificationTime, NULL);
            pMainFrame->VideosGrid->SetCellValue(nRow, MainFrame::DATE,
                LastModificationTime.FormatDate());

            // Time...
            pMainFrame->VideosGrid->SetCellValue(nRow, MainFrame::TIME,
                LastModificationTime.FormatTime());
            
            // Technician...
            pMainFrame->VideosGrid->SetCellValue(nRow, MainFrame::TECHNICIAN,
              ::wxGetUserId());

            // Length...
            pMainFrame->VideosGrid->SetCellValue(nRow, MainFrame::LENGTH,
                wxT("?"));

            // Size...
            ulFileSize = VideoFile.GetSize();
            ulFileSize /= ulMegaByte;
            pMainFrame->VideosGrid->SetCellValue(nRow, MainFrame::SIZE,
                ulFileSize.ToString() + wxT(" MB"));

            // Notes...
            pMainFrame->VideosGrid->SetCellValue(nRow, MainFrame::NOTES,
                wxT("You may place whatever you like here..."));
    }
    
    // Update total embedded videos count...
    wxString sEmbeddedVideos;
    sEmbeddedVideos << pMainFrame->VideosGrid->GetNumberRows();
    pMainFrame->EmbeddedVideos->ChangeValue(sEmbeddedVideos);
    
    // Set the new total size...
    ulTotalSize = pMainFrame->GetTotalVideoSize() / (1024 * 1024);
    pMainFrame->TotalSize->ChangeValue(ulTotalSize.ToString() + wxT(" MB"));

    // Done...
    return true;
}
