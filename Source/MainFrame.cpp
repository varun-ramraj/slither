/*
  Name:         MainFrame.cpp (implementation)
  Author:       Kip Warner (Kip@TheVertigo.com)
  Description:  The main window...
*/

// Includes...
#include "MainFrame.h"
#include "VideosGridDropTarget.h"
#include "ImageAnalysisWindow.h"
#include "Experiment.h"
#include "Version.h"
#include <wx/dcbuffer.h>
#include <wx/clipbrd.h>
#include <wx/tokenzr.h>
#include <iostream>
#include <fstream>

// Bitmaps...
#include "Resources/analyze_32x32.xpm"
#include "Resources/play_32x32.xpm"
#include "Resources/record_60x60.xpm"
#include "Resources/remove_32x32.xpm"
#include "Resources/rename_32x32.xpm"
#include "Resources/slither.xpm"
#include "Resources/save_60x60.xpm"
#include "Resources/stop_32x32.xpm"
//#include "Resources/usb_32x32.xpm"

    // Notebook icons... (look terrible on mac, so don't use)
    #ifndef __APPLE__
    #include "Resources/book_64x64.xpm"
    #include "Resources/camera_64x64.xpm"
    #include "Resources/robot_64x64.xpm"
    #endif

// Event table for MainFrame...
BEGIN_EVENT_TABLE(MainFrame, MainFrame_Base)
    
    // Media grid popup menu events...
    EVT_MENU                (ID_ANALYZE, MainFrame::OnAnalyze)
    EVT_MENU                (ID_PLAY, MainFrame::OnPlay)
    EVT_MENU                (ID_REMOVE, MainFrame::OnRemove)
    EVT_MENU                (ID_RENAME, MainFrame::OnRename)    
    EVT_MENU                (ID_STOP, MainFrame::OnStop)

    // Frame moving or resizing...
    EVT_MOVE                (MainFrame::OnMove)

    // Capture...
    EVT_TIMER               (TIMER_CAPTURE, MainFrame::OnCaptureFrameReadyTimer)

    // Analysis grid popup menu events...
    EVT_MENU                (ID_ANALYSIS_COPY_CLIPBOARD,
                                MainFrame::OnAnalysisCopyClipboard)
    EVT_MENU                (ID_ANALYSIS_SAVE_TO_DISK,
                                MainFrame::OnAnalysisSaveToDisk)

    // Analysis...
    EVT_BUTTON              (ID_ANALYSIS_ENDED, MainFrame::OnEndAnalysis)
    EVT_TIMER               (TIMER_ANALYSIS, 
                                MainFrame::OnAnalysisFrameReadyTimer)

END_EVENT_TABLE()

// MainFrame constructor...
MainFrame::MainFrame(wxWindow *Parent)
    : MainFrame_Base(Parent),
      pImageAnalysisWindow(new ImageAnalysisWindow(this)),
      pExperiment(NULL),
      pMediaPlayer(NULL),
      CaptureTimer(this, TIMER_CAPTURE),
      pAnalysisThread(NULL),
      AnalysisTimer(this, TIMER_ANALYSIS)
{
    // Set the title...
    SetTitle(wxT("Slither"));

    // Set the icon...
    SetIcon(slither_xpm);

    /* wxFormBuilder cannot use art provider IDs yet on toolbar...
    wxArtProvider::GetBitmap(wxART_NEW, wxART_TOOLBAR);
    wxArtProvider::GetBitmap(wxART_FILE_OPEN, wxART_TOOLBAR);
    wxArtProvider::GetBitmap(wxART_FILE_SAVE, wxART_TOOLBAR);
    wxArtProvider::GetBitmap(wxART_FILE_SAVE_AS, wxART_TOOLBAR);
    wxArtProvider::GetBitmap(wxART_HELP_SETTINGS, wxART_TOOLBAR);
    wxArtProvider::GetBitmap(wxART_INFORMATION, wxART_TOOLBAR);
    wxArtProvider::GetBitmap(wxART_INFORMATION, wxART_TOOLBAR);*/

    // Configure the notebook...   

        // Prepare graphics...
        #ifndef __APPLE__

            // Create image list... (skip Mac, because looks shitty)
            wxImageList *pNotebookImageList = new wxImageList(64, 64, true, 3);

            // Prepare graphics for each tab...

                // Library tab...
                wxIcon LibraryIcon(book_64x64_xpm);
                pNotebookImageList->Add(LibraryIcon);
                
                // Video capture tab...
                wxIcon FilmIcon(camera_64x64_xpm);
                pNotebookImageList->Add(FilmIcon);

                // Analysis tab...
                wxIcon LensIcon(robot_64x64_xpm);
                pNotebookImageList->Add(LensIcon);
            
            // Bind image list to notebook...
            MainNotebook->AssignImageList(pNotebookImageList);
            MainNotebook->SetPageImage(0, 0);
            MainNotebook->SetPageImage(1, 1);
            MainNotebook->SetPageImage(2, 2);

        #endif

        // Open up to the data pane...
        MainNotebook->ChangeSelection(DATA_PANE);

    // Initialize media grid...

        // Objects...
        wxGridCellAttr *pColumnAttributes   = NULL;

        // Set label sizes...
        MediaGrid->SetRowLabelSize(0);
        MediaGrid->SetColLabelSize(30);

        // Set row sizes...
        MediaGrid->SetRowMinimalAcceptableHeight(25);

        // Set grid colours...
        MediaGrid->SetDefaultCellTextColour(wxColour(wxT("GREEN")));
        MediaGrid->SetGridLineColour(wxColour(wxT("BLUE")));
        MediaGrid->SetDefaultCellBackgroundColour(wxColour(wxT("DIM GREY")));

        // Initialize columns...
        
            // Title...
            MediaGrid->SetColLabelValue(TITLE, wxT("Title"));
            pColumnAttributes = MediaGrid->GetOrCreateCellAttr(0, TITLE);
            pColumnAttributes->SetReadOnly();
            MediaGrid->SetColAttr(TITLE, pColumnAttributes);
            
            // Date...
            MediaGrid->SetColLabelValue(DATE, wxT("Date"));

            // Time...
            MediaGrid->SetColLabelValue(TIME, wxT("Time"));

            // Technician...
            MediaGrid->SetColLabelValue(TECHNICIAN, wxT("Technician"));
            
            // Length...
            MediaGrid->SetColLabelValue(LENGTH, wxT("Length"));
            pColumnAttributes = MediaGrid->GetOrCreateCellAttr(0, LENGTH);
            pColumnAttributes->SetReadOnly();
            MediaGrid->SetColAttr(LENGTH, pColumnAttributes);
            
            // Size...
            MediaGrid->SetColLabelValue(SIZE, wxT("Size"));
            pColumnAttributes = MediaGrid->GetOrCreateCellAttr(0, SIZE);
            pColumnAttributes->SetReadOnly();
            MediaGrid->SetColAttr(SIZE, pColumnAttributes);

            // Notes...
            MediaGrid->SetColLabelValue(NOTES, wxT("Notes"));

        // Automatically resize...
        
            // Pad each column label...
            for(int nColumn = 0; nColumn < MediaGrid->GetNumberCols(); 
                nColumn++)
            {
                // Boost this column...
                MediaGrid->SetColSize(nColumn, 
                    MediaGrid->GetColSize(nColumn) + 55);
            }
        
        // Force a refresh...
        MediaGrid->ForceRefresh();

    // Initialize video player...

        // Allocate media control... (wxFormBuilder can't do this yet via XRC)
        pMediaPlayer = new wxMediaCtrl(
            VideoPreviewPanel, ID_VIDEO_PLAYER, wxEmptyString, wxPoint(0, 0), 
            VideoPreviewPanel->GetSize(), 0, wxEmptyString);

        // Connect events...

            // Loaded enough data that it can start playing...
            Connect(ID_VIDEO_PLAYER, wxEVT_MEDIA_LOADED,
                    wxMediaEventHandler(MainFrame::OnMediaLoaded), 
                    (wxObject*) 0, this);

            // Media has switched to the wxMEDIASTATE_STOPPED state. You may be
            //  able to Veto this event to prevent it from stopping, causing it
            //  to continue playing - even if it has reached that end of the
            //  media (note that this may not have the desired effect - if you
            //  want to loop the media, for example, catch the 
            //  EVT_MEDIA_FINISHED and play there instead). 
            Connect(ID_VIDEO_PLAYER, wxEVT_MEDIA_STOP,
                    wxMediaEventHandler(MainFrame::OnMediaStop), (wxObject*) 0, 
                    this);

            // Switched to the wxMEDIASTATE_PLAYING state...
            Connect(ID_VIDEO_PLAYER, wxEVT_MEDIA_PLAY,
                    wxMediaEventHandler(MainFrame::OnMediaPlay), (wxObject*) 0, 
                    this);

    // Enable it for drag and drop...
    SetDropTarget(new MediaGridDropTarget(this));

    // Configure recording buttons...
    RecordButton->SetBitmapLabel(record_60x60_xpm);
    SaveRecordingButton->SetBitmapLabel(save_60x60_xpm);

    // Setup analysis pane...

        // Initialize microscope table... /* These tables are dummy values */

            /* Leica...
            Microscope Leica(wxT("Leica Wild M3Z"));
            Leica.AddDiameter(6, 55.0f);
            Leica.AddDiameter(40, 7.8f);
            MicroscopeTable.push_back(Leica);
            
            // Random Works...
            Microscope RandomWorks(wxT("Dummy Microscope"));
            RandomWorks.AddDiameter(7, 52.0f);
            RandomWorks.AddDiameter(35, 6.2f);
            MicroscopeTable.push_back(RandomWorks);*/
            
            // Custom microscope...
            Microscope Custom(wxT("Custom"));
            MicroscopeTable.push_back(Custom);
            
        // Populate the microscope name choice box...
        for(unsigned int unIndex = 0; 
            unIndex < MicroscopeTable.size(); unIndex++)
            ChosenMicroscopeName->Append(MicroscopeTable[unIndex].GetName());

        // Trigger default choices...
        wxCommandEvent  DummyCommandEvent;
        DummyCommandEvent.SetInt(0);
        
            // Microscope name...
            ChosenMicroscopeName->SetSelection(0);
            OnChooseMicroscopeName(DummyCommandEvent);

        // Trigger default analysis type choice...

            // Analysis type...
            ChosenAnalysisType->SetSelection(ANALYSIS_BODY_SIZE);
            OnChooseAnalysisType(DummyCommandEvent);

        // Trigger reset AI settings, since under OS X defaults filled...
        OnResetAIToDefaults(DummyCommandEvent);

    // Refresh...
    Refresh();

    // Send dummy size event to frame, forcing re-evaluation of children...
    Layout();
    
    // Adjust media player...
    pMediaPlayer->SetSize(VideoPreviewPanel->GetSize());

    // Set status bar text...
    SetStatusText(wxT("Welcome to Slither..."));
    
    // Restore window size if any available...
    if(::wxGetApp().pConfiguration->HasEntry(wxT("/General/Width")))
    {
        // Read the previously saved values...
        wxSize Size(::wxGetApp().pConfiguration->Read(wxT("/General/Width"), 640),
                    ::wxGetApp().pConfiguration->Read(wxT("/General/Height"), 480));
        
        // Create the size event...
        wxSizeEvent SizeEvent(Size);

        // Fire off resize event...
        wxPostEvent(this, SizeEvent);
    }

    // Show tip...
    ShowTip();

    // Create a new empty experiment...
    pExperiment = new Experiment(this);
}

// Compare two integers. Used for sorting rows in the media grid...
int wxCMPFUNC_CONV MainFrame::CompareIntegers(int *pnFirst, int *pnSecond)
{
    // Compare...
    return *pnFirst - *pnSecond;
}

// Media grid popup menu events for analyze...
void MainFrame::OnAnalyze(wxCommandEvent &Event)
{
    // They don't have a single media file selected...
    if(MediaGrid->GetSelectedRows().GetCount() != 1)
    {
        // Alert user...
        wxMessageBox(wxT("Please select the media you wish to analyze first."));
        
        // Don't change the page...
        return;
    }
    
    // Switch to analysis...
    else
        MainNotebook->ChangeSelection(ANALYSIS_PANE);
}

// User right clicked on analysis grid cell...
void MainFrame::OnAnalysisCellRightClick(wxGridEvent &Event)
{
    // Control down, add to selection...
    if(Event.ControlDown())
        AnalysisGrid->SelectRow(Event.GetRow(), true);

    // Control not down...
    else
    {
        // Over unselected row, select it only...
        if(!AnalysisGrid->IsInSelection(Event.GetRow(), Event.GetCol()))
            AnalysisGrid->SelectRow(Event.GetRow());
    }

    // Create popup menu...
    wxMenu      Menu;
    wxMenuItem *pMenuItem   = NULL;
        
        // Copy to clipboard and check for stable support...
        pMenuItem = new wxMenuItem(&Menu, ID_ANALYSIS_COPY_CLIPBOARD, 
                                   wxT("&Copy to clipboard"));
        pMenuItem->SetBitmap(wxArtProvider::GetBitmap(wxART_COPY, wxART_MENU));
        Menu.Append(pMenuItem);
        
        // Save to file...
        pMenuItem = new wxMenuItem(&Menu, ID_ANALYSIS_SAVE_TO_DISK, 
                                   wxT("&Save to file"));
        pMenuItem->SetBitmap(
            wxArtProvider::GetBitmap(wxART_FILE_SAVE_AS, wxART_MENU));
        Menu.Append(pMenuItem);

    // Popup menu...
    AnalysisGrid->PopupMenu(&Menu, Event.GetPosition());
}

// Copy analysis grid contents to clipboard...
void MainFrame::OnAnalysisCopyClipboard(wxCommandEvent &Event)
{
    // Variables...
    wxString    sContents;
    
    // Lock the clipboard...
    wxClipboardLocker ClipboardLocker;

        // Failed...
        if(!ClipboardLocker)
        {
            // Alert user...
            wxMessageBox(wxT("Unable to access the clipboard."));
            
            // Abort...
            return;
        }

    // Check to make sure format is supported...
    if(!wxTheClipboard->IsSupported(wxDF_TEXT))
    {
        // Alert user...
        wxMessageBox(wxT("Clipboard format is unsupported."));
        
        // Abort...
        return;
    }

    // Get the contents of the analysis results grid...
    sContents = GetAnalysisResults();

    // Add to clipboard...
    wxTheClipboard->SetData(new wxTextDataObject(sContents));
}

// A frame has just been analyzed and is ready to be displayed...
void MainFrame::OnAnalysisFrameReadyTimer(wxTimerEvent &Event)
{
    // Variables...
    wxString sTemp;

    // Get the thinking image...
    IplImage *pThinkingImage = Tracker.GetThinkingImage();
    
        // Not ready yet...
        if(!pThinkingImage)
            return;

    // Display the image...
    pImageAnalysisWindow->SetImage(*pThinkingImage);
    
    // Cleanup...
    cvReleaseImage(&pThinkingImage);

    // Update status every half a second...
    if(pAnalysisThread->StatusUpdateStopWatch.Time() >= 500)
    {
        // Were there any new worms found?
        unsigned int const unWormsJustFound = 
            Tracker.GetWormsAddedSinceLastCheck();

            // Yes, alert user...
            if(unWormsJustFound > 0)
            {
                // Format...
                if(unWormsJustFound == 1)
                    sTemp.Printf(wxT("Found a new worm..."));
                else
                    sTemp.Printf(wxT("Found %d new worms..."), 
                                 unWormsJustFound);

                // Append to analysis status list...
                AnalysisStatusList->Append(sTemp);
            }

        // Get current position...
        int const nCurrentFrame = Tracker.GetCurrentFrameIndex();

        // Get total number of frames...
        int const nTotalFrames = Tracker.GetTotalFrames();

        // Show number tracking...
        sTemp.Printf(wxT("%d"), Tracker.Tracking());
        AnalysisWormsTrackingStatus->ChangeValue(sTemp);
        
        // We have the information we need to compute progress...
        if(nCurrentFrame && nTotalFrames)
        {
            // Compute total progress...
            int const nProgress = 
                (int)(((float) nCurrentFrame / nTotalFrames)  * 100.0);

            // Prepare current frame processing string...
            sTemp.Printf(wxT("%d / %d"), nCurrentFrame, nTotalFrames);

            // Set it only if it has changed...
            if(nProgress != AnalysisGauge->GetValue())
            {
                // Update the current frame processed...
                AnalysisCurrentFrameStatus->ChangeValue(sTemp);

                // Update the progress bar...
                AnalysisGauge->SetValue(nProgress);
            }
        }
        
        // We cannot compute progress because the codec the backend on this
        //  platform is busted to shit... (probably ffmpeg)
        else
        {
            // Prepare current frame processing string...
            sTemp.Printf(wxT("%d"), nCurrentFrame);

            // Update the current frame...
            AnalysisCurrentFrameStatus->ChangeValue(sTemp);

            // Pulse progress bar...
            AnalysisGauge->Pulse();
        }
        
        // Reset the status update timer...
        pAnalysisThread->StatusUpdateStopWatch.Start();
    }
}

// Save the results of the analysis window to disk...
void MainFrame::OnAnalysisSaveToDisk(wxCommandEvent &Event)
{
    // Cache the last location saved to...
    wxStandardPaths StandardPaths = wxStandardPaths::Get();
    static wxString sLastPath = StandardPaths.GetDocumentsDir();

    // Prepare save dialog...
    //  2020/06/10 - updating for wxGTK 3
    wxFileDialog FileDialog(this, wxT("Save analysis results..."), sLastPath, 
                            ExperimentTitle->GetValue() + 
                                wxT(" Analysis Results.txt"),
                            wxT("Tab delimited text file (*.txt)|*.txt"),
                            wxFD_SAVE | wxFD_OVERWRITE_PROMPT | wxFD_PREVIEW);

        // Show save as dialog and check if user hit cancel...
        if(wxID_CANCEL == FileDialog.ShowModal())
            return;

    // Open the file to save to...
    std::ofstream UserFile(FileDialog.GetPath().fn_str(), 
                           std::ios::out | std::ios::trunc);

    // Could not open for writing...
    if(!UserFile.is_open())
    {
        // Alert and abort...
        wxLogError(wxT("There was a problem saving the analysis results."));
        return;
    }

    // Write out the file...
    UserFile << GetAnalysisResults().mb_str();

    // Update the last location saved to...
    sLastPath = ::wxPathOnly(FileDialog.GetPath());
}

// Begin analysis button hit...
void MainFrame::OnBeginAnalysis(wxCommandEvent &Event)
{
    // Variables...
    wxString    sTemp;

    // Analysis already running, abort...
    if(AnalysisTimer.IsRunning())
        return;

    // They don't have a single media selected...
    if(MediaGrid->GetSelectedRows().GetCount() != 1)
    {
        // Alert user...
        wxMessageBox(wxT("You must first select a single piece of media to"
                         " analyze. You will be taken to your available media"
                         " now."));

        // Take the user back to the data pane...
        MainNotebook->ChangeSelection(DATA_PANE);

        // Done...
        return;
    }

    // Load the artificial intelligence settings...
    Tracker.SetArtificialIntelligenceMagic(
        ThresholdSpinner->GetValue(),
        MaxThresholdValueSpinner->GetValue(),
        MinimumCandidateSizeSpinner->GetValue(),
        MaximumCandidateSizeSpinner->GetValue(),
        InletDetectionCheckBox->IsChecked(),
        InletCorrectionSpinner->GetValue());

    // Lock the UI...

        // Begin analysis button...
        BeginAnalysisButton->Disable();

        // Cancel analysis button...
        CancelAnalysisButton->Enable();

        // Microscope set...
        ChosenMicroscopeName->Disable();
        ChosenMicroscopeTotalZoom->Disable();
        FieldOfViewDiameter->Disable();

        // Analysis type...
        ChosenAnalysisType->Disable();

        // Analysis buttons...
        BeginAnalysisButton->Disable();
        CancelAnalysisButton->Enable();

        // Analysis gauge...
        AnalysisGauge->SetRange(100);
        AnalysisGauge->SetValue(0);

        // Clear the status list...
        AnalysisStatusList->Clear();

        // Analysis grid...
        AnalysisGrid->Disable();

        // Artificial intelligence settings...
        AISettingsScrolledWindow->Disable();

        // Alert user...
        sTemp = ChosenAnalysisType->GetString(
                    ChosenAnalysisType->GetCurrentSelection()) + 
                wxT(" analysis is running...");
        AnalysisStatusList->Append(sTemp);

        // Show some information about the AI settings we are using...
        if(InletDetectionCheckBox->IsChecked())
            AnalysisStatusList->Append(wxT("Inlet detection enabled..."));

        // Refresh the main frame...
        Refresh();
        
        // Show the image analysis window...
        pImageAnalysisWindow->Show();

    // Create the analysis thread and check for error...
    pAnalysisThread = new AnalysisThread(*this);
    if(pAnalysisThread->Create() != wxTHREAD_NO_ERROR)
    {
        // Alert...
        wxLogError(wxT("Unable to create AnalysisThread..."));

        // Cleanup and abort...
        delete pAnalysisThread;
        return;
    }

    // Run the thread and check for error...
    pAnalysisThread->SetPriority(WXTHREAD_MIN_PRIORITY);
    if(pAnalysisThread->Run() != wxTHREAD_NO_ERROR)
    {
        // Alert...
        wxLogError(wxT("Unable to start the AnalysisThread..."));

        // Cleanup and abort...
        delete pAnalysisThread;
        return;    
    }
}

// Cancel analysis button hit...
void MainFrame::OnCancelAnalysis(wxCommandEvent &Event)
{
    // Analysis not running, abort...
    if(!AnalysisTimer.IsRunning())
        return;

    // Delete the thread...
    pAnalysisThread->Delete();
}

// An analysis type was chosen...
void MainFrame::OnChooseAnalysisType(wxCommandEvent &Event)
{
    // Objects...
    wxGridCellAttr *pColumnAttributes   = NULL;
    wxString        sTemp;

    // We've only implemented analysis body size so far...
    if(Event.GetSelection() != ANALYSIS_BODY_SIZE)
    {
        // Revert selection to body size analysis...
        wxCommandEvent  DummyCommandEvent;
        DummyCommandEvent.SetInt(0);
        ChosenAnalysisType->SetSelection(ANALYSIS_BODY_SIZE);
        OnChooseAnalysisType(DummyCommandEvent);
        
        // Alert user...
        wxMessageDialog Message(this, 
                                wxT("Sorry, but that is not implemented yet."), 
                                wxT("Analysis Type"), wxICON_INFORMATION);
        Message.ShowModal();
        
        // Abort...
        return;
    }

    // Clear analysis grid...
        
        // Remove all rows, if any
        if(AnalysisGrid->GetNumberRows() > 0)
            AnalysisGrid->DeleteRows(0, AnalysisGrid->GetNumberRows());

        // Remove all columns, if any...
        if(AnalysisGrid->GetNumberCols() > 0)
            AnalysisGrid->DeleteCols(0, AnalysisGrid->GetNumberCols());

    // Set row sizes...
    AnalysisGrid->SetRowMinimalAcceptableHeight(25);

    // Initialize grid for body size analysis
    if(Event.GetSelection() == ANALYSIS_BODY_SIZE)
    {
        // Create columns with thirty stimulus and  three recovery taps...
        for(int nColumn = 0; nColumn < 3; nColumn++)
        {
            // Append column and check for error...
            if(!AnalysisGrid->AppendCols(1))
                return;

            // Generate column label...
            switch(nColumn)
            {
                // Label depends on the column...
                case 0: sTemp = wxT("Length"); break;
                case 1: sTemp = wxT("Width"); break;
                case 2: sTemp = wxT("Area"); break;
            }
            
            // Set column label
            AnalysisGrid->SetColLabelValue(nColumn, sTemp);

            // Set column attributes...
            pColumnAttributes = AnalysisGrid->GetOrCreateCellAttr(0, nColumn);
            pColumnAttributes->SetReadOnly();
            AnalysisGrid->SetColAttr(nColumn, pColumnAttributes);
        }
    }

    // Initialize grid for long term habituation...
    else if(Event.GetSelection() == ANALYSIS_LONG_TERM_HABITUATION)
    {
        // Create columns with thirty stimulus and  three recovery taps...
        for(int nColumn = 0; nColumn < 5 + 3; nColumn++)
        {
            // Append column and check for error...
            if(!AnalysisGrid->AppendCols(1))
                return;

            // Set column label...
            
                // First thirty are stimulus taps...
                if(nColumn < 5)
                    AnalysisGrid->SetColLabelValue(nColumn, 
                        wxString::Format(wxT("Stimulus %d"), nColumn + 1));
                
                // Last three are recovery taps...
                else
                    AnalysisGrid->SetColLabelValue(nColumn, 
                        wxString::Format(wxT("Recovery %d"), nColumn + 1 - 30));                

            // Set column attributes...
            pColumnAttributes = AnalysisGrid->GetOrCreateCellAttr(0, nColumn);
            pColumnAttributes->SetReadOnly();
            AnalysisGrid->SetColAttr(nColumn, pColumnAttributes);
        }
    }

    // Initialize grid for short term habituation...
    else if(Event.GetSelection() == ANALYSIS_SHORT_TERM_HABITUATION)
    {
        // Create columns with thirty stimulus and  three recovery taps...
        for(int nColumn = 0; nColumn < 30 + 3; nColumn++)
        {
            // Append column and check for error...
            if(!AnalysisGrid->AppendCols(1))
                return;

            // Set column label...
            
                // First thirty are stimulus taps...
                if(nColumn < 30)
                    AnalysisGrid->SetColLabelValue(nColumn, 
                        wxString::Format(wxT("Stimulus %d"), nColumn + 1));
                
                // Last three are recovery taps...
                else
                    AnalysisGrid->SetColLabelValue(nColumn, 
                        wxString::Format(wxT("Recovery %d - 30"), nColumn + 1));                

            // Set column attributes...
            pColumnAttributes = AnalysisGrid->GetOrCreateCellAttr(0, nColumn);
            pColumnAttributes->SetReadOnly();
            AnalysisGrid->SetColAttr(nColumn, pColumnAttributes);
        }
    }
    
    // This should not happen... (unknown analysis type)
    else
    {
        // Create...
        wxMessageDialog Notice(this, wxT("The selected analysis type is not"
                                         " implemented yet, sorry."), 
                               wxT("Analysis"), wxICON_INFORMATION);

        // Show and abort...
        Notice.ShowModal();
        return;
    }
    
    
    // Add blank rows to be filled in later by individual worms...
    for(unsigned int unRow = 0; unRow < 15; unRow++)
    {
        // Append new row...
        AnalysisGrid->AppendRows(100);
        
        /* Set empty row label...
        AnalysisGrid->SetRowLabelValue(unRow, wxT(""));*/
    }
    
    // Automatically resize all columns and rows to fit contents...
    AnalysisGrid->AutoSize();
    
    // Trigger analysis results sizer to recalculate layout...
    AnalysisGrid->GetContainingSizer()->Layout();
}

// Capture was toggled...
void MainFrame::OnCapture(wxCommandEvent &Event)
{
    // Capture is being disabled, capture thread will notice this by itself...
    if(!GetToolBar()->GetToolState(ID_CAPTURE))
        return;

    // Disable the button until capture thread is ready...
    GetToolBar()->EnableTool(ID_CAPTURE, false);

    // Switch to the capture notebook pane...
    MainNotebook->ChangeSelection(CAPTURE_PANE);

    // Create the capture thread and check for error...
    CaptureThread *pCaptureThread = new CaptureThread(this);
    if(pCaptureThread->Create() != wxTHREAD_NO_ERROR)
    {
        // Alert...
        wxLogError(wxT("Unable to create CaptureThread..."));

        // Cleanup and abort...
        GetToolBar()->EnableTool(ID_CAPTURE, true);
        delete pCaptureThread;
        return;
    }

    // Initiate the capture timer...
    CaptureTimer.Start(34, wxTIMER_CONTINUOUS);

    // Run the thread and check for error...
    pCaptureThread->SetPriority(WXTHREAD_MIN_PRIORITY);
    if(pCaptureThread->Run() != wxTHREAD_NO_ERROR)
    {
        // Alert...
        wxLogError(wxT("Unable to start the CaptureThread..."));

        // Cleanup and abort...
        GetToolBar()->EnableTool(ID_CAPTURE, true);
        delete pCaptureThread;
        return;    
    }
}

// A frame has just been captured and is ready to be displayed and stored...
void MainFrame::OnCaptureFrameReadyTimer(wxTimerEvent &Event)
{
    // Variables...
    IplImage               *pCapturedIntelImage = NULL;
    unsigned char          *pRawImageData       = NULL;
    int                     nStep               = 0;
    CvSize                  Size;
    int                     x                   = 0;
    int                     y                   = 0;
    int                     nWidth              = 0;
    int                     nHeight             = 0;

    // No frame to grab...
    if(CaptureFrameBuffer.empty())
        return;

    // Get the most recent frame...
    pCapturedIntelImage = (IplImage *) CaptureFrameBuffer.back();

    // Display frame on capture panel, but only if it is visible...
    if(MainNotebook->GetSelection() == CAPTURE_PANE)
    {
        // Get the frame's raw data...
        cvGetRawData(pCapturedIntelImage, &pRawImageData, &nStep, &Size);

        // Convert the raw data into something wxWidgets understands...
        wxImage WxImage = wxImage(pCapturedIntelImage->width, 
                                  pCapturedIntelImage->height, 
                                  pRawImageData, true);
        
        // Get the device context for the video panel...
        wxBufferedPaintDC  DeviceContext(CaptureImagePanel);

        // Get the rectangle surrounding the current clipping region...
        DeviceContext.GetClippingBox(&x, &y, &nWidth, &nHeight);

        // Get the width and height of the video preview panel...
        CaptureImagePanel->GetSize(&nWidth, &nHeight); 

        // Turn the image into a bitmap, scaled to the panel's dimensions...
        wxBitmap Bitmap = wxBitmap(WxImage.Scale(nWidth, nHeight));

        // Paint the bitmap onto the panel's surface...
        DeviceContext.DrawBitmap(Bitmap, x, y);
    }

    // Flush buffer to disk from oldest to newest...
    while(!CaptureFrameBuffer.empty())
    {
        // Retrieve oldest... (to preserve order)
        pCapturedIntelImage = (IplImage *) CaptureFrameBuffer.front();
        
        /* TODO: Write frame to disk here */
        
            /* increasing buffer -> warning for slow codec */
        
        // Deallocate the image and remove its dead pointer from buffer...
        CaptureFrameBuffer.pop_front();
        cvReleaseImage(&pCapturedIntelImage);
    }
}

// Field of view has been set either by user or progmatically...
void MainFrame::OnChooseFieldOfViewDiameter(wxCommandEvent &Event)
{
    // Variables...
    double dFieldOfViewDiameter = 1.0f;

    // Retrieve current diameter...
    FieldOfViewDiameter->GetValue().ToDouble(&dFieldOfViewDiameter);

    // Bounds check...
    if(dFieldOfViewDiameter <= 0.0f)
        dFieldOfViewDiameter = 1.0f;

    // Set the field of view diameter string with millimeter suffix...
    FieldOfViewDiameter->ChangeValue(wxString::Format(wxT("%.3f"), 
                                                      dFieldOfViewDiameter));
    
    // Set tracker's field of view diameter...
    Tracker.SetFieldOfViewDiameter(dFieldOfViewDiameter);

    // Store the custom diameter...
  ::wxGetApp().pConfiguration->Write(
        wxT("/Analysis/CustomFieldOfViewDiameter"), 
        wxString::Format(wxT("%.3f"), dFieldOfViewDiameter));
}

// A microscope name has been chosen...
void MainFrame::OnChooseMicroscopeName(wxCommandEvent &Event)
{
    // Get the chosen microscope...
    Microscope &ChosenMicroscope = MicroscopeTable.at(Event.GetInt());

    // Clear the choosable zooms...
    ChosenMicroscopeTotalZoom->Clear();
    
    // This is the custom microscope...
    if(ChosenMicroscope.GetName() == wxT("Custom"))
    {
        // Make sure field of view diameter is editable...
        FieldOfViewDiameter->SetEditable(true);
        
        // Fetch previously used custom diameter, if any...
        wxString sDiameter = ::wxGetApp().pConfiguration->Read(
                                wxT("/Analysis/CustomFieldOfViewDiameter"), 
                                wxT("5"));
        double dDiameter = 0.0f;
        sDiameter.ToDouble(&dDiameter);
        
        // Restore it
        FieldOfViewDiameter->ChangeValue(
            wxString::Format(wxT("%.3f"), dDiameter));
        
        // Hide zoom...
        ChosenMicroscopeTotalZoom->Hide();
        
        // Notify tracker...
        Tracker.SetFieldOfViewDiameter(dDiameter);
        
        // Done...
        return;
    }
    
    // This is not the custom microscope...
    else
    {
        // Show zoom...
        ChosenMicroscopeTotalZoom->Show();

        // Make sure field of view diameter is read-only...
        FieldOfViewDiameter->SetEditable(false);
    }

    // Populate the choosable zooms with the new microscope's capabilities...
    for(unsigned int unCurrentCapability = 0;
        unCurrentCapability < ChosenMicroscope.GetNumberOfZooms();
        unCurrentCapability++)
    {
        // Make zoom look fancy...
        wxString sZoom;
        sZoom.Printf(wxT("%dX"), 
                      ChosenMicroscope.GetZoom(unCurrentCapability));

        // Add the zoom...
        ChosenMicroscopeTotalZoom->Append(sZoom);
    }
    
    // Trigger default microscope zoom choice...

        // Create dummy command event...
        wxCommandEvent  DummyCommandEvent;
        DummyCommandEvent.SetInt(0);
    
        // Fire off event...
        ChosenMicroscopeTotalZoom->SetSelection(0);
        OnChooseMicroscopeTotalZoom(DummyCommandEvent);
}

// A total zoom has been chosen for currently chosen camera...
void MainFrame::OnChooseMicroscopeTotalZoom(wxCommandEvent &Event)
{
    // Variables...
    long    lTotalZoom      = 0L;
    float   fFieldOfView    = 0.0f;

    // Get the total zoom and parse to integer...
    wxString sTotalZoom = ChosenMicroscopeTotalZoom->GetString(Event.GetInt());
    sTotalZoom.BeforeFirst('X').ToLong(&lTotalZoom);

    // Get the chosen microscope...
    Microscope &ChosenMicroscope = 
        MicroscopeTable.at(ChosenMicroscopeName->GetSelection());

    // Lookup the field of view diameter for this microscope...
    fFieldOfView = ChosenMicroscope.GetDiameter(lTotalZoom);
    
    // Format diameter in a string so we can display SI units with it...
    wxString sDiameter;
    sDiameter.Printf(wxT("%.3f"), fFieldOfView);
    
    // Update diameter text box...
    FieldOfViewDiameter->ChangeValue(sDiameter);

    // Notify tracker...
    Tracker.SetFieldOfViewDiameter(fFieldOfView);
}

// Analysis thread is informing us that it has terminated...
void MainFrame::OnEndAnalysis(wxCommandEvent &Event)
{
    // Get the thinking image...
    IplImage *pThinkingImage = Tracker.GetThinkingImage();
    
    // Show it, if any...
    if(pThinkingImage)
    {
        // Display the image...
        pImageAnalysisWindow->SetImage(*pThinkingImage);
    
        // Cleanup...
        cvReleaseImage(&pThinkingImage);
    }

    // Unlock the UI...

        // Begin analysis button...
        BeginAnalysisButton->Enable();

        // Cancel analysis button...
        CancelAnalysisButton->Disable();

        // Microscope set...
        ChosenMicroscopeName->Enable();
        ChosenMicroscopeTotalZoom->Enable();
        FieldOfViewDiameter->Enable();

        // Analysis type...
        ChosenAnalysisType->Enable();

        // Analysis status...
        AnalysisCurrentFrameStatus->ChangeValue(wxT(""));
        AnalysisRateStatus->ChangeValue(wxT(""));
        AnalysisWormsTrackingStatus->ChangeValue(wxT(""));

        // Analysis buttons...
        BeginAnalysisButton->Enable();
        CancelAnalysisButton->Disable();

        // Analysis gauge...
        AnalysisGauge->SetRange(100);
        AnalysisGauge->SetValue(0);

        // Analysis grid...
        AnalysisGrid->Enable();

        // Artificial intelligence settings...
        AISettingsScrolledWindow->Enable();

        // Alert user...
        AnalysisStatusList->Append(wxT("Analysis ended..."));

        // Refresh the main frame...
        Refresh();
                    
    // Remove all rows, if any and if the user doesn't want to accumulate
    //  results...
    if(AnalysisGrid->GetNumberRows() > 0 && !AccumulateCheckBox->IsChecked())
        AnalysisGrid->DeleteRows(0, AnalysisGrid->GetNumberRows());

    // Body size analysis...
    if(ChosenAnalysisType->GetCurrentSelection() == ANALYSIS_BODY_SIZE)
    {
        // Output analysis results for each worm...
        for(unsigned int unWormIndex = 0; unWormIndex < Tracker.Tracking();
            unWormIndex++)
        {
            // Append a new row for this worm and check if ok...
            if(!AnalysisGrid->AppendRows())
            {
                // Alert user...
                wxLogError(wxT("Out of memory! Check your field of view"
                               " diameter."));
                
                // Abort...
                break;
            }
            
            // Get the index of the new row...
            int const nNewRow = AnalysisGrid->GetNumberRows() - 1;

            // Get the worm at this index...
            Worm const &CurrentWorm = Tracker.GetWorm(unWormIndex);

            // Set row label...
            AnalysisGrid->SetRowLabelValue(nNewRow, 
                wxString::Format(wxT("Worm %d"), nNewRow + 1));
                
            // Length...
            AnalysisGrid->SetCellValue(nNewRow, ANALYSIS_BODY_SIZE_COLUMN_LENGTH,
                wxString::Format(wxT("%.3f mm"), 
                    Tracker.ConvertPixelsToMillimeters(CurrentWorm.Length())));

            // Width...
            AnalysisGrid->SetCellValue(nNewRow, ANALYSIS_BODY_SIZE_COLUMN_WIDTH,
                wxString::Format(wxT("%.3f mm"), 
                    Tracker.ConvertPixelsToMillimeters(CurrentWorm.Width())));
                    
            // Area...
            AnalysisGrid->SetCellValue(nNewRow, ANALYSIS_BODY_SIZE_COLUMN_AREA,
                wxString::Format(wxT("%.3f mm²"), 
                    Tracker.ConvertSquarePixelsToSquareMillimeters(
                        CurrentWorm.Area())));
        }
    }
    
    // Long term habituation analysis...
    else if(ChosenAnalysisType->GetCurrentSelection() == 
            ANALYSIS_LONG_TERM_HABITUATION)
    {
    
    }
    
    // Short term habituation analysis...
    else
    {
    
    }
    
    // Automatically resize all columns and rows to fit contents...
    AnalysisGrid->AutoSize();
    
    // Trigger analysis results sizer to recalculate layout...
    AnalysisGrid->GetContainingSizer()->Layout();
}

// Extract frame button...
void MainFrame::OnExtractFrame(wxCommandEvent &Event)
{
    // Variables...
    CvCapture  *pCapture    = NULL;

    // Pause the video...
    pMediaPlayer->Pause();

    // Initialize capture from AVI...
    pCapture = cvCreateFileCapture(sCurrentMediaPath.fn_str());

        // Failed...
        if(!pCapture)
            return;
    
    // Seek...
    cvSetCaptureProperty(pCapture, CV_CAP_PROP_POS_MSEC, pMediaPlayer->Tell());
    
    // Retrieve the captured image...
    IplImage const *pOriginalImage = cvQueryFrame(pCapture);
    
        // Can't read it...
        if(!pOriginalImage)
        {
            // Cleanup and abort...
            cvReleaseCapture(&pCapture);
            return;
        }

    // Prompt user to add it now...

        // Remove old, if present...
        if(::wxFileExists(wxT("Frame.png")))
            ::wxRemoveFile(wxT("Frame.png"));
	
	// 2020/06/10 - convert to C++ Mat
	//so we can save it (cvSaveImage is deprecated)
        // Save to temporary file name..
        //cvSaveImage("Frame.png", pOriginalImage);
	cv::Mat pOriginalMatImage = cv::cvarrToMat( pOriginalImage );
	cv::imwrite("Frame.png", pOriginalMatImage);


        // Get the media grid drop target...
        MediaGridDropTarget *pDropTarget = 
            (MediaGridDropTarget *) GetDropTarget();
        
        // Prompt to add...
        wxArrayString sFileNameArray;
        sFileNameArray.Add(wxT("Frame.png"));
        pDropTarget->OnDropFiles(0, 0, sFileNameArray);

        // Cleanup...
        if(::wxFileExists(wxT("Frame.png")))
            ::wxRemoveFile(wxT("Frame.png"));

    // Release the capture...
    cvReleaseCapture(&pCapture);
}

// User has selected to go fullscreen...
void MainFrame::OnFullScreen(wxCommandEvent &Event)
{
    // Toggle full screen with newly selected state...
    ShowFullScreen(Event.IsChecked(), wxFULLSCREEN_NOCAPTION | 
                                      wxFULLSCREEN_NOBORDER);
                                      
    // Resize the media player because Quicktime is shit...
    if(pMediaPlayer)
        pMediaPlayer->SetSize(VideoPreviewPanel->GetSize());
}

// User selected to import media from menu...
void MainFrame::OnImportMedia(wxCommandEvent &Event)
{
    // Variables...
    wxArrayString   sFileNameArray;
    
    // Prepare import dialog...
    //  2020/06/10 - Updating for wxGTK 3 
    wxFileDialog FileDialog(this, wxT("Please select media to import..."), 
      wxStandardPaths::Get().GetDocumentsDir(), wxEmptyString, 
        wxT("Images (*.jpg;*.jpeg;*.png;*.bmp)|*.jpg;*.jpeg;*.png;*.bmp|"
            "Videos (*.mov;*.avi;*.mpg;*.mpeg)|*.mov;*.avi;*.mpg;*.mpeg"),
        wxFD_OPEN | wxFD_PREVIEW | wxFD_MULTIPLE | wxFD_FILE_MUST_EXIST);

    // Show open dialog and check if user hit cancel...
    if(wxID_CANCEL == FileDialog.ShowModal())
        return;
                
    // Store the file names...
    FileDialog.GetPaths(sFileNameArray);
            
    // Get the media grid drop target...
    MediaGridDropTarget *pDropTarget = (MediaGridDropTarget *) GetDropTarget();
        
    // Prompt to add...
    pDropTarget->OnDropFiles(0, 0, sFileNameArray);
}

// Get the analysis results formatted into a string...
wxString const MainFrame::GetAnalysisResults()
{
    // Variables...
    wxString sContents;
    
    // Add worm number column label...
    sContents += wxT("Worm #\t");

    // Add column names...
    for(int nColumn = 0; nColumn < AnalysisGrid->GetNumberCols(); ++nColumn)
    {
        // Append column label...
        sContents += AnalysisGrid->GetColLabelValue(nColumn);
        
        // Seperate with a tab if not last column...
        if(nColumn + 1 < AnalysisGrid->GetNumberCols())
            sContents += wxT("\t");
    }
    
    // Seperate column names from rest of data...
    sContents += wxT("\n\n");
    
    // Add each row...
    for(int nRow = 0; nRow < AnalysisGrid->GetNumberRows(); ++nRow)
    {
        // Append row label...
        sContents += AnalysisGrid->GetRowLabelValue(nRow) + wxT("\t");
        
        // Add each cell's value...
        for(int nColumn = 0; nColumn < AnalysisGrid->GetNumberCols(); ++nColumn)
        {
            // Append column label...
            sContents += AnalysisGrid->GetCellValue(nRow, nColumn);
            
            // Seperate with a tab if not last column...
            if(nColumn + 1 < AnalysisGrid->GetNumberCols())
                sContents += wxT("\t");
        }
        
        // Seperate each row by new line...
        sContents += wxT("\n");
    }
    
    // Done...
    return sContents;
}

// Get the total size of all media in the media grid...
wxULongLong MainFrame::GetTotalMediaSize()
{
    // Variables...
    wxULongLong ulTotalSize = 0;

    // Calculate total size...
    for(unsigned int unRow = 0; 
        unRow < (unsigned) MediaGrid->GetNumberRows(); 
        unRow++)
    {
        // Find the media...
        wxString sPath = pExperiment->GetCachePath() + wxT("/media/") + 
                         MediaGrid->GetCellValue(unRow, TITLE);

        // Load it...
        wxFileName MediaFile(sPath);

            // Failed...
            if(!MediaFile.IsOk())
                continue;

        // Size...
        ulTotalSize += MediaFile.GetSize();
    }
    
    // Done...
    return ulTotalSize;
}

// Is experiment contain a media by a specific name?
bool MainFrame::IsExperimentContainMedia(wxString sName)
{
    // Search the media list...
    for(int nRow = 0; nRow < MediaGrid->GetNumberRows(); nRow++)
    {
        // Found match...
        if(MediaGrid->GetCellValue(nRow, MainFrame::TITLE).Lower() == 
           sName.Lower())
            return true;
    }
    
    // Not found...
    return false;
}

// Is experiment contain a media by a specific name, except a row?
bool MainFrame::IsExperimentContainMediaExceptRow(wxString sName, int nSkipRow)
{
    // Search the media list...
    for(int nRow = 0; nRow < MediaGrid->GetNumberRows(); nRow++)
    {
        // Not checking this row, skip it...
        if(nSkipRow == nRow)
            continue;

        // Found match...
        if(MediaGrid->GetCellValue(nRow, MainFrame::TITLE).Lower() == 
           sName.Lower())
            return true;
    }
    
    // Not found...
    return false;
}

// Is there an experiment loaded?
bool MainFrame::IsExperimentLoaded() const
{
    // Check...
    return (pExperiment && pExperiment->IsLoadOk());
}

// Media has just been loaded for play back...
void MainFrame::OnMediaLoaded(wxMediaEvent& Event)
{
    // Try to play movie and check for error...
    if(!pMediaPlayer->Play())
        wxMessageBox(wxT("Unable to play movie."));

    // Show media controls...
    if(pMediaPlayer)
        pMediaPlayer->ShowPlayerControls();
    
    // Enable the extract frame button... (only works on non-gtk backend)
    #ifndef __LINUX__
    ExtractFrameButton->Enable();
    #endif
}

// Media has stopped...
void MainFrame::OnMediaStop(wxMediaEvent& Event)
{
    /*if(bUserWantsToSeek)
    {
        m_mediactrl->SetPosition(
            m_mediactrl->GetDuration() << 1
                                );
        evt.Veto();
    }*/
}

// Media is now playing...
void MainFrame::OnMediaPlay(wxMediaEvent& Event)
{

}

// Frame moving...
void MainFrame::OnMove(wxMoveEvent &Event)
{
    // Resize the media player because Quicktime is shit...
    if(pMediaPlayer)
        pMediaPlayer->SetSize(VideoPreviewPanel->GetSize());

    // Refresh the frame...
//    Refresh();

    // Allow child controls to resize appropriately...
    Event.Skip();
}


// New command event handler...
void MainFrame::OnNew(wxCommandEvent &Event)
{
    // An experiment is already loaded and needs a save...
    if(IsExperimentLoaded() && pExperiment->IsNeedSave())
    {
        // Prepare warning message box...
        wxMessageDialog 
            Message(this, wxT("You have made changes to the experiment that"
                              " have not been saved. Continue anyways?"), 
                    wxT("Unsaved changes"), 
                    wxYES_NO | wxNO_DEFAULT | wxICON_EXCLAMATION);
                        
        // Now show it and cancel quit if they request it...
        if(Message.ShowModal() == wxID_NO)
            return;
    }
    
    // Cleanup current experiment...
    delete pExperiment;
    pExperiment = NULL;
    
    // Create a new experiment...
    pExperiment = new Experiment(this);
}

// Notebook page is in the process of changing...
void MainFrame::OnPageChanging(wxNotebookEvent &Event)
{
    /* Trying to switch to analysis page...
    if(Event.GetSelection() == ANALYSIS_PANE)
    {
        // They don't have a single media selected...
        if(MediaGrid->GetSelectedRows().GetCount() != 1)
        {
            // Veto the page change and go back to the data pane...
            Event.Veto();
            MainNotebook->ChangeSelection(ANALYSIS_PANE);

            // Alert user...
            wxMessageBox(wxT("Please select a single media to analyze first."));

            // Done...
            return;
        }
    }*/
}

// Media grid popup menu event for play...
void MainFrame::OnPlay(wxCommandEvent &Event)
{
    // Get the complete path to the media to play...
    
        // Find the row selected...
        wxArrayInt SelectedRows = MediaGrid->GetSelectedRows();
        int nRow = SelectedRows[0];
        
        // Generate complete path...
        wxString sPath = pExperiment->GetCachePath() + wxT("/media/") +
                         MediaGrid->GetCellValue(nRow, TITLE);

    // Load media and check for error...
    if(!pMediaPlayer->Load(sPath))
    {
        // Alert and abort...
        wxMessageBox(wxT("Unable to load media. Perhaps it uses an unknown "
                         "codec or is corrupt?"));
        return;
    }
    
    // Store the file name for extract frame button...
    sCurrentMediaPath = sPath;

    // Update media length in the media grid...

        // Calculate minutes and seconds of media...
        wxLongLong llMediaLength = pMediaPlayer->Length();
        int nMinutes = (int) (llMediaLength / 60000).GetValue();
        int nSeconds = (int) ((llMediaLength % 60000) / 1000).GetValue();

        // Format string and update field...
        wxString sDuration;
        sDuration.Printf(wxT("%2i:%02i"), nMinutes, nSeconds);
        MediaGrid->SetCellValue(nRow, LENGTH, sDuration);
}

// Experiment has changed handler...
void MainFrame::OnExperimentChange(wxCommandEvent &Event)
{
    // No experiment is loaded...
    if(!IsExperimentLoaded())
        return;

    // Trigger save necessary...
    pExperiment->TriggerNeedSave();
}

// Media grid has had a cell change...
void MainFrame::OnExperimentChangeCell(wxGridEvent &Event)
{
    // No experiment is loaded...
    if(!IsExperimentLoaded())
        return;

    /* Is this somewhere within the title column?
    if(Event.GetCol() == TITLE)
    {
        // What did the user just try and rename the media's title to?
        wxString sNewTitle = MediaGrid->GetCellValue(Event.GetRow(), TITLE)
        
        IsExperimentContainMedia(
        
    }*/

    // Trigger save necessary...
    pExperiment->TriggerNeedSave();
}

// Open command event handler...
void MainFrame::OnOpen(wxCommandEvent &Event)
{
    // Variables...
    wxString    sFileName;

    // An experiment is already loaded and needs a save...
    if(IsExperimentLoaded() && pExperiment->IsNeedSave())
    {
        // Prepare warning message box...
        wxMessageDialog 
            Message(this, wxT("You have made changes to the experiment that"
                              " have not been saved. Continue anyways?"), 
                    wxT("Unsaved changes"), 
                    wxYES_NO | wxNO_DEFAULT | wxICON_EXCLAMATION);
                        
        // Now show it and cancel quit if they request it...
        if(Message.ShowModal() == wxID_NO)
            return;
    }

    // Open experiment now...
    
        // Shell has provided us with experiment file name...
        if(!::wxGetApp().sExperimentRequestedFromShell.IsEmpty())
        {
            // Store the file name...
            sFileName = ::wxGetApp().sExperimentRequestedFromShell;

            // Since the string doubles as a flag, clear for next time...
            ::wxGetApp().sExperimentRequestedFromShell.Empty();
        }

        // User will manually provide us with experiment file name...
        else
        {
            // Prepare open dialog...
	    //  2020/06/10 - updated for wxGTK 3
            wxFileDialog 
                FileDialog(this, wxT("Please select a Slither experiment..."), 
                         wxStandardPaths::Get().GetDocumentsDir(), 
                           ExperimentTitle->GetValue() + wxT(".sex"), 
                           wxT("Slither experiment (*.sex)|*.sex"),
                           wxFD_OPEN | wxFD_PREVIEW | wxFD_FILE_MUST_EXIST);

            // Show open dialog and check if user hit cancel...
            if(wxID_CANCEL == FileDialog.ShowModal())
                return;
                
            // Store the file name...
            sFileName = FileDialog.GetPath();
        }

    // Cleanup current experiment... (an object is always allocated)
    delete pExperiment;

    // Allocate space for experiment to load...
    pExperiment = new Experiment(this);

    // It doesn't need to be saved since it is already saved on disk...
    pExperiment->ClearNeedSave();

    // Load experiment and check for error...
    if(!pExperiment->Load(sFileName))
    {
        // Alert user...
        SetStatusText(wxString(wxT("Error reading from ")) + 
                      pExperiment->GetPath() + wxT("..."));
                      
        // Unload what was loaded, if anything...
        delete pExperiment;
        pExperiment = NULL;
    }

    // Loaded ok...
    else
        SetStatusText(wxString::Format(wxT("Loaded %s successfully..."),
                      pExperiment->GetPath().c_str()));
}

// Media grid popup menu event for remove...
void MainFrame::OnRemove(wxCommandEvent &Event)
{
    // Remove each row backwards to preserve row ordinals...
    wxArrayInt SelectedRows = MediaGrid->GetSelectedRows();
    SelectedRows.Sort(CompareIntegers);
    for(int nIndex = SelectedRows.GetCount() - 1; nIndex >= 0; --nIndex)
    {
        // Double check with the user first...
        wxMessageDialog 
        Message(this, wxT("Are you sure you want to remove the following media"
                          " from your experiment?\n\n") + 
                      MediaGrid->GetCellValue(SelectedRows[nIndex], TITLE), 
                wxT("Remove Media"), 
                wxOK | wxCANCEL | wxICON_EXCLAMATION);
                        
            // They requested to not remove the media from the experiment...
            if(Message.ShowModal() == wxID_CANCEL)
                continue;

        // Generate a complete path to the media selected...
        wxString sPath = pExperiment->GetCachePath() + wxT("/media/") + 
                         MediaGrid->GetCellValue(SelectedRows[nIndex], TITLE);

        // Remove it and check for error...
        if(!::wxRemoveFile(sPath))
        {
            // Log it and skip to next...
            wxLogError(wxString::Format(wxT("Unable to delete:\n\n%s\n(Row: %d)"),
                             sPath.c_str(), SelectedRows[nIndex]));
            continue;
        }

        // Remove the row...
        MediaGrid->DeleteRows(SelectedRows[nIndex]);
        
        // Trigger need save...
        pExperiment->TriggerNeedSave();

        // Update embedded media count...
        wxString sEmbeddedMedia; 
        sEmbeddedMedia << MediaGrid->GetNumberRows();
        EmbeddedMedia->ChangeValue(sEmbeddedMedia);
        
        // Update total embedded media size...
        wxULongLong ulTotalSize = GetTotalMediaSize();
        ulTotalSize /= 1024;
        TotalSize->ChangeValue(ulTotalSize.ToString() + wxT(" KB"));
    }
}

// Media grid popup menu event for rename...
void MainFrame::OnRename(wxCommandEvent &Event)
{
    // Find the selected row...
    wxArrayInt SelectedRow = MediaGrid->GetSelectedRows();
    
        // Verify that there is but one...
        if(SelectedRow.GetCount() != 1)
            return;

    // Get the original name...
    wxString sOriginalName = MediaGrid->GetCellValue(SelectedRow[0], TITLE);

    // Prepare the text entry dialog...
    wxTextEntryDialog Dialog(this, wxT("Please enter the new unique name:"),
                             wxT("Rename Media"),
                             sOriginalName, wxOK | wxCANCEL);

        // Display and check for cancel...
        if(Dialog.ShowModal() == wxID_CANCEL)
            return;

    // Value unchanged...
    if(Dialog.GetValue().Lower() == sOriginalName.Lower())
        return;

    // Contains invalid characters...
    if((Dialog.GetValue().Find(wxT("..")) != wxNOT_FOUND) || 
       (Dialog.GetValue().Find(wxT("/")) != wxNOT_FOUND) || 
       (Dialog.GetValue().Find(wxT("\\")) != wxNOT_FOUND) || 
       (Dialog.GetValue().Find(wxT(" ")) != wxNOT_FOUND))
    {
        // Log error and then abort...
        wxLogError(wxT("You cannot rename the media to that. Make sure you do"
                       " not have any spaces."));
        return;
    }

    // Verify that this name isn't already taken...
    if(IsExperimentContainMediaExceptRow(Dialog.GetValue(), SelectedRow[0]))
    {
        // Log error and then abort...
        wxLogError(wxT("Media already exists in your experiment by that name."
                       " Try something else."));
        return;
    }

    // Rename the file...
    if(!::wxRenameFile(pExperiment->GetCachePath() + wxT("/media/") + 
                         sOriginalName, 
                       pExperiment->GetCachePath() + wxT("/media/") + 
                         Dialog.GetValue(), false))
    {
        // Log error and then abort...
        wxLogError(wxT("Unable to rename media..."));
        return;    
    }

    // Rename the title in the media grid...
    MediaGrid->SetCellValue(SelectedRow[0], TITLE, Dialog.GetValue());

    // Trigger need save...
    pExperiment->TriggerNeedSave();
}

// Revert command event handler...
void MainFrame::OnRevert(wxCommandEvent &Event)
{
    // Stubbed...
    wxLogMessage(wxT("MainFrame::OnRevert not implemented yet..."));
}

// Save command event handler...
void MainFrame::OnSave(wxCommandEvent &Event)
{
    // Experiment hasn't been saved yet...
    if(!pExperiment->IsEverBeenSaved())
    {
        // Use save as instead then...
        ProcessCommand(wxID_SAVEAS);    
    }

    // Save experiment and check for error...
    if(!pExperiment->Save())
        SetStatusText(wxString(wxT("Unable to write to ")) + 
                      pExperiment->GetPath());

    // Saved ok...
    else
        SetStatusText(wxString(wxT("Saved...")));
}

// Save as command event handler...
void MainFrame::OnSaveAs(wxCommandEvent &Event)
{
    // Prepare save as dialog...
    //  2020/06/10 - updating for wxGTK 3
    wxFileDialog FileDialog(this, wxT("Save Slither experiment as..."), 
                          wxStandardPaths::Get().GetDocumentsDir(), 
                            ExperimentTitle->GetValue() + wxT(".sex"), 
                            wxT("Slither experiment (*.sex)|*.sex"),
                            wxFD_SAVE | wxFD_OVERWRITE_PROMPT | wxFD_PREVIEW);

    // Show save as dialog and check if user hit cancel...
    if(wxID_CANCEL == FileDialog.ShowModal())
        return;

    // Save experiment and check for error...
    if(!pExperiment->SaveAs(FileDialog.GetPath()))
        SetStatusText(wxString(wxT("Unable to write to ")) + 
                      pExperiment->GetPath());

    // Saved ok...
    else
        SetStatusText(wxString::Format(wxT("Saved %s successfully..."), 
                      pExperiment->GetPath().c_str()));
}

// Frame resized...
void MainFrame::OnSize(wxSizeEvent &Event)
{
    // Resize the media player because Quicktime is shit...
    if(pMediaPlayer)
        pMediaPlayer->SetSize(VideoPreviewPanel->GetSize());

    // Store the window size...
  ::wxGetApp().pConfiguration->Write(wxT("/General/Width"), 
                                     Event.GetSize().GetWidth());
  ::wxGetApp().pConfiguration->Write(wxT("/General/Height"), 
                                     Event.GetSize().GetHeight());

        // Flush configuration to disk...
//      ::wxGetApp().pConfiguration->Flush();

    // Allow child controls to resize appropriately...
    Event.Skip();
}

// Media grid popup menu event for stop...
void MainFrame::OnStop(wxCommandEvent &Event)
{
    // Stop...
    if(pMediaPlayer)
        pMediaPlayer->Stop();
}

// Image analysis window has been toggled...
void MainFrame::OnToggleImageAnalysisWindow(wxCommandEvent &Event)
{
    // Show or hide...
    pImageAnalysisWindow->Show(Event.IsChecked());
}

// Close command event handler...
void MainFrame::OnClose(wxCommandEvent &Event)
{
    // An experiment needs to be saved...
    if(pExperiment->IsNeedSave())
    {
        // Prepare warning message box...
        wxMessageDialog 
            Message(this, wxT("You have made changes to the experiment that"
                              " have not been saved. Continue anyways?"), 
                    wxT("Unsaved changes"), 
                    wxYES_NO | wxNO_DEFAULT | wxICON_EXCLAMATION);
                        
        // Now show it and cancel quit if they request it...
        if(Message.ShowModal() == wxID_NO)
            return;
    }

    // Stop the analysis and capture threads if running...
    wxCommandEvent DummyEvent;
    CancelAnalysisButton->Command(DummyEvent);

    // Remove current experiment...
    delete pExperiment;
    pExperiment = NULL;
}

// About command event handler...
void MainFrame::OnAbout(wxCommandEvent &Event)
{
    // Objects...
    wxAboutDialogInfo   AboutDialogInfo;
    wxPlatformInfo      PlatformInfo;

    // Initialize about box information...

        // Name and version...    
        AboutDialogInfo.SetName(wxT("Slither"));
        AboutDialogInfo.SetVersion(SLITHER_VERSION);

        // Description...
        AboutDialogInfo.SetDescription(wxString::Format(
            wxT("Slither is a free and open source tool available for\n"
                "GNU/Linux, Win32, and Mac OS X. It is designed to help\n"
                "you perform tap experiments on C. elegans within a\n"
                "lab. It manages your experiments by capturing and\n"
                "organizing media directly from your lab's dissection\n"
                "microscope camera. It can also try to analyze what it\n"
                "thinks it saw, generating some useful data\n"
                "automagically for you.\n\n"

                "Your hardware appears to be %s (%s).\n\n"

                "Slither was built on %s at\n"
                "%s. It was developed using %s\n"
                "(provided by the %s port).\n\n"
                
                "Compiled against version " CV_VERSION " of Intel's\n"
                "OpenCV.\n\n"),

                PlatformInfo.GetArchName(PlatformInfo.GetArchitecture()).c_str(),
                PlatformInfo.GetEndiannessName(PlatformInfo.GetEndianness()).
                                                MakeLower().c_str(),
                wxT(__DATE__),
                wxT(__TIME__),
                wxVERSION_STRING,
                PlatformInfo.GetPortIdName().c_str()));

        // Developers... (kind of)
        AboutDialogInfo.AddDeveloper(wxT("Current Developers:"));
        AboutDialogInfo.AddDeveloper(wxT("\tKip Warner <kip@thevertigo.com> (OpenPGP: 0x0244AE0E)"));
        AboutDialogInfo.AddDeveloper(wxT("\tVarun Ramraj <vr@oracology.net> (OpenPGP: 0xD1C3104E)"));
        AboutDialogInfo.AddDeveloper(wxT(""));
        AboutDialogInfo.AddDeveloper(wxT("Beta Testers:"));
        AboutDialogInfo.AddDeveloper(wxT("\tEvan Ardiel <eardiel@yahoo.ca>"));
        AboutDialogInfo.AddDeveloper(wxT("\tMike Butterfield <butter@interchange.ubc.ca>"));
        AboutDialogInfo.AddDeveloper(wxT(""));
        AboutDialogInfo.AddDeveloper(wxT("Thanks:"));
        AboutDialogInfo.AddDeveloper(wxT("\tCatharine Rankin <crankin@psych.ubc.ca> (opportunity)"));
        AboutDialogInfo.AddDeveloper(wxT("\tEd Knorr <knorr@cs.ubc.ca> (criticisms)"));
        AboutDialogInfo.AddDeveloper(wxT("\tThomas L. Adelman <tadelman2@yahoo.com> (advice)"));
        AboutDialogInfo.AddDeveloper(wxT("\tVarun Ramraj <vr@oracology.net> (OpenPGP: 0xD1C3104E)"));

        // Documentation writers...
        AboutDialogInfo.AddDocWriter(wxT("Kip Warner <kip@thevertigo.com> (OpenPGP: B6E28B6D)"));

        // Website...
        AboutDialogInfo.SetWebSite(wxT("https://github.com/kiplingw/slither/"));
        
        // Set license...
        AboutDialogInfo.SetLicense(
            wxT("Slither is free software; you can redistribute it and/or "
                "modify it under the terms of the GNU General Public License "
                "as published by the Free Software Foundation; either version "
                "3 of the License, or (at your option) any later version.\n\n"

                "Slither is distributed in the hope that it will be useful, "
                "but WITHOUT ANY WARRANTY; without even the implied warranty "
                "of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See "
                "the GNU General Public License for more details.\n\n"

                "You should have received a copy of the GNU General Public "
                "License along with Slither; if not, write to the Free "
                "Software Foundation, Inc., 51 Franklin Street, Fifth Floor, "
                "Boston, MA 02110-1301, USA"));

    // Display the about box...
    wxAboutBox(AboutDialogInfo);
}

// User double clicked with the left mouse button over a cell...
void MainFrame::OnMediaCellDoubleLeftClick(wxGridEvent &Event)
{
    // Move the grid cursor to the cell under the mouse cursor...
    MediaGrid->SetGridCursor(Event.GetRow(), Event.GetCol());
    
    // Is this cell editable?
    if(MediaGrid->CanEnableCellControl())
    {
        // Enable and show the edit control...
        MediaGrid->EnableCellEditControl();
        MediaGrid->ShowCellEditControl();
    }
    
    // Otherwise jump to the analysis pane...
    else
        ProcessCommand(ID_ANALYZE);
}

// User left clicked a cell...
void MainFrame::OnMediaCellLeftClick(wxGridEvent &Event)
{
    // Select the entire row...
    MediaGrid->SelectRow(Event.GetRow(), Event.ControlDown() ? true : false);
}

// User right clicked a cell...
void MainFrame::OnMediaCellRightClick(wxGridEvent &Event)
{
    // Control down, add to selection...
    if(Event.ControlDown())
        MediaGrid->SelectRow(Event.GetRow(), true);

    // Control not down...
    else
    {
        // Over unselected row, select it only...
        if(!MediaGrid->IsInSelection(Event.GetRow(), Event.GetCol()))
            MediaGrid->SelectRow(Event.GetRow());
    }

    // Create popup menu...
    wxMenu Menu;
        
        // Analyze...
        wxMenuItem *pAnalyzeItem = new wxMenuItem(&Menu, ID_ANALYZE, 
                                                  wxT("&Analyze"));
        pAnalyzeItem->SetBitmap(analyze_32x32_xpm);
        Menu.Append(pAnalyzeItem);
        
        // Play...
        wxMenuItem *pPlayItem = new wxMenuItem(&Menu, ID_PLAY, wxT("&Play"));
        pPlayItem->SetBitmap(play_32x32_xpm);
        Menu.Append(pPlayItem);
        
        // Remove...
        wxMenuItem *pRemoveItem = new wxMenuItem(&Menu, ID_REMOVE, 
                                                 wxT("&Remove"));
        pRemoveItem->SetBitmap(remove_32x32_xpm);
        Menu.Append(pRemoveItem);
        
        // Rename, if only one row is selected...
        if(MediaGrid->GetSelectedRows().GetCount() == 1)
        {
            // Create the menu item...
            wxMenuItem *pRenameItem = 
                new wxMenuItem(&Menu, ID_RENAME, wxT("R&ename"));
            pRenameItem->SetBitmap(rename_32x32_xpm);
            Menu.Append(pRenameItem);
        }

        // Stop...
        wxMenuItem *pStopItem = new wxMenuItem(&Menu, ID_STOP, wxT("&Stop"));
        pStopItem->SetBitmap(stop_32x32_xpm);
        Menu.Append(pStopItem);
        
    // Popup menu...
    MediaGrid->PopupMenu(&Menu, Event.GetPosition());
}

// Quit command event handler...
void MainFrame::OnQuit(wxCommandEvent &Event)
{
    // Make sure capture and analysis threads exit cleanly first...
    /*if(CaptureTimer.IsRunning())
        pCaptureThread->Delete();*/
    if(AnalysisTimer.IsRunning())
        pAnalysisThread->Delete();

    // Close experiment first...
    ProcessCommand(wxID_CLOSE); 

        // Experiment still loaded, abort...
        if(pExperiment)
            return;

    // Close the frame...
    Close();
}

// Reset the artificial intelligence magic to defaults...
void MainFrame::OnResetAIToDefaults(wxCommandEvent &Event)
{
    // Image threshold...
    ThresholdSpinner->SetValue(150);
    MaxThresholdValueSpinner->SetValue(255);
    
    // Candidate size...
    MinimumCandidateSizeSpinner->SetValue(50);
    MaximumCandidateSizeSpinner->SetValue(120);
    
    // Inlet detection...
    InletDetectionCheckBox->SetValue(true);
    InletCorrectionSpinner->SetValue(5);
}

// User clicked the (X) / system menu (Windows) or Close() was invoked...
void MainFrame::OnSystemClose(wxCloseEvent &Event)
{
    // Make sure capture and analysis threads exit cleanly first...
    /*if(CaptureTimer.IsRunning())
        pCaptureThread->Delete();*/
    if(AnalysisTimer.IsRunning())
        pAnalysisThread->Delete();

    // An experiment needs to be saved...
    if(pExperiment && pExperiment->IsNeedSave())
    {
        // Prepare warning message box...
        wxMessageDialog 
            Message(this, wxT("You have made changes to the experiment that"
                              " have not been saved.\n\nSave before exit?"), 
                    wxT("Unsaved changes"), 
                    wxYES_NO | wxYES_DEFAULT | wxICON_EXCLAMATION);
                        
        // Now show it and save if they request it
        if(Message.ShowModal() == wxID_YES)
            ProcessCommand(wxID_SAVE);
    }

    // Remove current experiment...
    delete pExperiment;
    pExperiment = NULL;

    // Destroy the frame...
    Destroy();
}

// Show the tip window...
void MainFrame::ShowTip()
{
    // Variables...
    static size_t           nTipIndex   = (size_t) - 1;
    // 2020/06/10 - wxStandardPaths is protected now
    //in wxWidgets 3
           wxStandardPaths  StandardPaths = wxStandardPaths::Get();
           wxString         sTipsPath;

    // Has the user disabled tips?
    if(!::wxGetApp().pConfiguration->Read(wxT("/General/ShowTips"), true))
        return;

    // Pick a random tip to start displaying at...
    if(nTipIndex == (size_t) -1)
    {
        // Seed the random number generator...
        srand(time(NULL));
        
        // Pick a random tip index...
        nTipIndex = rand() % 5;    
    }

    // Find the tips...

        // Macs store it in the application bundle...
        #if defined(__APPLE__)

            // Check for buggy wxMac standard paths implementation...
            if(StandardPaths.GetResourcesDir().StartsWith(wxT("/usr")))
            {
                // Use path to executable as a reference point...
                sTipsPath = StandardPaths.GetExecutablePath().BeforeLast('/') +
                            wxT("/../Resources/tips.txt");
                
                // Alert user via console...
                wprintf(wxT("Hack: Buggy wxStandardPaths::GetResourceDir()...\n"
                            "\t\"%ls\"\n"), sTipsPath.wc_str());
            }

            // Working standard paths...
            else
                sTipsPath = wxStandardPaths.GetResourcesDir() + wxT("/tips.txt");

        // Linux has it in the primary hierarchy's share...
        #elif defined(__LINUX__)
            sTipsPath = wxT("/usr/share/slither/tips.txt");

        // Windows we'll just use it in the executable directory...
        #elif defined(__WINDOWS__)
            sTipsPath = wxT("tips.txt");

        // Unknown platform...
        #else
            #error "I don't know where to look for tips on your platform..."
        #endif
    
    // Initialize tip provider for this platform...
    wxTipProvider *pTipProvider = 
        wxCreateFileTipProvider(sTipsPath, nTipIndex);

    // Display tip window and store user preference...
  ::wxGetApp().pConfiguration->Write(wxT("/General/ShowTips"), 
                                     wxShowTip(this, pTipProvider, true));

    // Cleanup tip provider...
    delete pTipProvider;
}

