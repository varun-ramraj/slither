/*
  Name:         Experiment.cpp (implementation)
  Author:       Kip Warner (Kip@TheVertigo.com)
  Description:  An experiment class...
*/

// Includes...
#include "Experiment.h"

// Constructor...
Experiment::Experiment(MainFrame *_pMainFrame)
    :   pMainFrame(_pMainFrame),
        sPath(wxEmptyString),
        sCachePath(wxEmptyString),
        bLoadOk(true),
        bNeedSave(false)
{
    // Fill in UI elements with default values...
    pMainFrame->ExperimentTitle->ChangeValue(wxT("My New Experiment"));
    pMainFrame->EmbeddedMedia->ChangeValue(wxT("0"));
    pMainFrame->TotalSize->ChangeValue(wxT("0 KB"));
    pMainFrame->ExperimentNotes->ChangeValue(wxT("Store any experiment notes here..."));

    // Create a temporary directory...
    sCachePath = CreateTempDirectory();

        // Open it...
        wxDir ExperimentCacheDirectory(sCachePath);
        ExperimentCacheDirectory.IsOpened();

    // Enable UI...
    EnableUI(true);
}

// Clear need save flag...
void Experiment::ClearNeedSave()
{
    // Update title bar...
    pMainFrame->SetTitle(wxT(PACKAGE_STRING));

    // Untoggle...
    bNeedSave = false;
}

// Get a temporary directory...
wxString Experiment::CreateTempDirectory() const
{
    // Variables...
    int         nAttempts       = 0;
    wxString    sCheckCachePath;
    wxString    sTempDirectory;

    // Query standard paths for temp directory...
    sTempDirectory = wxStandardPaths::Get().GetTempDir();
    if(sTempDirectory[sTempDirectory.length() - 1] == wxFILE_SEP_PATH)
        sTempDirectory = sTempDirectory.RemoveLast();

    // Look for an unused subdirectory name...
    do
    {   
        // Make a guess...
        sCheckCachePath = 
            wxString::Format(wxT("%s/SlitherCache-%.2d"), 
                             sTempDirectory.c_str(), nAttempts++);
    }
    
    // Break when we find an unused directory name...
    while(::wxDirExists(sCheckCachePath));
        
    // Create sub-directory...
    if(!wxFileName::Mkdir(sCheckCachePath))
    {
        // Alert user...
        wxMessageBox(wxT("Unable to create temporary directory..."));
        
        // Exit...
        wxExit();
    }
    
    // Create skeleton subdirectory structure...
    wxFileName::Mkdir(sCheckCachePath + wxT("/control/"));
    wxFileName::Mkdir(sCheckCachePath + wxT("/media/"));

    // Return directory to caller...
    return sCheckCachePath;
}

// Enable or disable UI, and optionally reset GUI...
void Experiment::EnableUI(bool bEnable, bool bReset)
{
    // Toggle menus...
    pMainFrame->GetMenuBar()->Enable(wxID_SAVE, bEnable);
    pMainFrame->GetMenuBar()->Enable(wxID_SAVEAS, bEnable);
    pMainFrame->GetMenuBar()->Enable(wxID_REVERT, bEnable);
    pMainFrame->GetMenuBar()->Enable(wxID_CLOSE, bEnable);
    
    // Toggle tools...
    pMainFrame->GetToolBar()->EnableTool(wxID_SAVE, bEnable);
    pMainFrame->GetToolBar()->EnableTool(wxID_SAVEAS, bEnable);

    // Toggle notebook panels... (and all child controls)
    pMainFrame->DataPane->Enable(bEnable);
    pMainFrame->CapturePane->Enable(bEnable);
    pMainFrame->AnalysisPane->Enable(bEnable);    
    
    // Clear out UI elements if requested...
    if(bReset)
    {
        // Fields...
        pMainFrame->ExperimentTitle->Clear();
        pMainFrame->EmbeddedMedia->Clear();
        pMainFrame->TotalSize->Clear();
        pMainFrame->ExperimentNotes->Clear();
        
        // Clear media grid...
        while(pMainFrame->MediaGrid->GetNumberRows() > 0)
            pMainFrame->MediaGrid->DeleteRows();
        
        // Stop media playback...
        if(pMainFrame->pMediaPlayer)
            pMainFrame->pMediaPlayer->Stop();
        
        // Remove save flag in title bar, if any...
        ClearNeedSave();
    }
}


// Get the path to experiment cache...
wxString &Experiment::GetCachePath()
{
    // Return it...
    return sCachePath;
}


// Get the full path, file name, and extension to file on disk...
wxString &Experiment::GetPath()
{
    // Return it...
    return sPath;
}

// Get new unique cache file name...
wxString Experiment::GetUniqueCacheFileName() const
{
    // Objects...
    wxFileName  UniqueCacheFileName;
    
    // Generate and return unique cache file name...
    return UniqueCacheFileName.CreateTempFileName(sCachePath + wxT("/"));
}

// Has this file ever been saved?
bool Experiment::IsEverBeenSaved() const
{
    // Check...
    return (sPath != wxEmptyString);
}

// Does the experiment need to be saved?
bool Experiment::IsNeedSave() const
{
    // Check...
    return bNeedSave;
}

// Experiment loaded ok?
bool Experiment::IsLoadOk() const
{
    // Check...
    return bLoadOk;
}

// Load experiment...
bool Experiment::Load(const wxString _sPath)
{
    // Variables...
    unsigned int    unProgress  = 0;
    wxULongLong     ulTotalSize = 0;

    // Disable load flag until we are done loading...
    bLoadOk = false;

    // Disable and reset the UI...
    EnableUI(false, true);
    
    // Store the path...
    sPath = _sPath;
    
    // Open the experiment...
    wxFFileInputStream InputStream(sPath);

        // Failed...
        if(!InputStream.IsOk())
            return false;

    // Pass the input stream through the zip input stream for decompression...
    wxZipInputStream ZipInputStream(InputStream);

    // Initialize progress dialog...
    wxProgressDialog *pProgressDialog = new 
        wxProgressDialog(wxT("Loading"),
                         wxT("Decompressing experimental media..."), 
                         ZipInputStream.GetTotalEntries(), NULL, 
                         wxPD_APP_MODAL | wxPD_AUTO_HIDE | wxPD_SMOOTH);

    // Set progress bar to zero...
    unProgress = 0;
    pProgressDialog->Update(unProgress);

    // Now unpack the experiment into the cache...
    while(wxZipEntry *pZipEntry = ZipInputStream.GetNextEntry())
    {
        // Get the name of the entry...
        wxString sName = pZipEntry->GetName();

        // Update progress, but check for user abort...
        if(!pProgressDialog->Update(unProgress++))
        {
            // Alert user...
            wxMessageBox(wxT("The experiment load was cancelled..."));
            
            // Cleanup...
            EnableUI(false, true);
            return false;
        }

        // Is this a directory?
        if(pZipEntry->IsDir())
        {   
            // It doesn't exist yet...
            if(!::wxDirExists(sCachePath + wxT("/") + sName))
            {
                // Create directory and check for error...
                if(!wxFileName::Mkdir(sCachePath + wxT("/") + sName))
                {
                    // Alert user...
                    wxMessageBox(wxT("Can't create ") + sCachePath + wxT("/") + 
                                 sName);
                    
                    // Cleanup...
                    EnableUI(false, true);
                    return false;
                }
            }
        }
        
        // This is a file...
        else
        {
            // Create output file for writing...
            wxFFileOutputStream OutputStream(sCachePath + wxT("/") + sName);

                // Failed...
                if(!OutputStream.IsOk())
                {
                    // Alert user...
                    wxMessageBox(wxT("Can't create ") + sCachePath + wxT("/") + 
                                 sName);
                    
                    // Cleanup...
                    EnableUI(false, true);
                    return false;
                }

            // Decompress file out to disk...
            ZipInputStream.Read(OutputStream);
            
                // Failed...
                if(!OutputStream.IsOk())
                {
                    // Alert user...
                    wxMessageBox(wxT("Can't write to ") + sCachePath + wxT("/") 
                                 + sName);
                    
                    // Cleanup...
                    EnableUI(false, true);
                    return false;
                }

            // Close output stream and cleanup...
            OutputStream.Close();
        }
    }
    
    // Parse control data...
            
        // Allocate and load...
        wxXmlDocument   XmlControlDocument(GetCachePath() + 
                                           wxT("/control/control.xml"));
        
        // Find the root node and verify it is the control node...
        if(!XmlControlDocument.IsOk() || 
           XmlControlDocument.GetRoot()->GetName() != wxT("control"))
        {
            // Alert user...
            wxLogError(wxT("No valid meta data was found. Are you sure this is"
                           " a Slither experiment?"));

            // Cleanup...
            EnableUI(false, true);
            return false;
        }

        // Parse each child node of control...
        wxXmlNode *pChildNode = 
            XmlControlDocument.GetRoot()->GetChildren();
        while(pChildNode)
        {
            // Experiment title...
            if(pChildNode->GetName() == wxT("title"))
            {
                // Set it...
                pMainFrame->ExperimentTitle->
                    ChangeValue(pChildNode->GetNodeContent());
            }

            // Experiment notes...
            else if(pChildNode->GetName() == wxT("notes"))
            {
                // Set it...
                pMainFrame->ExperimentNotes->
                    ChangeValue(pChildNode->GetNodeContent());
            }

            // Experimental media...
            else if(pChildNode->GetName() == wxT("media"))
            {
                // Parse each media file...
                wxXmlNode *pMediaNode = pChildNode->GetChildren();
                while(pMediaNode)
                {
                    // Unexpected tag...
                    if(pMediaNode->GetName() != wxT("file"))
                    {
                        // Alert user...
                        wxLogWarning(wxT("Unknown tag in metadata: \"") + 
                                 pMediaNode->GetName() + 
                                 wxT("\".\n\nPerhaps a newer save format?"));

                        // Try the next one...
                        pMediaNode = pMediaNode->GetNext();
                        continue;
                    }
                    
                    // Add new row to media grid and check for error...
                    pMainFrame->MediaGrid->InsertRows();
                    int nRow = 0;
                    
                    // Update each column...
                    wxString sValue;
        
                        // Title...
                        sValue = pMediaNode->GetNodeContent();
                        pMainFrame->MediaGrid->
                            SetCellValue(nRow, MainFrame::TITLE, sValue);

                        // Date...
                        sValue = pMediaNode->GetPropVal(wxT("date"), wxEmptyString);
                        pMainFrame->MediaGrid->
                            SetCellValue(nRow, MainFrame::DATE, sValue);

                        // Time...
                        sValue = pMediaNode->GetPropVal(wxT("time"), wxEmptyString);
                        pMainFrame->MediaGrid->
                            SetCellValue(nRow, MainFrame::TIME, sValue);
                        
                        // Technician...
                        sValue = pMediaNode->GetPropVal(wxT("technician"), 
                                                        ::wxGetUserId());
                        pMainFrame->MediaGrid->
                            SetCellValue(nRow, MainFrame::TECHNICIAN, sValue);

                        // Length...
                        pMainFrame->MediaGrid->
                            SetCellValue(nRow, MainFrame::LENGTH, wxT("?"));

                        // Size...
                        
                            // Open media...
                            wxFileName  MediaFile(GetCachePath() + 
                                                  wxT("/media/") + 
                                                  pMediaNode->GetNodeContent());

                            // Calculate size...
                            wxULongLong ulFileSize = 
                                MediaFile.GetSize() / 1024;
                            ulTotalSize += MediaFile.GetSize();

                            // Format and add to grid...
                            pMainFrame->MediaGrid->SetCellValue(nRow, 
                                MainFrame::SIZE, ulFileSize.ToString() + 
                                wxT(" KB"));

                        // Notes...
                        sValue = pMediaNode->GetPropVal(wxT("notes"), 
                                                        wxEmptyString);
                        pMainFrame->MediaGrid->
                            SetCellValue(nRow, MainFrame::NOTES, sValue);
                    
                    // Seek to the next media node...
                    pMediaNode = pMediaNode->GetNext();
                }
            }

            // Unknown tag...
            else
                wxLogWarning(wxT("Unknown tag in metadata: \"") + 
                             pChildNode->GetName() + 
                             wxT("\".\n\nPerhaps a newer save format?"));

            // Seek to the next child node of the control node...
            pChildNode = pChildNode->GetNext();
        }

    // Set the total embedded media count...
    wxString sEmbeddedMedia;
    sEmbeddedMedia << pMainFrame->MediaGrid->GetNumberRows();
    pMainFrame->EmbeddedMedia->ChangeValue(sEmbeddedMedia);

    // Set the total size...
    ulTotalSize /= 1024;
    pMainFrame->TotalSize->ChangeValue(ulTotalSize.ToString() + wxT(" KB"));

    // Done...

        // Kill the progress dialog so we can enable menu items on Mac...
        delete pProgressDialog;

        // Trigger load ok flag...
        bLoadOk = true;
        
        // Enable UI, but don't clear it...
        EnableUI(true, false);

        // Return ok...
        return true;
}

// Recursively remove directory and all of its contents... (be careful)
bool Experiment::RecursivelyRemoveDirectory(wxString sPath)
{ 
    // Variables...
    bool            bSuccessful             = true;
    wxArrayString   FilesFoundArray;
    wxArrayString   DirectoriesFoundArray;

    // Create directory traverser...
    wxDirectoryScanner Scanner(FilesFoundArray, DirectoriesFoundArray);

    // Bind directory object to path...
    wxDir Directory(sPath);
    
    // Scan directory for contents...
    Directory.Traverse(Scanner);
    
    // Remove all files...
    for(unsigned int i = 0; i < FilesFoundArray.GetCount(); i++)
    {
        // Try and check for error...
        if(!::wxRemoveFile(FilesFoundArray.Item(i)))
        {
            // Log error...
            wxLogError(wxT("Unable to remove file: ") + 
                       FilesFoundArray.Item(i));

            // Remember failure...
            bSuccessful = false;
        }
    }

    // Remove all directories...
    for(unsigned int i = 0; i < DirectoriesFoundArray.GetCount(); i++)
    {
        // Try and check for error...
        if(!::wxRmdir(DirectoriesFoundArray.Item(i)))
        {
            // Log error...
            wxLogError(wxT("Unable to remove directory: ") + 
                       DirectoriesFoundArray.Item(i));

            // Remember failure...
            bSuccessful = false;
        }
    }
    
    // Try to remove the parent directory now...
    if(!::wxRmdir(sPath))
    {
        // Log error...
        wxLogError(wxT("Unable to remove directory: ") + sPath);

        // Remember failure...
        bSuccessful = false;
    }
    
    // Return status...
    return bSuccessful;
}

// Save experiment...
bool Experiment::Save()
{
    // No path to save to...
    if(sPath == wxEmptyString)
        return false;

    // Create the output stream...
    wxFFileOutputStream OutputStream(GetPath());
    
        // Failed...
        if(!OutputStream.IsOk())
            return false;

    // Alert user...
    pMainFrame->SetStatusText(wxT("Saving, please be patient..."));

    // Create the zip output stream...
    wxZipOutputStream ZipOutputStream(OutputStream);

    // Set compression level to store to make it fast...
    ZipOutputStream.SetLevel(0);
    
    // Create the control data stream...
    wxTextOutputStream ControlOutputStream(ZipOutputStream);
    
    // Write control data...
    ZipOutputStream.PutNextDirEntry(wxT("control"));
    ZipOutputStream.PutNextEntry(wxT("control/control.xml"));
    
        // Format...
            
            // Allocate...
            wxXmlDocument   XmlControlDocument;
            
            // Configure...
            XmlControlDocument.SetVersion(wxT("1.0"));
            XmlControlDocument.SetFileEncoding(wxT("UTF-8"));
            
            // Create control element node...
            wxXmlNode *XmlControlNode = new 
                wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("control"), 
                               wxEmptyString); 

            // Set the root node...
            XmlControlDocument.SetRoot(XmlControlNode);
            
                // Set title element node...
                wxXmlNode *XmlTitleElementNode = new 
                wxXmlNode(XmlControlDocument.GetRoot(), wxXML_ELEMENT_NODE, 
                          wxT("title"), wxEmptyString);
                          
                    // Set title child node...
                    wxXmlNode *XmlTitleChildNode = NULL;
                    XmlTitleChildNode = new 
                    wxXmlNode(XmlTitleElementNode, wxXML_TEXT_NODE, 
                              wxEmptyString, 
                              pMainFrame->ExperimentTitle->GetValue());
                    
                // Set notes element node...
                wxXmlNode *XmlNotesElementNode = new 
                wxXmlNode(XmlControlDocument.GetRoot(), wxXML_ELEMENT_NODE, 
                          wxT("notes"), wxEmptyString);
                          
                    // Set notes child node...
                    wxXmlNode *XmlNotesChildNode = NULL;
                    XmlNotesChildNode = new 
                    wxXmlNode(XmlNotesElementNode, wxXML_TEXT_NODE, 
                              wxEmptyString, 
                              pMainFrame->ExperimentNotes->GetValue());

                // Set media element node...
                wxXmlNode *XmlMediaElementNode = new 
                wxXmlNode(XmlControlDocument.GetRoot(), wxXML_ELEMENT_NODE, 
                          wxT("media"), wxEmptyString);

                // Write out each media metadata...
                for(int nRow = 0; 
                    nRow < pMainFrame->MediaGrid->GetNumberRows(); 
                    nRow++)
                {
                    // Set media child node...
                    wxXmlNode *XmlMediaChildNode = new 
                    wxXmlNode(XmlMediaElementNode, wxXML_ELEMENT_NODE, 
                              wxT("file"), wxEmptyString);
                        
                        // Add date property...
                        XmlMediaChildNode->AddProperty(wxT("date"), 
                            pMainFrame->MediaGrid->
                                GetCellValue(nRow, MainFrame::DATE));
                        
                        // Add time property...
                        XmlMediaChildNode->AddProperty(wxT("time"), 
                            pMainFrame->MediaGrid->
                                GetCellValue(nRow, MainFrame::TIME));
                        
                        // Add technician property...
                        XmlMediaChildNode->AddProperty(wxT("technician"), 
                            pMainFrame->MediaGrid->
                                GetCellValue(nRow, MainFrame::TECHNICIAN));
                        
                        // Add notes property...
                        XmlMediaChildNode->AddProperty(wxT("notes"), 
                            pMainFrame->MediaGrid->
                                GetCellValue(nRow, MainFrame::NOTES));
                        
                        // Set the media name child node...
                        wxXmlNode *XmlMediaNameChildNode = NULL;
                        XmlMediaNameChildNode = new 
                        wxXmlNode(XmlMediaChildNode, wxXML_TEXT_NODE, 
                                  wxEmptyString, 
                                  pMainFrame->MediaGrid->
                                    GetCellValue(nRow, MainFrame::TITLE));
                }

        // Write...
        XmlControlDocument.Save(ZipOutputStream, 2);

    // Write media...
    
        // Create directory in archive...
        ZipOutputStream.PutNextDirEntry(wxT("media"));
    
        // Archive each file...
        for(int nRow = 0; 
            nRow < pMainFrame->MediaGrid->GetNumberRows(); 
            nRow++)
        {
            // Try to open the media...
            wxFFileInputStream MediaEntry(GetCachePath() + wxT("/media/") + 
                                            pMainFrame->MediaGrid->
                                            GetCellValue(nRow, MainFrame::TITLE));

                // This shouldn't happen...
                if(!MediaEntry.IsOk())
                {
                    // Log it and skip to next media...
                    wxLogError(wxT("Can't save with experiment: ") +
                               GetCachePath() + wxT("/media/") + 
                                pMainFrame->MediaGrid->
                                    GetCellValue(nRow, MainFrame::TITLE));
                    continue;
                }

            // Create the entry...
            ZipOutputStream.PutNextEntry(wxT("media/") + 
                pMainFrame->MediaGrid->GetCellValue(nRow, MainFrame::TITLE));
                                    
            // Compress the media into the zip...
            ZipOutputStream.Write(MediaEntry);
        }

    // Clear need save flag...
    ClearNeedSave();

    // Done...
    return true;
}

// Save the experiment under a new file name...
bool Experiment::SaveAs(const wxString _sPath)
{
    // Store the new path...
    sPath = _sPath;
    
    // Make sure it has a file extension if none present...
    if(!sPath.Contains(wxT(".")))
        sPath += wxT(".sex");
    
    // Save...
    return Save();
}

// Flag experiment as needing a save...
void Experiment::TriggerNeedSave()
{
    // Update title bar...
    pMainFrame->SetTitle(wxString(wxT("*")) + wxT(PACKAGE_STRING));

    // Set...
    bNeedSave = true;
}

// Deconstructor...
Experiment::~Experiment()
{
    // Disable UI...
    EnableUI(false, true);
    
    // Cleanup cache...
    
        // Alert user...
        pMainFrame->SetStatusText(wxT("Cleaning up, please be patient..."));
        
        // Delete it...
        RecursivelyRemoveDirectory(sCachePath);

    // Done...
    pMainFrame->SetStatusText(wxT("Ready..."));
}

