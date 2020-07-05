///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Mar 19 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifdef WX_PRECOMP

#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif //__BORLANDC__

#else
#include <wx/wx.h>
#endif //WX_PRECOMP

#include "Resources.h"

#include "Resources/usb_32x32.xpm"

///////////////////////////////////////////////////////////////////////////

MainFrame_Base::MainFrame_Base( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxSize( 640,480 ), wxDefaultSize );
	this->SetExtraStyle( wxFRAME_EX_CONTEXTHELP );
	this->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 70, 90, 90, false, wxEmptyString ) );
	
	MenuBar = new wxMenuBar( 0 );
	FileMenu = new wxMenu();
	wxMenuItem* NewMenuItem;
	NewMenuItem = new wxMenuItem( FileMenu, wxID_NEW, wxString( wxT("&New") ) + wxT('\t') + wxT("Ctrl+N"), wxT("Create a new experiment..."), wxITEM_NORMAL );
	FileMenu->Append( NewMenuItem );
	
	wxMenuItem* OpenMenuItem;
	OpenMenuItem = new wxMenuItem( FileMenu, wxID_OPEN, wxString( wxT("&Open") ) + wxT('\t') + wxT("Ctrl+O"), wxT("Open an existing experiment..."), wxITEM_NORMAL );
	FileMenu->Append( OpenMenuItem );
	
	FileMenu->AppendSeparator();
	
	wxMenuItem* ImportMediaMenuItem;
	ImportMediaMenuItem = new wxMenuItem( FileMenu, ID_IMPORT_MEDIA, wxString( wxT("&Import Media") ) + wxT('\t') + wxT("Ctrl-m"), wxT("Import new media into the experiment..."), wxITEM_NORMAL );
	FileMenu->Append( ImportMediaMenuItem );
	
	FileMenu->AppendSeparator();
	
	wxMenuItem* SaveMenuItem;
	SaveMenuItem = new wxMenuItem( FileMenu, wxID_SAVE, wxString( wxT("&Save") ) + wxT('\t') + wxT("Ctrl+S"), wxT("Save experiment..."), wxITEM_NORMAL );
	FileMenu->Append( SaveMenuItem );
	
	wxMenuItem* SaveAsMenuItem;
	SaveAsMenuItem = new wxMenuItem( FileMenu, wxID_SAVEAS, wxString( wxT("Save &As") ) + wxT('\t') + wxT("Shift+Ctrl+S"), wxT("Save an experiment as..."), wxITEM_NORMAL );
	FileMenu->Append( SaveAsMenuItem );
	
	wxMenuItem* RevertMenuItem;
	RevertMenuItem = new wxMenuItem( FileMenu, wxID_REVERT, wxString( wxT("&Revert") ) , wxT("Revert to experiment in the previously saved state..."), wxITEM_NORMAL );
	FileMenu->Append( RevertMenuItem );
	
	FileMenu->AppendSeparator();
	
	wxMenuItem* CloseMenuItem;
	CloseMenuItem = new wxMenuItem( FileMenu, wxID_CLOSE, wxString( wxT("&Close") ) + wxT('\t') + wxT("Ctrl+W"), wxT("Close the open experiment..."), wxITEM_NORMAL );
	FileMenu->Append( CloseMenuItem );
	
	wxMenuItem* QuitMenuItem;
	QuitMenuItem = new wxMenuItem( FileMenu, wxID_EXIT, wxString( wxT("&Quit") ) + wxT('\t') + wxT("Ctrl+Q"), wxT("Quit Slither..."), wxITEM_NORMAL );
	FileMenu->Append( QuitMenuItem );
	
	MenuBar->Append( FileMenu, wxT("&File") );
	
	ViewMenu = new wxMenu();
	wxMenuItem* FullScreenMenuItem;
	FullScreenMenuItem = new wxMenuItem( ViewMenu, ID_FULLSCREEN, wxString( wxT("&Full screen") ) , wxT("Toggle full screen mode..."), wxITEM_CHECK );
	ViewMenu->Append( FullScreenMenuItem );
	
	wxMenuItem* ShowImageAnalysisMenuItem;
	ShowImageAnalysisMenuItem = new wxMenuItem( ViewMenu, ID_TOGGLE_IMAGE_ANALYSIS_WINDOW, wxString( wxT("&Show image analysis window") ) + wxT('\t') + wxT("Ctrl+i"), wxT("Show image analysis window..."), wxITEM_CHECK );
	ViewMenu->Append( ShowImageAnalysisMenuItem );
	
	MenuBar->Append( ViewMenu, wxT("&View") );
	
	HelpMenu = new wxMenu();
	wxMenuItem* AboutMenuItem;
	AboutMenuItem = new wxMenuItem( HelpMenu, wxID_ABOUT, wxString( wxT("&About") ) + wxT('\t') + wxT("F1"), wxT("Some general information about this version of Slither..."), wxITEM_NORMAL );
	HelpMenu->Append( AboutMenuItem );
	
	MenuBar->Append( HelpMenu, wxT("&Help") );
	
	this->SetMenuBar( MenuBar );
	
	MainToolBar = this->CreateToolBar( wxTB_HORIZONTAL|wxTB_NOICONS|wxTB_TEXT, wxID_ANY ); 
	MainToolBar->AddTool( wxID_NEW, wxT("New"), wxNullBitmap, wxNullBitmap, wxITEM_NORMAL, wxT("Create a new experiment..."), wxT("Create a new experiment...") );
	MainToolBar->AddTool( wxID_OPEN, wxT("Open"), wxNullBitmap, wxNullBitmap, wxITEM_NORMAL, wxT("Open existing experiment..."), wxT("Open existing experiment...") );
	MainToolBar->AddTool( wxID_SAVE, wxT("Save"), wxNullBitmap, wxNullBitmap, wxITEM_NORMAL, wxT("Save experiment..."), wxT("Save experiment...") );
	MainToolBar->AddTool( wxID_SAVEAS, wxT("Save As"), wxNullBitmap, wxNullBitmap, wxITEM_NORMAL, wxT("Save experiment as..."), wxT("Save experiment as...") );
	MainToolBar->AddTool( ID_CAPTURE, wxT("Capture"), wxBitmap( usb_32x32_xpm ), wxNullBitmap, wxITEM_CHECK, wxT("Connect to your camera and show what it sees in realtime..."), wxT("Connect to your camera and show what it sees in realtime...") );
	MainToolBar->AddSeparator();
	MainToolBar->AddTool( wxID_ABOUT, wxT("About"), wxNullBitmap, wxNullBitmap, wxITEM_NORMAL, wxT("Some general information about this version of Slither..."), wxT("Some general information about this version of Slither...") );
	MainToolBar->AddSeparator();
	MainToolBar->AddTool( wxID_EXIT, wxT("Quit"), wxNullBitmap, wxNullBitmap, wxITEM_NORMAL, wxT("You can exit Slither, but I'll prompt you incase you still need to save your work..."), wxT("You can exit Slither, but I'll prompt you incase you still need to save your work...") );
	MainToolBar->Realize();
	
	wxBoxSizer* EverythingSizer;
	EverythingSizer = new wxBoxSizer( wxHORIZONTAL );
	
	MainNotebook = new wxNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNB_LEFT );
	DataPane = new wxPanel( MainNotebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	DataPane->SetHelpText( wxT("All available media for this experiment.") );
	
	wxBoxSizer* DataPaneSizer;
	DataPaneSizer = new wxBoxSizer( wxVERTICAL );
	
	
	DataPaneSizer->Add( 0, 10, 0, wxEXPAND, 5 );
	
	wxFlexGridSizer* DataPaneTopSizer;
	DataPaneTopSizer = new wxFlexGridSizer( 1, 2, 10, 10 );
	DataPaneTopSizer->AddGrowableCol( 0 );
	DataPaneTopSizer->AddGrowableCol( 1 );
	DataPaneTopSizer->AddGrowableRow( 0 );
	DataPaneTopSizer->AddGrowableRow( 1 );
	DataPaneTopSizer->SetFlexibleDirection( wxBOTH );
	DataPaneTopSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	wxStaticBoxSizer* GeneralStaticSizer;
	GeneralStaticSizer = new wxStaticBoxSizer( new wxStaticBox( DataPane, wxID_ANY, wxT("General:") ), wxVERTICAL );
	
	wxFlexGridSizer* GeneralSizer;
	GeneralSizer = new wxFlexGridSizer( 4, 2, 0, 0 );
	GeneralSizer->AddGrowableCol( 1 );
	GeneralSizer->AddGrowableRow( 3 );
	GeneralSizer->SetFlexibleDirection( wxBOTH );
	GeneralSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText72 = new wxStaticText( DataPane, wxID_ANY, wxT("Title:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText72->Wrap( -1 );
	GeneralSizer->Add( m_staticText72, 0, wxALL, 5 );
	
	ExperimentTitle = new wxTextCtrl( DataPane, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( -1,-1 ), 0 );
	ExperimentTitle->SetMaxLength( 128 ); 
	ExperimentTitle->SetToolTip( wxT("Enter a descriptive name for your tap experiment. It can be whatever you like.") );
	
	GeneralSizer->Add( ExperimentTitle, 0, wxALL|wxEXPAND, 5 );
	
	m_staticText92 = new wxStaticText( DataPane, wxID_ANY, wxT("Media:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText92->Wrap( -1 );
	GeneralSizer->Add( m_staticText92, 0, wxALL, 5 );
	
	EmbeddedMedia = new wxTextCtrl( DataPane, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( -1,-1 ), wxTE_CENTRE|wxTE_READONLY );
	EmbeddedMedia->SetMaxLength( 128 ); 
	EmbeddedMedia->SetToolTip( wxT("This is the total number of videos in your experiment.") );
	
	GeneralSizer->Add( EmbeddedMedia, 0, wxALL, 5 );
	
	m_staticText911 = new wxStaticText( DataPane, wxID_ANY, wxT("Total Size:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText911->Wrap( -1 );
	GeneralSizer->Add( m_staticText911, 0, wxALL, 5 );
	
	TotalSize = new wxTextCtrl( DataPane, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( -1,-1 ), wxTE_CENTRE|wxTE_READONLY );
	TotalSize->SetMaxLength( 128 ); 
	TotalSize->SetToolTip( wxT("This is the combined size of all of your microscope videos, indicating the size of your experiment.") );
	
	GeneralSizer->Add( TotalSize, 0, wxALL, 5 );
	
	m_staticText1221 = new wxStaticText( DataPane, wxID_ANY, wxT("Notes:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1221->Wrap( -1 );
	GeneralSizer->Add( m_staticText1221, 0, wxALL, 5 );
	
	ExperimentNotes = new wxTextCtrl( DataPane, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( -1,-1 ), wxTE_MULTILINE );
	ExperimentNotes->SetMaxLength( 4096 ); 
	ExperimentNotes->SetToolTip( wxT("You may enter notes specific to this experiment here.") );
	
	GeneralSizer->Add( ExperimentNotes, 1, wxALL|wxEXPAND, 5 );
	
	GeneralStaticSizer->Add( GeneralSizer, 1, wxALL|wxEXPAND, 5 );
	
	DataPaneTopSizer->Add( GeneralStaticSizer, 1, wxEXPAND, 5 );
	
	wxStaticBoxSizer* VideoPreviewSizer;
	VideoPreviewSizer = new wxStaticBoxSizer( new wxStaticBox( DataPane, wxID_ANY, wxT("Playback:") ), wxVERTICAL );
	
	VideoPreviewPanel = new wxPanel( DataPane, wxID_ANY, wxDefaultPosition, wxSize( 160,120 ), wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
	VideoPreviewPanel->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_3DDKSHADOW ) );
	VideoPreviewPanel->SetMinSize( wxSize( 160,120 ) );
	
	VideoPreviewSizer->Add( VideoPreviewPanel, 1, wxALL|wxEXPAND|wxSHAPED|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	ExtractFrameButton = new wxButton( DataPane, wxID_ANY, wxT("Extract Frame"), wxDefaultPosition, wxDefaultSize, 0 );
	ExtractFrameButton->Enable( false );
	ExtractFrameButton->SetToolTip( wxT("Click this to extract the currently displayed image in the playback window to your list of media...") );
	
	VideoPreviewSizer->Add( ExtractFrameButton, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxEXPAND, 5 );
	
	DataPaneTopSizer->Add( VideoPreviewSizer, 1, wxEXPAND, 5 );
	
	DataPaneSizer->Add( DataPaneTopSizer, 1, wxEXPAND, 5 );
	
	
	DataPaneSizer->Add( 10, 0, 0, wxEXPAND, 5 );
	
	wxFlexGridSizer* DataPaneBottomSizer;
	DataPaneBottomSizer = new wxFlexGridSizer( 1, 1, 10, 10 );
	DataPaneBottomSizer->AddGrowableCol( 0 );
	DataPaneBottomSizer->AddGrowableRow( 0 );
	DataPaneBottomSizer->SetFlexibleDirection( wxBOTH );
	DataPaneBottomSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	wxStaticBoxSizer* MediaGridSizer;
	MediaGridSizer = new wxStaticBoxSizer( new wxStaticBox( DataPane, wxID_ANY, wxT("Media:") ), wxHORIZONTAL );
	
	MediaGrid = new wxGrid( DataPane, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	
	// Grid
	MediaGrid->CreateGrid( 0, 7 );
	MediaGrid->EnableEditing( true );
	MediaGrid->EnableGridLines( true );
	MediaGrid->EnableDragGridSize( false );
	MediaGrid->SetMargins( 0, 0 );
	
	// Columns
	MediaGrid->EnableDragColMove( false );
	MediaGrid->EnableDragColSize( true );
	MediaGrid->SetColLabelSize( 30 );
	MediaGrid->SetColLabelAlignment( wxALIGN_CENTRE, wxALIGN_CENTRE );
	
	// Rows
	MediaGrid->EnableDragRowSize( true );
	MediaGrid->SetRowLabelSize( 80 );
	MediaGrid->SetRowLabelAlignment( wxALIGN_CENTRE, wxALIGN_CENTRE );
	
	// Label Appearance
	
	// Cell Defaults
	MediaGrid->SetDefaultCellAlignment( wxALIGN_LEFT, wxALIGN_TOP );
	MediaGrid->SetToolTip( wxT("Right click on a video to see additional options. If you are on a Mac and have only one mouse button, hold down on Ctrl and then click.") );
	
	MediaGridSizer->Add( MediaGrid, 1, wxALL|wxEXPAND, 5 );
	
	DataPaneBottomSizer->Add( MediaGridSizer, 1, wxEXPAND, 5 );
	
	DataPaneSizer->Add( DataPaneBottomSizer, 1, wxEXPAND, 5 );
	
	DataPane->SetSizer( DataPaneSizer );
	DataPane->Layout();
	DataPaneSizer->Fit( DataPane );
	MainNotebook->AddPage( DataPane, wxT("Data"), false );
	CapturePane = new wxPanel( MainNotebook, wxID_ANY, wxDefaultPosition, wxSize( -1,-1 ), wxTAB_TRAVERSAL );
	CapturePane->Enable( false );
	
	wxBoxSizer* CapturePaneSizer;
	CapturePaneSizer = new wxBoxSizer( wxHORIZONTAL );
	
	wxStaticBoxSizer* PreviewImageSizer;
	PreviewImageSizer = new wxStaticBoxSizer( new wxStaticBox( CapturePane, wxID_ANY, wxT("Preview:") ), wxVERTICAL );
	
	CaptureImagePanel = new wxPanel( CapturePane, wxID_ANY, wxDefaultPosition, wxSize( 320,240 ), wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
	CaptureImagePanel->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_3DDKSHADOW ) );
	CaptureImagePanel->SetMinSize( wxSize( 320,240 ) );
	
	PreviewImageSizer->Add( CaptureImagePanel, 1, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL|wxEXPAND|wxSHAPED, 5 );
	
	m_staticText11 = new wxStaticText( CapturePane, wxID_ANY, wxT("TODO: Implement recording and codec selection."), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText11->Wrap( -1 );
	m_staticText11->SetForegroundColour( wxColour( 255, 0, 0 ) );
	
	PreviewImageSizer->Add( m_staticText11, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	CapturePaneSizer->Add( PreviewImageSizer, 3, wxALIGN_CENTER_HORIZONTAL|wxEXPAND, 5 );
	
	
	CapturePaneSizer->Add( 10, 0, 0, wxEXPAND, 5 );
	
	wxStaticBoxSizer* SetupSizer;
	SetupSizer = new wxStaticBoxSizer( new wxStaticBox( CapturePane, wxID_ANY, wxT("Setup:") ), wxVERTICAL );
	
	RecordButton = new wxBitmapButton( CapturePane, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	RecordButton->Enable( false );
	
	RecordButton->Enable( false );
	
	SetupSizer->Add( RecordButton, 0, wxALL, 5 );
	
	SaveRecordingButton = new wxBitmapButton( CapturePane, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	SaveRecordingButton->Enable( false );
	
	SaveRecordingButton->Enable( false );
	
	SetupSizer->Add( SaveRecordingButton, 0, wxALL, 5 );
	
	CapturePaneSizer->Add( SetupSizer, 1, wxEXPAND, 5 );
	
	CapturePane->SetSizer( CapturePaneSizer );
	CapturePane->Layout();
	CapturePaneSizer->Fit( CapturePane );
	MainNotebook->AddPage( CapturePane, wxT("Capture"), false );
	AnalysisPane = new wxPanel( MainNotebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* AnalysisPaneSizer;
	AnalysisPaneSizer = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer7;
	bSizer7 = new wxBoxSizer( wxHORIZONTAL );
	
	wxStaticBoxSizer* AnalysisSetupSizer;
	AnalysisSetupSizer = new wxStaticBoxSizer( new wxStaticBox( AnalysisPane, wxID_ANY, wxT("Setup:") ), wxVERTICAL );
	
	
	AnalysisSetupSizer->Add( 0, 10, 0, wxEXPAND, 5 );
	
	wxStaticBoxSizer* MicroscopeStaticSizer;
	MicroscopeStaticSizer = new wxStaticBoxSizer( new wxStaticBox( AnalysisPane, wxID_ANY, wxT("Microscope / Total Zoom:") ), wxHORIZONTAL );
	
	wxArrayString ChosenMicroscopeNameChoices;
	ChosenMicroscopeName = new wxChoice( AnalysisPane, wxID_ANY, wxDefaultPosition, wxDefaultSize, ChosenMicroscopeNameChoices, 0 );
	ChosenMicroscopeName->SetSelection( 0 );
	ChosenMicroscopeName->SetToolTip( wxT("The specific make and model of your camera determines the available zooms.") );
	
	MicroscopeStaticSizer->Add( ChosenMicroscopeName, 1, wxALL|wxEXPAND, 5 );
	
	wxArrayString ChosenMicroscopeTotalZoomChoices;
	ChosenMicroscopeTotalZoom = new wxChoice( AnalysisPane, wxID_ANY, wxDefaultPosition, wxDefaultSize, ChosenMicroscopeTotalZoomChoices, 0 );
	ChosenMicroscopeTotalZoom->SetSelection( 0 );
	ChosenMicroscopeTotalZoom->SetToolTip( wxT("This zoom is the total zoom factor and not just for the objective or ocular lense. The total zoom factor is used to calculate the field of view diameter.") );
	
	MicroscopeStaticSizer->Add( ChosenMicroscopeTotalZoom, 0, wxALL, 5 );
	
	AnalysisSetupSizer->Add( MicroscopeStaticSizer, 0, wxEXPAND, 5 );
	
	
	AnalysisSetupSizer->Add( 0, 10, 0, wxEXPAND, 5 );
	
	wxStaticBoxSizer* sbSizer81;
	sbSizer81 = new wxStaticBoxSizer( new wxStaticBox( AnalysisPane, wxID_ANY, wxT("Field of View Diameter:") ), wxHORIZONTAL );
	
	FieldOfViewDiameter = new wxTextCtrl( AnalysisPane, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_CENTRE|wxTE_READONLY );
	FieldOfViewDiameter->SetToolTip( wxT("The field of view diameter is used in all calculations involving area and distance. To set it manually, use the custom microscope option above.") );
	
	sbSizer81->Add( FieldOfViewDiameter, 1, wxALIGN_CENTER_HORIZONTAL|wxALL|wxEXPAND, 5 );
	
	m_staticText121 = new wxStaticText( AnalysisPane, wxID_ANY, wxT("mm"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText121->Wrap( -1 );
	sbSizer81->Add( m_staticText121, 0, wxALL, 5 );
	
	AnalysisSetupSizer->Add( sbSizer81, 0, wxEXPAND, 5 );
	
	
	AnalysisSetupSizer->Add( 0, 10, 0, wxEXPAND, 5 );
	
	wxStaticBoxSizer* sbSizer9;
	sbSizer9 = new wxStaticBoxSizer( new wxStaticBox( AnalysisPane, wxID_ANY, wxT("Analysis Type:") ), wxHORIZONTAL );
	
	wxString ChosenAnalysisTypeChoices[] = { wxT("Body Size"), wxT("Long Term Habituation"), wxT("Short Term Habituation") };
	int ChosenAnalysisTypeNChoices = sizeof( ChosenAnalysisTypeChoices ) / sizeof( wxString );
	ChosenAnalysisType = new wxChoice( AnalysisPane, wxID_ANY, wxDefaultPosition, wxDefaultSize, ChosenAnalysisTypeNChoices, ChosenAnalysisTypeChoices, 0 );
	ChosenAnalysisType->SetSelection( 0 );
	ChosenAnalysisType->SetToolTip( wxT("Select the type of analysis you wish to run on your video.") );
	
	sbSizer9->Add( ChosenAnalysisType, 1, wxALL|wxEXPAND, 5 );
	
	AccumulateCheckBox = new wxCheckBox( AnalysisPane, wxID_ANY, wxT("Accumulate results"), wxDefaultPosition, wxDefaultSize, 0 );
	
	AccumulateCheckBox->SetToolTip( wxT("When this is enabled, the results of this analysis will not erase the previous results listed. Instead, they will be appended to the previous.") );
	
	sbSizer9->Add( AccumulateCheckBox, 0, wxALL, 5 );
	
	AnalysisSetupSizer->Add( sbSizer9, 0, wxEXPAND, 5 );
	
	
	AnalysisSetupSizer->Add( 0, 10, 0, wxEXPAND, 5 );
	
	AnalysisGauge = new wxGauge( AnalysisPane, wxID_ANY, 100, wxDefaultPosition, wxDefaultSize, wxGA_HORIZONTAL|wxGA_SMOOTH );
	AnalysisGauge->SetToolTip( wxT("This shows the current progress of the analysis. If the codec used on your system cannot report the total video length, then a non-progressive animation will be displayed.") );
	AnalysisGauge->SetMinSize( wxSize( -1,25 ) );
	
	AnalysisSetupSizer->Add( AnalysisGauge, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxEXPAND, 5 );
	
	bSizer7->Add( AnalysisSetupSizer, 1, wxALL|wxEXPAND, 5 );
	
	wxStaticBoxSizer* StatusSizer;
	StatusSizer = new wxStaticBoxSizer( new wxStaticBox( AnalysisPane, wxID_ANY, wxT("Status:") ), wxVERTICAL );
	
	wxBoxSizer* StatusSizer2;
	StatusSizer2 = new wxBoxSizer( wxVERTICAL );
	
	AnalysisStatusList = new wxListBox( AnalysisPane, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, wxLB_NEEDED_SB|wxLB_SINGLE );
	AnalysisStatusList->Append( wxT("Select an analysis type, then hit begin...") );
	StatusSizer2->Add( AnalysisStatusList, 1, wxALL|wxEXPAND, 5 );
	
	wxFlexGridSizer* StatusFlexSizer;
	StatusFlexSizer = new wxFlexGridSizer( 3, 4, 0, 10 );
	StatusFlexSizer->SetFlexibleDirection( wxBOTH );
	StatusFlexSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText12 = new wxStaticText( AnalysisPane, wxID_ANY, wxT("Frame:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText12->Wrap( -1 );
	StatusFlexSizer->Add( m_staticText12, 0, wxALL, 5 );
	
	AnalysisCurrentFrameStatus = new wxTextCtrl( AnalysisPane, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_CENTRE|wxTE_READONLY );
	AnalysisCurrentFrameStatus->SetToolTip( wxT("This shows the index of the current frame being processed. If the total size is also know, Slither will display this as well. Note that some multimedia backends on certain operating systems may not be able to provide the latter information (possibly buggy codecs).") );
	
	StatusFlexSizer->Add( AnalysisCurrentFrameStatus, 1, wxALL, 5 );
	
	BeginAnalysisButton = new wxButton( AnalysisPane, wxID_ANY, wxT("Begin"), wxDefaultPosition, wxDefaultSize, 0 );
	StatusFlexSizer->Add( BeginAnalysisButton, 0, wxALL, 5 );
	
	
	StatusFlexSizer->Add( 10, 0, 1, wxEXPAND, 5 );
	
	m_staticText13 = new wxStaticText( AnalysisPane, wxID_ANY, wxT("Rate:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText13->Wrap( -1 );
	StatusFlexSizer->Add( m_staticText13, 0, wxALL, 5 );
	
	AnalysisRateStatus = new wxTextCtrl( AnalysisPane, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_CENTRE|wxTE_READONLY );
	AnalysisRateStatus->SetToolTip( wxT("Displays the number of frames that are analyzed per second on average.") );
	
	StatusFlexSizer->Add( AnalysisRateStatus, 1, wxALL, 5 );
	
	CancelAnalysisButton = new wxButton( AnalysisPane, wxID_ANY, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	CancelAnalysisButton->Enable( false );
	
	StatusFlexSizer->Add( CancelAnalysisButton, 0, wxALL, 5 );
	
	
	StatusFlexSizer->Add( 10, 0, 1, wxEXPAND, 5 );
	
	m_staticText14 = new wxStaticText( AnalysisPane, wxID_ANY, wxT("Tracking:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText14->Wrap( -1 );
	StatusFlexSizer->Add( m_staticText14, 0, wxALL, 5 );
	
	AnalysisWormsTrackingStatus = new wxTextCtrl( AnalysisPane, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_CENTRE|wxTE_READONLY );
	AnalysisWormsTrackingStatus->SetToolTip( wxT("The number of worms Slither thinks it has found.") );
	
	StatusFlexSizer->Add( AnalysisWormsTrackingStatus, 1, wxALL, 5 );
	
	StatusSizer2->Add( StatusFlexSizer, 0, wxEXPAND, 5 );
	
	StatusSizer->Add( StatusSizer2, 1, wxEXPAND, 5 );
	
	bSizer7->Add( StatusSizer, 1, wxALL|wxEXPAND, 5 );
	
	AnalysisPaneSizer->Add( bSizer7, 0, wxEXPAND|wxFIXED_MINSIZE, 5 );
	
	wxBoxSizer* bSizer8;
	bSizer8 = new wxBoxSizer( wxHORIZONTAL );
	
	wxStaticBoxSizer* AnalysisResultsSizer;
	AnalysisResultsSizer = new wxStaticBoxSizer( new wxStaticBox( AnalysisPane, wxID_ANY, wxT("Results:") ), wxHORIZONTAL );
	
	AnalysisGrid = new wxGrid( AnalysisPane, wxID_ANY, wxDefaultPosition, wxSize( -1,-1 ), 0 );
	
	// Grid
	AnalysisGrid->CreateGrid( 5, 5 );
	AnalysisGrid->EnableEditing( true );
	AnalysisGrid->EnableGridLines( true );
	AnalysisGrid->EnableDragGridSize( true );
	AnalysisGrid->SetMargins( 0, 0 );
	
	// Columns
	AnalysisGrid->EnableDragColMove( false );
	AnalysisGrid->EnableDragColSize( true );
	AnalysisGrid->SetColLabelSize( 30 );
	AnalysisGrid->SetColLabelAlignment( wxALIGN_CENTRE, wxALIGN_CENTRE );
	
	// Rows
	AnalysisGrid->EnableDragRowSize( true );
	AnalysisGrid->SetRowLabelSize( 80 );
	AnalysisGrid->SetRowLabelAlignment( wxALIGN_CENTRE, wxALIGN_CENTRE );
	
	// Label Appearance
	
	// Cell Defaults
	AnalysisGrid->SetDefaultCellAlignment( wxALIGN_CENTRE, wxALIGN_CENTRE );
	AnalysisResultsSizer->Add( AnalysisGrid, 1, wxALL|wxEXPAND, 5 );
	
	bSizer8->Add( AnalysisResultsSizer, 2, wxALL|wxEXPAND, 5 );
	
	wxStaticBoxSizer* sbSizer16;
	sbSizer16 = new wxStaticBoxSizer( new wxStaticBox( AnalysisPane, wxID_ANY, wxT("Artificial Intelligence:") ), wxVERTICAL );
	
	AISettingsScrolledWindow = new wxScrolledWindow( AnalysisPane, wxID_ANY, wxDefaultPosition, wxSize( -1,-1 ), wxHSCROLL|wxVSCROLL );
	AISettingsScrolledWindow->SetScrollRate( 5, 5 );
	wxBoxSizer* bSizer9;
	bSizer9 = new wxBoxSizer( wxVERTICAL );
	
	wxStaticBoxSizer* sbSizer17;
	sbSizer17 = new wxStaticBoxSizer( new wxStaticBox( AISettingsScrolledWindow, wxID_ANY, wxT("Image Threshold:") ), wxVERTICAL );
	
	wxBoxSizer* bSizer10;
	bSizer10 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText131 = new wxStaticText( AISettingsScrolledWindow, wxID_ANY, wxT("Threshold:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText131->Wrap( -1 );
	bSizer10->Add( m_staticText131, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5 );
	
	ThresholdSpinner = new wxSpinCtrl( AISettingsScrolledWindow, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 255, 150 );
	ThresholdSpinner->SetToolTip( wxT("During image processing, fixed-level thresholding is applied to each pixel in the 8-bit image representation (grayscale). If a pixel is greater than the threshold, then the maximum value is applied. e.g.\n\ndst(x,y) = max_value, if src(x,y)>threshold\n           0, otherwise\n") );
	
	bSizer10->Add( ThresholdSpinner, 0, wxALL, 5 );
	
	sbSizer17->Add( bSizer10, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	wxBoxSizer* bSizer11;
	bSizer11 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText141 = new wxStaticText( AISettingsScrolledWindow, wxID_ANY, wxT("Max Value:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText141->Wrap( -1 );
	bSizer11->Add( m_staticText141, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	MaxThresholdValueSpinner = new wxSpinCtrl( AISettingsScrolledWindow, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 255, 255 );
	MaxThresholdValueSpinner->SetToolTip( wxT("During image processing, fixed-level thresholding is applied to each pixel in the 8-bit image representation (grayscale). If a pixel is greater than the threshold, then the maximum value is applied. e.g.\n\ndst(x,y) = max_value, if src(x,y)>threshold\n           0, otherwise\n") );
	
	bSizer11->Add( MaxThresholdValueSpinner, 0, wxALL, 5 );
	
	sbSizer17->Add( bSizer11, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	bSizer9->Add( sbSizer17, 0, wxEXPAND, 5 );
	
	
	bSizer9->Add( 0, 5, 0, 0, 5 );
	
	wxStaticBoxSizer* sbSizer18;
	sbSizer18 = new wxStaticBoxSizer( new wxStaticBox( AISettingsScrolledWindow, wxID_ANY, wxT("Candidate Size:") ), wxVERTICAL );
	
	wxBoxSizer* bSizer101;
	bSizer101 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText1311 = new wxStaticText( AISettingsScrolledWindow, wxID_ANY, wxT("Min:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1311->Wrap( -1 );
	bSizer101->Add( m_staticText1311, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5 );
	
	MinimumCandidateSizeSpinner = new wxSpinCtrl( AISettingsScrolledWindow, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 255, 50 );
	MinimumCandidateSizeSpinner->SetToolTip( wxT("The minimum size of a contour's area necessary to be considered a nematode. e.g. A value of 50 is the same as 0.050 mm².") );
	
	bSizer101->Add( MinimumCandidateSizeSpinner, 0, wxALL, 5 );
	
	m_staticText13111 = new wxStaticText( AISettingsScrolledWindow, wxID_ANY, wxT("x 10^-3 mm²"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText13111->Wrap( -1 );
	bSizer101->Add( m_staticText13111, 0, wxALL, 5 );
	
	sbSizer18->Add( bSizer101, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	wxBoxSizer* bSizer111;
	bSizer111 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText1411 = new wxStaticText( AISettingsScrolledWindow, wxID_ANY, wxT("Max:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1411->Wrap( -1 );
	bSizer111->Add( m_staticText1411, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	MaximumCandidateSizeSpinner = new wxSpinCtrl( AISettingsScrolledWindow, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 255, 120 );
	MaximumCandidateSizeSpinner->SetToolTip( wxT("The maximum size of a contour's area necessary to be considered a nematode. e.g. A value of 120 is the same as 0.120 mm².") );
	
	bSizer111->Add( MaximumCandidateSizeSpinner, 0, wxALL, 5 );
	
	m_staticText131111 = new wxStaticText( AISettingsScrolledWindow, wxID_ANY, wxT("x 10^-3 mm²"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText131111->Wrap( -1 );
	bSizer111->Add( m_staticText131111, 0, wxALL, 5 );
	
	sbSizer18->Add( bSizer111, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	bSizer9->Add( sbSizer18, 0, wxEXPAND, 5 );
	
	
	bSizer9->Add( 0, 5, 0, 0, 5 );
	
	wxStaticBoxSizer* sbSizer14;
	sbSizer14 = new wxStaticBoxSizer( new wxStaticBox( AISettingsScrolledWindow, wxID_ANY, wxT("Inlet Detection:") ), wxVERTICAL );
	
	InletDetectionCheckBox = new wxCheckBox( AISettingsScrolledWindow, wxID_ANY, wxT("Enabled"), wxDefaultPosition, wxDefaultSize, 0 );
	InletDetectionCheckBox->SetValue(true);
	
	sbSizer14->Add( InletDetectionCheckBox, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	wxBoxSizer* bSizer18;
	bSizer18 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText19 = new wxStaticText( AISettingsScrolledWindow, wxID_ANY, wxT("Morphology Size:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText19->Wrap( -1 );
	bSizer18->Add( m_staticText19, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	InletCorrectionSpinner = new wxSpinCtrl( AISettingsScrolledWindow, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 2, 20, 5 );
	InletCorrectionSpinner->SetToolTip( wxT("When enabled, helps to smooth out the contour surrounding the nematode with the goal of removing sharp contour inlets. These can lead to inaccurate results. The method works by applying the morphological composite operations of erosion followed by dilation. The result forms the result of the higher order operation of \"opening\" the image. When you erode, objects tend to shrink. When dilating, objects tend to grow.\n\nThe morphology size is the number of pixels that form the width / height of the box used to apply the opening of the image. Play with this to get your automatic inlet correction just right.") );
	
	bSizer18->Add( InletCorrectionSpinner, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	sbSizer14->Add( bSizer18, 0, wxALIGN_CENTER_HORIZONTAL|wxEXPAND, 5 );
	
	bSizer9->Add( sbSizer14, 0, wxEXPAND, 5 );
	
	ResetAIToDefaultsButton = new wxButton( AISettingsScrolledWindow, wxID_ANY, wxT("Reset to Defaults"), wxDefaultPosition, wxDefaultSize, 0 );
	ResetAIToDefaultsButton->SetToolTip( wxT("Reset the artificial intelligence settings to the default values.") );
	
	bSizer9->Add( ResetAIToDefaultsButton, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	AISettingsScrolledWindow->SetSizer( bSizer9 );
	AISettingsScrolledWindow->Layout();
	bSizer9->Fit( AISettingsScrolledWindow );
	sbSizer16->Add( AISettingsScrolledWindow, 1, wxALL|wxEXPAND, 5 );
	
	bSizer8->Add( sbSizer16, 1, wxEXPAND, 5 );
	
	AnalysisPaneSizer->Add( bSizer8, 1, wxEXPAND, 5 );
	
	AnalysisPane->SetSizer( AnalysisPaneSizer );
	AnalysisPane->Layout();
	AnalysisPaneSizer->Fit( AnalysisPane );
	MainNotebook->AddPage( AnalysisPane, wxT("Analysis"), true );
	
	EverythingSizer->Add( MainNotebook, 1, wxALL|wxEXPAND, 0 );
	
	this->SetSizer( EverythingSizer );
	this->Layout();
	EverythingSizer->Fit( this );
	MainFrame_Base_statusbar = this->CreateStatusBar( 1, wxST_SIZEGRIP, wxID_ANY );
	
	this->Centre( wxBOTH );
	
	// Connect Events
	this->Connect( wxEVT_SIZE, wxSizeEventHandler( MainFrame_Base::OnSize ) );
	this->Connect( NewMenuItem->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame_Base::OnNew ) );
	this->Connect( OpenMenuItem->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame_Base::OnOpen ) );
	this->Connect( ImportMediaMenuItem->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame_Base::OnImportMedia ) );
	this->Connect( SaveMenuItem->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame_Base::OnSave ) );
	this->Connect( SaveAsMenuItem->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame_Base::OnSaveAs ) );
	this->Connect( RevertMenuItem->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame_Base::OnRevert ) );
	this->Connect( CloseMenuItem->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame_Base::OnClose ) );
	this->Connect( QuitMenuItem->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame_Base::OnQuit ) );
	this->Connect( FullScreenMenuItem->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame_Base::OnFullScreen ) );
	this->Connect( ShowImageAnalysisMenuItem->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame_Base::OnToggleImageAnalysisWindow ) );
	this->Connect( AboutMenuItem->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame_Base::OnAbout ) );
	this->Connect( wxID_NEW, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame_Base::OnNew ));
	this->Connect( wxID_OPEN, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame_Base::OnOpen ));
	this->Connect( wxID_SAVE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame_Base::OnSave ));
	this->Connect( wxID_SAVEAS, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame_Base::OnSaveAs ));
	this->Connect( ID_CAPTURE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame_Base::OnCapture ));
	this->Connect( wxID_ABOUT, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame_Base::OnAbout ));
	MainNotebook->Connect( wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGING, wxNotebookEventHandler( MainFrame_Base::OnPageChanging ), NULL, this );
	ExperimentTitle->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( MainFrame_Base::OnExperimentChange ), NULL, this );
	EmbeddedMedia->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( MainFrame_Base::OnTitleEdit ), NULL, this );
	TotalSize->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( MainFrame_Base::OnTitleEdit ), NULL, this );
	ExperimentNotes->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( MainFrame_Base::OnExperimentChange ), NULL, this );
	ExtractFrameButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainFrame_Base::OnExtractFrame ), NULL, this );
	MediaGrid->Connect( wxEVT_GRID_CELL_CHANGE, wxGridEventHandler( MainFrame_Base::OnExperimentChangeCell ), NULL, this );
	MediaGrid->Connect( wxEVT_GRID_CELL_LEFT_CLICK, wxGridEventHandler( MainFrame_Base::OnMediaCellLeftClick ), NULL, this );
	MediaGrid->Connect( wxEVT_GRID_CELL_LEFT_DCLICK, wxGridEventHandler( MainFrame_Base::OnMediaCellDoubleLeftClick ), NULL, this );
	MediaGrid->Connect( wxEVT_GRID_CELL_RIGHT_CLICK, wxGridEventHandler( MainFrame_Base::OnMediaCellRightClick ), NULL, this );
	ChosenMicroscopeName->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( MainFrame_Base::OnChooseMicroscopeName ), NULL, this );
	ChosenMicroscopeTotalZoom->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( MainFrame_Base::OnChooseMicroscopeTotalZoom ), NULL, this );
	FieldOfViewDiameter->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( MainFrame_Base::OnChooseFieldOfViewDiameter ), NULL, this );
	ChosenAnalysisType->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( MainFrame_Base::OnChooseAnalysisType ), NULL, this );
	BeginAnalysisButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainFrame_Base::OnBeginAnalysis ), NULL, this );
	CancelAnalysisButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainFrame_Base::OnCancelAnalysis ), NULL, this );
	AnalysisGrid->Connect( wxEVT_GRID_CELL_RIGHT_CLICK, wxGridEventHandler( MainFrame_Base::OnAnalysisCellRightClick ), NULL, this );
	ResetAIToDefaultsButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainFrame_Base::OnResetAIToDefaults ), NULL, this );
}

MainFrame_Base::~MainFrame_Base()
{
	// Disconnect Events
	this->Disconnect( wxEVT_SIZE, wxSizeEventHandler( MainFrame_Base::OnSize ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame_Base::OnNew ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame_Base::OnOpen ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame_Base::OnImportMedia ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame_Base::OnSave ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame_Base::OnSaveAs ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame_Base::OnRevert ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame_Base::OnClose ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame_Base::OnQuit ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame_Base::OnFullScreen ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame_Base::OnToggleImageAnalysisWindow ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame_Base::OnAbout ) );
	this->Disconnect( wxID_NEW, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame_Base::OnNew ));
	this->Disconnect( wxID_OPEN, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame_Base::OnOpen ));
	this->Disconnect( wxID_SAVE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame_Base::OnSave ));
	this->Disconnect( wxID_SAVEAS, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame_Base::OnSaveAs ));
	this->Disconnect( ID_CAPTURE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame_Base::OnCapture ));
	this->Disconnect( wxID_ABOUT, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame_Base::OnAbout ));
	MainNotebook->Disconnect( wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGING, wxNotebookEventHandler( MainFrame_Base::OnPageChanging ), NULL, this );
	ExperimentTitle->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( MainFrame_Base::OnExperimentChange ), NULL, this );
	EmbeddedMedia->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( MainFrame_Base::OnTitleEdit ), NULL, this );
	TotalSize->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( MainFrame_Base::OnTitleEdit ), NULL, this );
	ExperimentNotes->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( MainFrame_Base::OnExperimentChange ), NULL, this );
	ExtractFrameButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainFrame_Base::OnExtractFrame ), NULL, this );
	MediaGrid->Disconnect( wxEVT_GRID_CELL_CHANGE, wxGridEventHandler( MainFrame_Base::OnExperimentChangeCell ), NULL, this );
	MediaGrid->Disconnect( wxEVT_GRID_CELL_LEFT_CLICK, wxGridEventHandler( MainFrame_Base::OnMediaCellLeftClick ), NULL, this );
	MediaGrid->Disconnect( wxEVT_GRID_CELL_LEFT_DCLICK, wxGridEventHandler( MainFrame_Base::OnMediaCellDoubleLeftClick ), NULL, this );
	MediaGrid->Disconnect( wxEVT_GRID_CELL_RIGHT_CLICK, wxGridEventHandler( MainFrame_Base::OnMediaCellRightClick ), NULL, this );
	ChosenMicroscopeName->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( MainFrame_Base::OnChooseMicroscopeName ), NULL, this );
	ChosenMicroscopeTotalZoom->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( MainFrame_Base::OnChooseMicroscopeTotalZoom ), NULL, this );
	FieldOfViewDiameter->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( MainFrame_Base::OnChooseFieldOfViewDiameter ), NULL, this );
	ChosenAnalysisType->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( MainFrame_Base::OnChooseAnalysisType ), NULL, this );
	BeginAnalysisButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainFrame_Base::OnBeginAnalysis ), NULL, this );
	CancelAnalysisButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainFrame_Base::OnCancelAnalysis ), NULL, this );
	AnalysisGrid->Disconnect( wxEVT_GRID_CELL_RIGHT_CLICK, wxGridEventHandler( MainFrame_Base::OnAnalysisCellRightClick ), NULL, this );
	ResetAIToDefaultsButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainFrame_Base::OnResetAIToDefaults ), NULL, this );
}

ImageAnalysisWindow_Base::ImageAnalysisWindow_Base( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	this->SetBackgroundColour( wxColour( 0, 0, 0 ) );
	this->SetToolTip( wxT("The image analysis window...") );
	
	
	// Connect Events
	this->Connect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( ImageAnalysisWindow_Base::OnClose ) );
	this->Connect( wxEVT_PAINT, wxPaintEventHandler( ImageAnalysisWindow_Base::OnPaint ) );
}

ImageAnalysisWindow_Base::~ImageAnalysisWindow_Base()
{
	// Disconnect Events
	this->Disconnect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( ImageAnalysisWindow_Base::OnClose ) );
	this->Disconnect( wxEVT_PAINT, wxPaintEventHandler( ImageAnalysisWindow_Base::OnPaint ) );
}
