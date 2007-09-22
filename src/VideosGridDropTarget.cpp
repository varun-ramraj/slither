/*
  Name:         MediaGridDropTarget.cpp (implementation)
  Author:       Kip Warner (Kip@TheVertigo.com)
  Description:  Our derivation of wxFileDropTarget...
*/

// Includes...
#include "VideosGridDropTarget.h"
#include <wx/longlong.h>

// Constructor...
MediaGridDropTarget::MediaGridDropTarget(MainFrame *_pMainFrame)
    : pMainFrame(_pMainFrame)
{

}

// We override here to receive dropped files...
bool MediaGridDropTarget::OnDropFiles(wxCoord x, wxCoord y, 
                                      const wxArrayString& FileNames)
{
    // Constants...
    //const wxULongLong   ulMegaByte  = 1024 * 1024;
    const wxULongLong   ulKiloByte  = 1024;
    
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

    // Calculate total media size...
    for(unsigned int unIndex = 0; unIndex < FileNames.GetCount(); unIndex++)
    {
        // Find the media...
        wxFileName MediaFile(FileNames[unIndex]);

            // Failed...
            if(!MediaFile.IsOk())
                return false;

        // Verify it is of the write format...
        if(!(MediaFile.GetExt().Lower() == wxT("mov")   ||
             MediaFile.GetExt().Lower() == wxT("avi")   ||
             MediaFile.GetExt().Lower() == wxT("mpg")   ||
             MediaFile.GetExt().Lower() == wxT("mpeg")  ||
             MediaFile.GetExt().Lower() == wxT("png")   ||
             MediaFile.GetExt().Lower() == wxT("jpg")   ||
             MediaFile.GetExt().Lower() == wxT("jpeg")  ||
             MediaFile.GetExt().Lower() == wxT("bmp")))
        {
            // Log it...
            wxLogError(MediaFile.GetFullName() + 
                       wxT(" is not a valid media format..."));

            // Abort...
            return false;
        }

        // Update total size...
        ulTotalSize += MediaFile.GetSize();
    }

        // Nothing to add...
        if(ulTotalSize == 0)
            return false;

        // Convert bytes to kilobytes...
        ulTotalSize /= ulKiloByte;

    // Alert user...
    if(wxMessageBox(wxString::Format(wxT("Add %s KB of media footage to your"
                                         " experiment?"),
                                     ulTotalSize.ToString().c_str()),
                    wxT("New Media Footage"), wxOK | wxCANCEL, 
                    pMainFrame) == wxCANCEL)
        return false;

    // Initialize progress dialog...
    wxProgressDialog ProgressDialog(wxT("Media"), 
                                    wxT("Adding new media content..."), 
                                    FileNames.GetCount(), NULL, 
                                    wxPD_APP_MODAL | wxPD_AUTO_HIDE | 
                                    wxPD_SMOOTH);

    // Set progress bar to zero...
    unProgress = 0;
    ProgressDialog.Update(unProgress);

    // Add the media...
    for(unsigned int unIndex = 0; unIndex < FileNames.GetCount(); unIndex++)
    {
        // Find the media...
        wxFileName MediaFile(FileNames[unIndex]);

            // Failed...
            if(!MediaFile.IsOk())
                continue;

        // Check for duplicate name in cache...
        if(pMainFrame->IsExperimentContainMedia(MediaFile.GetFullName()))
        {
            // Prepare error message box...
            wxMessageDialog 
                Message(pMainFrame, 
                        wxT("Your experiment already contains media by the"
                            " name of:\n\n\t") + MediaFile.GetFullName() + 
                            wxT(".\n\nYou might want to rename it and try"
                                " again."), wxT("Duplicate Media"), 
                        wxICON_EXCLAMATION);
                            
            // Show it...
            Message.ShowModal();
            
            // Skip media...
            continue;
        }

        // Copy the file into the cache...

            // Create path to media to copy into cache...
            wxString sDestination = pMainFrame->pExperiment->GetCachePath() + 
                                wxT("/media/") + MediaFile.GetFullName();

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
                wxMessageBox(wxT("The adding of new media was cancelled..."));
                
                // Cleanup...
                return false;
            }

        // Add new row to media grid and check for error...
        /*nRow = pMainFrame->MediaGrid->YToRow(y);
        nRow = nRow == wxNOT_FOUND ? 0 : nRow;*/
        nRow = 0;
        if(!pMainFrame->MediaGrid->InsertRows(nRow))
            continue;

        // Update each column...
        
            // Title...
            pMainFrame->MediaGrid->SetCellValue(nRow, MainFrame::TITLE, 
                                                MediaFile.GetFullName());
            
            // Date...
            wxDateTime LastModificationTime;
            MediaFile.GetTimes(NULL, &LastModificationTime, NULL);
            pMainFrame->MediaGrid->SetCellValue(nRow, MainFrame::DATE,
                LastModificationTime.FormatDate());

            // Time...
            pMainFrame->MediaGrid->SetCellValue(nRow, MainFrame::TIME,
                LastModificationTime.FormatTime());
            
            // Technician...
            pMainFrame->MediaGrid->SetCellValue(nRow, MainFrame::TECHNICIAN,
              ::wxGetUserId());

            // Length...
            pMainFrame->MediaGrid->SetCellValue(nRow, MainFrame::LENGTH,
                wxT("?"));

            // Size...
            ulFileSize = MediaFile.GetSize();
            ulFileSize /= ulKiloByte;
            pMainFrame->MediaGrid->SetCellValue(nRow, MainFrame::SIZE,
                ulFileSize.ToString() + wxT(" KB"));

            // Notes...
            pMainFrame->MediaGrid->SetCellValue(nRow, MainFrame::NOTES,
                wxT("You may place whatever you like here..."));
    }
    
    // Update total embedded media count...
    wxString sEmbeddedMedia;
    sEmbeddedMedia << pMainFrame->MediaGrid->GetNumberRows();
    pMainFrame->EmbeddedMedia->ChangeValue(sEmbeddedMedia);
    
    // Set the new total size...
    ulTotalSize = pMainFrame->GetTotalMediaSize() / ulKiloByte;
    pMainFrame->TotalSize->ChangeValue(ulTotalSize.ToString() + wxT(" KB"));

    // Done...
    return true;
}
