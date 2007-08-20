/*
  Name:         MainFrame.cpp (implementation)
  Author:       Kip Warner (Kip@TheVertigo.com)
  Description:  The main window...
*/

// Includes...
#include "MainFrame.h"
#include "VideosGridDropTarget.h"
#include "Experiment.h"
#include <wx/dcbuffer.h>

// Bitmaps...
#include "resources/analyze_32x32.xpm"
#include "resources/book_64x64.xpm"
#include "resources/camera_64x64.xpm"
//#include "resources/folder_64x64.xpm"
//#include "resources/pause_32x32.xpm"
#include "resources/play_32x32.xpm"
#include "resources/record_60x60.xpm"
#include "resources/remove_32x32.xpm"
#include "resources/rename_32x32.xpm"
#include "resources/robot_64x64.xpm"
//#include "resources/skip_back_32x32.xpm"
//#include "resources/skip_forward_32x32.xpm"
#include "resources/slither.xpm"
#include "resources/save_60x60.xpm"
#include "resources/stop_32x32.xpm"
//#include "resources/stop_60x60.xpm"
#include "resources/usb_32x32.xpm"

// Event table for MainFrame...
BEGIN_EVENT_TABLE(MainFrame, MainFrame_Base)

    // System events...
    EVT_CLOSE               (MainFrame::OnSystemClose)

    // Toolbar and menu events...
    EVT_MENU                (wxID_NEW,                      MainFrame::OnNew)
    EVT_MENU                (wxID_OPEN,                     MainFrame::OnOpen)
    EVT_MENU                (wxID_SAVE,                     MainFrame::OnSave)
    EVT_MENU                (wxID_SAVEAS,                   MainFrame::OnSaveAs)
    EVT_MENU                (wxID_REVERT,                   MainFrame::OnRevert)
    EVT_MENU                (wxID_CLOSE,                    MainFrame::OnClose)
    EVT_MENU                (wxID_EXIT,                     MainFrame::OnQuit)
    EVT_MENU                (ID_FULLSCREEN,                 MainFrame::OnFullScreen)
    EVT_MENU                (wxID_PREFERENCES,              MainFrame::OnPreferences)
    EVT_MENU                (ID_CAPTURE,                    MainFrame::OnCapture)
    EVT_MENU                (wxID_ABOUT,                    MainFrame::OnAbout)
    
    // Videos grid popup menu events...
    EVT_MENU                (ID_ANALYZE,                    MainFrame::OnAnalyze)
    EVT_MENU                (ID_PLAY,                       MainFrame::OnPlay)
    EVT_MENU                (ID_REMOVE,                     MainFrame::OnRemove)
    EVT_MENU                (ID_RENAME,                     MainFrame::OnRename)    
    EVT_MENU                (ID_STOP,                       MainFrame::OnStop)

    // Frame moving or resizing...
    EVT_MOVE                (                               MainFrame::OnMove)
    EVT_SIZE                (                               MainFrame::OnSize)

    // Text edit events...
    EVT_TEXT                (XRCID("ExperimentTitle"),      MainFrame::OnExperimentChange)
    EVT_TEXT                (XRCID("ExperimentNotes"),      MainFrame::OnExperimentChange)

    // VideosGrid events...
    EVT_GRID_CMD_CELL_CHANGE(XRCID("VideosGrid"),           MainFrame::OnExperimentChangeCell)
    EVT_GRID_CELL_LEFT_DCLICK(                              MainFrame::OnVideoCellDoubleLeftClick)
    EVT_GRID_CELL_LEFT_CLICK(                               MainFrame::OnVideoCellLeftClick)
    EVT_GRID_CELL_RIGHT_CLICK(                              MainFrame::OnVideoCellRightClick)

    // Capture...
    EVT_TIMER               (TIMER_CAPTURE,                 MainFrame::OnCaptureFrameReadyTimer)

    // Analysis...
    EVT_CHOICE              (XRCID("ChosenMicroscopeName"), MainFrame::OnChooseMicroscopeName)
    EVT_CHOICE              (XRCID("ChosenMicroscopeTotalZoom"),   
                                                            MainFrame::OnChooseMicroscopeTotalZoom)
    EVT_CHOICE              (XRCID("ChosenAnalysisType"),   MainFrame::OnChooseAnalysisType)
    EVT_BUTTON              (XRCID("BeginAnalysisButton"),  MainFrame::OnBeginAnalysis)
    EVT_TIMER               (TIMER_ANALYSIS,                MainFrame::OnAnalysisFrameReadyTimer)
    EVT_BUTTON              (XRCID("CancelAnalysisButton"), MainFrame::OnCancelAnalysis)

END_EVENT_TABLE()

// MainFrame constructor...
MainFrame::MainFrame(const wxString &sTitle)
    : pExperiment(NULL),
      pMediaPlayer(NULL),
      CaptureTimer(this, TIMER_CAPTURE),
      pAnalysisThread(NULL),
      AnalysisTimer(this, TIMER_ANALYSIS),
      bExiting(false)
{
    // Set the title...
    SetTitle(wxT(PACKAGE_STRING));

    // Set the icon...
    SetIcon(slither_xpm);

    // Create menu...

        // Create file menu...
        wxMenu *pFileMenu = new wxMenu;
        pFileMenu->Append(wxID_NEW, wxT("&New\tCtrl+N"), 
                          wxT("Create a new experiment..."));
        pFileMenu->Append(wxID_OPEN, wxT("&Open\tCtrl+O"), 
                          wxT("Open an existing experiment..."));
        pFileMenu->AppendSeparator();
        pFileMenu->Append(wxID_SAVE, wxT("&Save\tCtrl+S"), 
                          wxT("Save experiment..."));
        pFileMenu->Append(wxID_SAVEAS, wxT("Save &As\tShift+Ctrl+S"), 
                          wxT("Save experiment under a new name..."));
        pFileMenu->Append(wxID_REVERT, wxT("&Revert"), 
                          wxT("Revert to saved version of experiment..."));
        pFileMenu->AppendSeparator();
        pFileMenu->Append(wxID_CLOSE, wxT("&Close\tCtrl+W"), 
                          wxT("Close the open experiment..."));
        pFileMenu->Append(wxID_EXIT, wxT("&Quit\tCtrl+Q"), 
                          wxT("Quit Slither..."));

        // Create edit menu...
        wxMenu *pEditMenu = new wxMenu;
        #ifdef __APPLE__
        pEditMenu->Append(wxID_PREFERENCES, wxT("&Preferences\tCtrl+,"),
        #else
        pEditMenu->Append(wxID_PREFERENCES, wxT("&Preferences\tCtrl+P"),
        #endif
                          wxT("Configure your camera and other things..."));
                          
        // Create view menu...
        wxMenu *pViewMenu = new wxMenu;
        #ifdef __APPLE__
        pViewMenu->AppendCheckItem(ID_FULLSCREEN, wxT("&Full Screen\tCtrl+F"),
        #else
        pViewMenu->AppendCheckItem(ID_FULLSCREEN, wxT("&Full Screen\tF11"),
        #endif
                                   wxT("Toggle full screen mode..."));

        // Create help menu...
        wxMenu *pHelpMenu = new wxMenu;
        pHelpMenu->Append(wxID_ABOUT, wxT("&About\tF1"), wxT("Show about"));

        // Bind menus to menu bar...
        wxMenuBar *pMenuBar = new wxMenuBar();
        pMenuBar->Append(pFileMenu, wxT("&File"));
        pMenuBar->Append(pEditMenu, wxT("&Edit"));
        pMenuBar->Append(pViewMenu, wxT("&View"));
        pMenuBar->Append(pHelpMenu, wxT("&Help"));

        // Bind menu bar to frame...
        SetMenuBar(pMenuBar);

    // Create the toolbar... (wxGlade doesn't appear to support this just yet)
        
        // Allocate...        
        wxToolBar* pMainToolBar = new wxToolBar(this, wxID_ANY, 
                                                wxDefaultPosition, wxDefaultSize, 
                                                wxTB_HORIZONTAL | wxNO_BORDER |
                                                wxTB_TEXT);

        // Hold bitmaps as we load them here...
        wxBitmap Bitmap;

        // Populate toolbar...

            // New...
            Bitmap = wxArtProvider::GetBitmap(wxART_NEW, wxART_TOOLBAR);
            pMainToolBar->AddTool(wxID_NEW, wxT("New"), Bitmap, 
                                  wxT("Create new experiment..."));
            pMainToolBar->SetToolLongHelp(wxID_NEW, 
                wxT("Create a brand new experiment..."));

            // Open...
            Bitmap = wxArtProvider::GetBitmap(wxART_FILE_OPEN, wxART_TOOLBAR);
            pMainToolBar->AddTool(wxID_OPEN, wxT("Open"), Bitmap,
                                  wxT("Open existing experiment..."));
            pMainToolBar->SetToolLongHelp(wxID_OPEN, 
                wxT("Open an experiment you've already saved..."));

            // Save...
            Bitmap = wxArtProvider::GetBitmap(wxART_FILE_SAVE, wxART_TOOLBAR);
            pMainToolBar->AddTool(wxID_SAVE, wxT("Save"), Bitmap,
                                  wxT("Save experiment..."));
            pMainToolBar->SetToolLongHelp(wxID_SAVE, 
                wxT("Save the experiment you are working on now..."));

            // Save As...
            Bitmap = wxArtProvider::GetBitmap(wxART_FILE_SAVE_AS, wxART_TOOLBAR);
            pMainToolBar->AddTool(wxID_SAVEAS, wxT("Save As"), Bitmap,
                                  wxT("Save experiment as..."));
            pMainToolBar->SetToolLongHelp(wxID_SAVEAS, 
                wxT("Save the experiment you are working on now under a new "
                    "name..."));

            // Preferences...
            Bitmap = wxArtProvider::GetBitmap(wxART_HELP_SETTINGS, wxART_TOOLBAR);
            pMainToolBar->AddTool(wxID_PREFERENCES, wxT("Preferences"), Bitmap, 
                                  wxT("Change preferences..."));
            pMainToolBar->SetToolLongHelp(wxID_PREFERENCES, 
                wxT("You can configure your camera and other preferences here..."));

            // Capture...
            pMainToolBar->AddCheckTool(ID_CAPTURE, wxT("Capture"), usb_32x32_xpm, 
                                       usb_32x32_xpm, wxT("Capture video..."));
            pMainToolBar->SetToolLongHelp(ID_CAPTURE, 
                wxT("Connect to your camera and show what it sees in realtime..."));

            // Seperator...
            pMainToolBar->AddSeparator();

            // About...
            Bitmap = wxArtProvider::GetBitmap(wxART_INFORMATION, wxART_TOOLBAR);
            pMainToolBar->AddTool(wxID_ABOUT, wxT("About"), Bitmap, 
                                  wxT("About the software..."));
            pMainToolBar->SetToolLongHelp(wxID_ABOUT, 
                wxT("General " PACKAGE_STRING " build information..."));

            // Seperator...
            pMainToolBar->AddSeparator();

            // Exit...
            Bitmap = wxArtProvider::GetBitmap(wxART_QUIT, wxART_TOOLBAR);
            pMainToolBar->AddTool(wxID_EXIT, wxT("Exit"), Bitmap, 
                                  wxT("Exit software..."));
            pMainToolBar->SetToolLongHelp(wxID_EXIT, 
                wxT("You can exit Slither, but I'll prompt you incase you"
                    " still need to save your work..."));

        // Update the toolbar now that we've changed it...
        pMainToolBar->Realize();
        
        // Bind toolbar to main frame...
        SetToolBar(pMainToolBar);

    // Configure the notebook...   

        // Create image list...
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

        // Connect page changing event...
        Connect(wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGING,
                wxNotebookEventHandler(MainFrame::OnPageChanging));

    // Initialize videos grid...

        // Objects...
        wxGridCellAttr *pColumnAttributes   = NULL;

        // Create it...
        VideosGrid->CreateGrid(0, VIDEOS_GRID_COLUMNS);

        // Implement drop target... (doesn't work on OS X)
        VideosGrid->SetDropTarget(new VideosGridDropTarget(this));

        // Set label sizes...
        VideosGrid->SetRowLabelSize(0);
        VideosGrid->SetColLabelSize(30);

        // Set row sizes...
        VideosGrid->SetRowMinimalAcceptableHeight(25);

        // Set grid colours...
        VideosGrid->SetDefaultCellTextColour(wxColour(wxT("GREEN")));
        VideosGrid->SetGridLineColour(wxColour(wxT("BLUE")));
        VideosGrid->SetDefaultCellBackgroundColour(wxColour(wxT("DIM GREY")));

        // Initialize columns...
        
            // Title...
            VideosGrid->SetColLabelValue(TITLE, wxT("Title"));
            pColumnAttributes = VideosGrid->GetOrCreateCellAttr(0, TITLE);
            pColumnAttributes->SetReadOnly();
            VideosGrid->SetColAttr(TITLE, pColumnAttributes);
            
            // Date...
            VideosGrid->SetColLabelValue(DATE, wxT("Date"));

            // Time...
            VideosGrid->SetColLabelValue(TIME, wxT("Time"));

            // Technician...
            VideosGrid->SetColLabelValue(TECHNICIAN, wxT("Technician"));
            
            // Length...
            VideosGrid->SetColLabelValue(LENGTH, wxT("Length"));
            pColumnAttributes = VideosGrid->GetOrCreateCellAttr(0, LENGTH);
            pColumnAttributes->SetReadOnly();
            VideosGrid->SetColAttr(LENGTH, pColumnAttributes);
            
            // Size...
            VideosGrid->SetColLabelValue(SIZE, wxT("Size"));
            pColumnAttributes = VideosGrid->GetOrCreateCellAttr(0, SIZE);
            pColumnAttributes->SetReadOnly();
            VideosGrid->SetColAttr(SIZE, pColumnAttributes);

            // Notes...
            VideosGrid->SetColLabelValue(NOTES, wxT("Notes"));

        // Automatically resize...
        
            // Pad each column label...
            for(int nColumn = 0; nColumn < VideosGrid->GetNumberCols(); 
                nColumn++)
            {
                // Boost this column...
                VideosGrid->SetColSize(nColumn, 
                    VideosGrid->GetColSize(nColumn) + 55);
            }
        
        // Force a refresh...
        VideosGrid->ForceRefresh();

    // Initialize video player...

        // Allocate media control... (wxFormBuilder can't do this yet via XRC)
        pMediaPlayer = new wxMediaCtrl(VideoPreviewPanel, ID_VIDEO_PLAYER, 
                                       wxEmptyString, wxPoint(0, 0), 
                                       VideoPreviewPanel->GetSize(), 0, 
                                       wxEmptyString);

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
        pMediaPlayer->SetDropTarget(new VideosGridDropTarget(this));

    // Configure recording buttons...
    RecordButton->SetBitmapLabel(record_60x60_xpm);
    SaveRecordingButton->SetBitmapLabel(save_60x60_xpm);

    // Setup analysis pane...

        // Initialize microscope table... /* These tables are dummy values */

            // Leica...
            Microscope Leica(wxT("Leica Wild M3Z"));
            Leica.AddDiameter(6, 55.0f);
            Leica.AddDiameter(40, 7.8f);
            MicroscopeTable.push_back(Leica);
            
            // Random Works...
            Microscope RandomWorks(wxT("Dummy Microscope"));
            RandomWorks.AddDiameter(7, 52.0f);
            RandomWorks.AddDiameter(35, 6.2f);
            MicroscopeTable.push_back(RandomWorks);
            
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
        
            // Allocate analysis grid internal memory first...
            if(!AnalysisGrid->CreateGrid(1, 1))
                printf("CreateGrid failed...\n");

            // Analysis type...
            ChosenAnalysisType->SetSelection(ANALYSIS_BODY_SIZE);
            OnChooseAnalysisType(DummyCommandEvent);

    // Refresh...
    Refresh();

    // Send dummy size event to frame, forcing reevaluation of children...
    Layout();
    
    // Adjust media player...
    pMediaPlayer->SetSize(VideoPreviewPanel->GetSize());

    // Set status bar text...
    SetStatusText(wxT("Welcome to Slither..."));

    // Restore window size if any available...
    if(::wxGetApp().pConfiguration->HasEntry(wxT("/General/Width")))
    {
        // Read the previously saved values...
        wxSize Size;
        Size.SetWidth(::wxGetApp().pConfiguration->
                            Read(wxT("/General/Width"), 640));
        Size.SetHeight(::wxGetApp().pConfiguration->
                            Read(wxT("/General/Height"), 480));
        
        // Set the window size now...
        SetSize(Size);
    }

    // Show tip...
    ShowTip();

    // Create a new empty experiment...
    pExperiment = new Experiment(this);
}

// Compare two integers. Used for sorting rows in the videos grid...
int wxCMPFUNC_CONV MainFrame::CompareIntegers(int *pnFirst, int *pnSecond)
{
    // Compare...
    return *pnFirst - *pnSecond;
}

// Videos grid popup menu events for analyze...
void MainFrame::OnAnalyze(wxCommandEvent &Event)
{
    // They don't have a single video selected...
    if(VideosGrid->GetSelectedRows().GetCount() != 1)
    {
        // Alert user...
        wxMessageBox(wxT("Please select the video you wish to analyze first."));
        
        // Don't change the page...
        return;
    }
    
    // Switch to analysis...
    else
        MainNotebook->ChangeSelection(ANALYSIS_PANE);
}

// A frame has just been analyzed and is ready to be displayed...
void MainFrame::OnAnalysisFrameReadyTimer(wxTimerEvent &Event)
{
    // Get the thinking image...
    IplImage *pThinkingImage = pAnalysisThread->Tracker.GetThinkingImage();
    
        // Not ready yet...
        if(!pThinkingImage)
            return;

    // Display the image...
    cvShowImage("Analysis", pThinkingImage);
    
    // Cleanup...
    cvReleaseImage(&pThinkingImage);

        /* Update status every 3000 milliseconds...
        if(StatusUpdateStopWatch.Time() >= 3000)
        {
::wxMutexGuiEnter();
            
            // Get current position...
            int nCurrentFrame = (int) 
                cvGetCaptureProperty(pCapture, CV_CAP_PROP_POS_FRAMES);

            // Get total number of frames...
            int nTotalFrames = (int) 
                cvGetCaptureProperty(pCapture, CV_CAP_PROP_FRAME_COUNT);

            // We have the information we need to compute progress...
            if(nCurrentFrame && nTotalFrames)
            {
                // Compute total progress...
                int nProgress = 
                    (int)(((float) nCurrentFrame / nTotalFrames)  * 100.0);

                // Prepare current frame processing string...
                sTemp.Printf(wxT("%d / %d (%d %%)"), nCurrentFrame, 
                             nTotalFrames, nProgress);

                // Set it only if it has changed...
                if(nProgress != Frame.AnalysisGauge->GetValue())
                {
                    // Update the current frame...
                    Frame.AnalysisCurrentFrameStatus->ChangeValue(sTemp);

                    // Update the progress bar...
                    Frame.AnalysisGauge->SetValue(nProgress);
                }

                // The Quicktime backend appears to be buggy in that it keeps
                //  cycling through the video even after we have all frames.
                //  A temporary hack is to just break the analysis loop when we
                //  have both current frame, total frame, and they are equal...
                if(nCurrentFrame + 1 == nTotalFrames)
                    break;
            }
            
            // We cannot compute progress because the codec the backend on this
            //  platform is busted to shit...
            else
            {
                // Prepare current frame processing string...
                sTemp.Printf(wxT("%d"), nCurrentFrame);

                // Update the current frame...
                Frame.AnalysisCurrentFrameStatus->ChangeValue(sTemp);

                // Pulse progress bar...
                Frame.AnalysisGauge->Pulse();
            }
            
            // Reset the status update timer...
            StatusUpdateStopWatch.Start();
::wxMutexGuiLeave();
        }
    }

    // Show total time...
    sTemp.Printf(wxT("Took %.3f s..."), AnalysisStopWatch.Time() / 1000.0f);
::wxMutexGuiEnter();
    Frame.AnalysisStatusList->Append(sTemp);
::wxMutexGuiLeave();*/

    // Let HighGUI process events...
    cvWaitKey(1);
  ::wxGetApp().Yield();
}

// Begin analysis button hit...
void MainFrame::OnBeginAnalysis(wxCommandEvent &Event)
{
    // Analysis already running, abort...
    if(AnalysisTimer.IsRunning())
        return;

    // They don't have a single video selected...
    if(VideosGrid->GetSelectedRows().GetCount() != 1)
    {
        // Alert user...
        wxMessageBox(wxT("You must first select a single piece of video to"
                         " analyze. You will be taken to your available media"
                         " now."));

        // Take the user back to the data pane...
        MainNotebook->ChangeSelection(DATA_PANE);

        /* TODO: Highlight videos grid here... */

        // Done...
        return;
    }

    // Create the analysis thread and check for error...
    pAnalysisThread = new AnalysisThread(*this);
    if(pAnalysisThread->Create(wxTHREAD_DETACHED) != wxTHREAD_NO_ERROR)
    {
        // Alert...
        wxLogError(wxT("Unable to create AnalysisThread..."));

        // Cleanup and abort...
        delete pAnalysisThread;
        return;
    }

    // Initiate the analysis timer...
    AnalysisTimer.Start(50, wxTIMER_CONTINUOUS);

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
                case 0: sTemp = wxT("Length (mm)"); break;
                case 1: sTemp = wxT("Width (mm)"); break;
                case 2: sTemp = wxT("Area (mm^2)"); break;
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

// Cancel analysis button hit...
void MainFrame::OnCancelAnalysis(wxCommandEvent &Event)
{
    // Signal to analysis thread that it should end when convenient by disabling
    //  the cancel analysis button...
    CancelAnalysisButton->Disable();
    
    // Begin analysis button will be re-enabled when analysis thread 
    //  terminates...
    BeginAnalysisButton->Disable();
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
    if(pCaptureThread->Create(wxTHREAD_DETACHED) != wxTHREAD_NO_ERROR)
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

// A microscope name has been chosen...
void MainFrame::OnChooseMicroscopeName(wxCommandEvent &Event)
{
    // Get the chosen microscope...
    Microscope &ChosenMicroscope = MicroscopeTable.at(Event.GetInt());

    // Clear the choosable zooms...
    ChosenMicroscopeTotalZoom->Clear();

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
    sDiameter.Printf(wxT("%.2f mm"), fFieldOfView);
    
    // Update diameter text box...
    FieldOfViewDiameter->SetValue(sDiameter);
}

// Get the total size of all videos in the videos grid...
wxULongLong MainFrame::GetTotalVideoSize()
{
    // Variables...
    wxULongLong ulTotalSize = 0;

    // Calculate total size...
    for(unsigned int unRow = 0; 
        unRow < (unsigned) VideosGrid->GetNumberRows(); 
        unRow++)
    {
        // Find the video...
        wxString sPath = pExperiment->GetCachePath() + wxT("/videos/") + 
                         VideosGrid->GetCellValue(unRow, TITLE);

        // Load it...
        wxFileName VideoFile(sPath);

            // Failed...
            if(!VideoFile.IsOk())
                continue;

        // Size...
        ulTotalSize += VideoFile.GetSize();
    }
    
    // Done...
    return ulTotalSize;
}

// Is experiment contain a video by a specific name?
bool MainFrame::IsExperimentContainVideo(wxString sName)
{
    // Search the video list...
    for(int nRow = 0; nRow < VideosGrid->GetNumberRows(); nRow++)
    {
        // Found match...
        if(VideosGrid->GetCellValue(nRow, MainFrame::TITLE).Lower() == 
           sName.Lower())
            return true;
    }
    
    // Not found...
    return false;
}

// Is experiment contain a video by a specific name, except a row?
bool MainFrame::IsExperimentContainVideoExceptRow(wxString sName, int nSkipRow)
{
    // Search the video list...
    for(int nRow = 0; nRow < VideosGrid->GetNumberRows(); nRow++)
    {
        // Not checking this row, skip it...
        if(nSkipRow == nRow)
            continue;

        // Found match...
        if(VideosGrid->GetCellValue(nRow, MainFrame::TITLE).Lower() == 
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
        // They don't have a single video selected...
        if(VideosGrid->GetSelectedRows().GetCount() != 1)
        {
            // Veto the page change and go back to the data pane...
            Event.Veto();
            MainNotebook->ChangeSelection(ANALYSIS_PANE);

            // Alert user...
            wxMessageBox(wxT("Please select a single video to analyze first."));

            // Done...
            return;
        }
    }*/
}

// Videos grid popup menu event for play...
void MainFrame::OnPlay(wxCommandEvent &Event)
{
    // Get the complete path to the video to play...
    
        // Find the row selected...
        wxArrayInt SelectedRows = VideosGrid->GetSelectedRows();
        int nRow = SelectedRows[0];
        
        // Generate complete path...
        wxString sPath = pExperiment->GetCachePath() + wxT("/videos/") +
                         VideosGrid->GetCellValue(nRow, TITLE);

    // Load media and check for error...
    if(!pMediaPlayer->Load(sPath))
    {
        // Alert and abort...
        wxMessageBox(wxT("Unable to load media. Perhaps it uses an unknown "
                         "codec?"));
        return;
    }

    // Update media length in the videos grid...

        // Calculate minutes and seconds of video...
        wxLongLong llMediaLength = pMediaPlayer->Length();
        int nMinutes = (int) (llMediaLength / 60000).GetValue();
        int nSeconds = (int) ((llMediaLength % 60000) / 1000).GetValue();

        // Format string and update field...
        wxString sDuration;
        sDuration.Printf(wxT("%2i:%02i"), nMinutes, nSeconds);
        VideosGrid->SetCellValue(nRow, LENGTH, sDuration);
    
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

// Videos grid has had a cell change...
void MainFrame::OnExperimentChangeCell(wxGridEvent &Event)
{
    // No experiment is loaded...
    if(!IsExperimentLoaded())
        return;

    /* Is this somewhere within the title column?
    if(Event.GetCol() == TITLE)
    {
        // What did the user just try and rename the video's title to?
        wxString sNewTitle = VideosGrid->GetCellValue(Event.GetRow(), TITLE)
        
        IsExperimentContainVideo(
        
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
            wxFileDialog 
                FileDialog(this, wxT("Please select a Slither experiment..."), 
                         ::wxGetApp().StandardPaths.GetDocumentsDir(), 
                           ExperimentTitle->GetValue() + wxT(".sex"), 
                           wxT("Slither experiment (*.sex)|*.sex"),
                           wxOPEN | wxFD_PREVIEW | wxFD_FILE_MUST_EXIST);

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

// Videos grid popup menu event for remove...
void MainFrame::OnRemove(wxCommandEvent &Event)
{
    // Remove each row backwards to preserve row ordinals...
    wxArrayInt SelectedRows = VideosGrid->GetSelectedRows();
    SelectedRows.Sort(CompareIntegers);
    for(int nIndex = SelectedRows.GetCount() - 1; nIndex >= 0; --nIndex)
    {
        // Double check with the user first...
        wxMessageDialog 
        Message(this, wxT("Are you sure you want to remove the following video"
                          " from your experiment?\n\n") + 
                      VideosGrid->GetCellValue(SelectedRows[nIndex], TITLE), 
                wxT("Remove Video"), 
                wxOK | wxCANCEL | wxICON_EXCLAMATION);
                        
            // They requested to not remove the video from the experiment...
            if(Message.ShowModal() == wxID_CANCEL)
                continue;

        // Generate a complete path to the video selected...
        wxString sPath = pExperiment->GetCachePath() + wxT("/videos/") + 
                         VideosGrid->GetCellValue(SelectedRows[nIndex], TITLE);

        // Remove it and check for error...
        if(!::wxRemoveFile(sPath))
        {
            // Log it and skip to next...
            wxLogError(wxString::Format(wxT("Unable to delete:\n\n%s\n(Row: %d)"),
                             sPath.c_str(), SelectedRows[nIndex]));
            continue;
        }

        // Remove the row...
        VideosGrid->DeleteRows(SelectedRows[nIndex]);
        
        // Trigger need save...
        pExperiment->TriggerNeedSave();

        // Update embedded videos count...
        wxString sEmbeddedVideos; 
        sEmbeddedVideos << VideosGrid->GetNumberRows();
        EmbeddedVideos->ChangeValue(sEmbeddedVideos);
        
        // Update total embedded video size...
        wxULongLong ulTotalSize = GetTotalVideoSize();
        ulTotalSize /= (1024 * 1024);
        TotalSize->ChangeValue(ulTotalSize.ToString() + wxT(" MB"));
    }
}

// Videos grid popup menu event for rename...
void MainFrame::OnRename(wxCommandEvent &Event)
{
    // Find the selected row...
    wxArrayInt SelectedRow = VideosGrid->GetSelectedRows();
    
        // Verify that there is but one...
        if(SelectedRow.GetCount() != 1)
            return;

    // Get the original name...
    wxString sOriginalName = VideosGrid->GetCellValue(SelectedRow[0], TITLE);

    // Prepare the text entry dialog...
    wxTextEntryDialog Dialog(this, wxT("Please enter the new unique name:"),
                             wxT("Rename video"),
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
        wxLogError(wxT("You cannot rename the video to that. Make sure you do"
                       " not have any spaces."));
        return;
    }

    // Verify that this name isn't already taken...
    if(IsExperimentContainVideoExceptRow(Dialog.GetValue(), SelectedRow[0]))
    {
        // Log error and then abort...
        wxLogError(wxT("A video already exists in your experiment by that name."
                       " Try something else."));
        return;
    }

    // Rename the file...
    if(!::wxRenameFile(pExperiment->GetCachePath() + wxT("/videos/") + 
                         sOriginalName, 
                       pExperiment->GetCachePath() + wxT("/videos/") + 
                         Dialog.GetValue(), false))
    {
        // Log error and then abort...
        wxLogError(wxT("Unable to rename video..."));
        return;    
    }

    // Rename the title in the videos grid...
    VideosGrid->SetCellValue(SelectedRow[0], TITLE, Dialog.GetValue());

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
    wxFileDialog FileDialog(this, wxT("Save Slither experiment as..."), 
                          ::wxGetApp().StandardPaths.GetDocumentsDir(), 
                            ExperimentTitle->GetValue() + wxT(".sex"), 
                            wxT("Slither experiment (*.sex)|*.sex"),
                            wxSAVE | wxOVERWRITE_PROMPT | wxFD_PREVIEW);

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
      ::wxGetApp().pConfiguration->Flush();

    // Allow child controls to resize appropriately...
    Event.Skip();
}

// Videos grid popup menu event for stop...
void MainFrame::OnStop(wxCommandEvent &Event)
{
    // Stop...
    if(pMediaPlayer)
        pMediaPlayer->Stop();
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
    ProcessCommand(ID_STOP_ANALYSIS);
    
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
        AboutDialogInfo.SetName(wxT(PACKAGE_NAME));
        AboutDialogInfo.SetVersion(wxT(PACKAGE_VERSION));
    
    
        // Description...
        AboutDialogInfo.SetDescription(wxString::Format(
            wxT("Slither is a free and open source tool available for\n"
                "Linux, Win32, and Mac OS X. It is designed to help\n"
                "you perform tap experiments on C. elegans within a\n"
                "lab. It manages your experiments by capturing and\n"
                "organizing video directly from your lab's dissection\n"
                "microscope camera. It can also try to analyze what it\n"
                "thinks it saw, generating some useful data\n"
                "automagically for you.\n\n"

                "Your hardware appears to be %s (%s).\n\n"
                        
                "You are running %s. This is of the %s\n"
                "family of operating systems.\n\n"
                        
                "%s was built on %s at\n"
                "%s. It was developed using %s\n"
                "(provided by the %s port).\n\n"
                
                "Compiled against version " CV_VERSION " of Intel's\n"
                "OpenCV.\n\n"),

                PlatformInfo.GetArchName(PlatformInfo.GetArchitecture()).c_str(),
                PlatformInfo.GetEndiannessName(PlatformInfo.GetEndianness()).
                                                MakeLower().c_str(),
                PlatformInfo.GetOperatingSystemIdName().c_str(),
                PlatformInfo.GetOperatingSystemFamilyName().c_str(),
                wxT(PACKAGE_STRING),
                wxT(__DATE__),
                wxT(__TIME__),
                wxVERSION_STRING,
                PlatformInfo.GetPortIdName().c_str()));

        // Developers... (kind of)
        AboutDialogInfo.AddDeveloper(wxT("Current Developers:"));
        AboutDialogInfo.AddDeveloper(wxT("\tKip Warner <Kip@TheVertigo.com>"));
        AboutDialogInfo.AddDeveloper(wxT(""));
        AboutDialogInfo.AddDeveloper(wxT("Beta Testers:"));
        AboutDialogInfo.AddDeveloper(wxT("\tAaron Dowler <adowlerb@shaw.ca>"));
        AboutDialogInfo.AddDeveloper(wxT("\tMike Butterfield <butter@interchange.ubc.ca>"));
        AboutDialogInfo.AddDeveloper(wxT(""));
        AboutDialogInfo.AddDeveloper(wxT("Thanks:"));
        AboutDialogInfo.AddDeveloper(wxT("\tAaron Dowler <adowlerb@shaw.ca> (hardware)"));
        AboutDialogInfo.AddDeveloper(wxT("\tCatharine Rankin <crankin@psych.ubc.ca> (the opportunity)"));
        AboutDialogInfo.AddDeveloper(wxT("\tEd Knorr <knorr@cs.ubc.ca> (criticisms)"));
        AboutDialogInfo.AddDeveloper(wxT("\tThomas L. Adelman <tadelman2@yahoo.com> (guidance)"));
        AboutDialogInfo.AddDeveloper(wxT("\tVarun Ramraj <silverballer47@gmail.com> (criticisms)"));
        
        // Documentation writers...
        AboutDialogInfo.AddDocWriter(wxT("Kip Warner <Kip@TheVertigo.com>"));
            
        // Website...
        AboutDialogInfo.SetWebSite(wxT("http://slither.thevertigo.com/"));
        
        // Set license...
        AboutDialogInfo.SetLicense(
            wxT("Slither is free software; you can redistribute it and/or "
                "modify it under the terms of the GNU General Public License "
                "as published by the Free Software Foundation; either version "
                "2 of the License, or (at your option) any later version.\n\n"

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
void MainFrame::OnVideoCellDoubleLeftClick(wxGridEvent &Event)
{
    // Move the grid cursor to the cell under the mouse cursor...
    VideosGrid->SetGridCursor(Event.GetRow(), Event.GetCol());
    
    // Is this cell editable?
    if(VideosGrid->CanEnableCellControl())
    {
        // Enable and show the edit control...
        VideosGrid->EnableCellEditControl();
        VideosGrid->ShowCellEditControl();
    }
    
    // Otherwise jump to the analysis pane...
    else
        ProcessCommand(ID_ANALYZE);
}

// User left clicked a cell...
void MainFrame::OnVideoCellLeftClick(wxGridEvent &Event)
{
    // Select the entire row...
    VideosGrid->SelectRow(Event.GetRow(), Event.ControlDown() ? true : false);
}

// User right clicked a cell...
void MainFrame::OnVideoCellRightClick(wxGridEvent &Event)
{
    // Control down, add to selection...
    if(Event.ControlDown())
        VideosGrid->SelectRow(Event.GetRow(), true);

    // Control not down...
    else
    {
        // Over unselected row, select it only...
        if(!VideosGrid->IsInSelection(Event.GetRow(), Event.GetCol()))
            VideosGrid->SelectRow(Event.GetRow());
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
        if(VideosGrid->GetSelectedRows().GetCount() == 1)
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
    VideosGrid->PopupMenu(&Menu, Event.GetPosition());
}

// Quit command event handler...
void MainFrame::OnQuit(wxCommandEvent &Event)
{
    // Make sure capture and analysis threads exit cleanly first...
    bExiting = true;
    while(CaptureTimer.IsRunning() || AnalysisTimer.IsRunning())
        wxSleep(1);

    // Close experiment first...
    ProcessCommand(wxID_CLOSE); 

        // Experiment still loaded, abort...
        if(pExperiment)
        {
            // But remember to untrigger exit flag first...
            bExiting = false;
            return;
        }

    // Close the frame...
    Close();
}

// Preferences command event handler...
void MainFrame::OnPreferences(wxCommandEvent &Event)
{
    // Initialize preferences dialog...
    PreferencesDialog Preferences(this);
    
    // Display it...
    Preferences.ShowModal();
}

// User clicked the (X) / system menu (Windows) or Close() was invoked...
void MainFrame::OnSystemClose(wxCloseEvent &Event)
{
    // Make sure capture and analysis threads exit cleanly first...
    bExiting = true;
    while(CaptureTimer.IsRunning() || AnalysisTimer.IsRunning())
        wxSleep(1);

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
           wxStandardPaths  StandardPaths;
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
                sTipsPath = StandardPaths.GetResourcesDir() + wxT("/tips.txt");

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

