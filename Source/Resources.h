///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Mar 19 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __Resources__
#define __Resources__

#include <wx/string.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/menu.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/toolbar.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/panel.h>
#include <wx/button.h>
#include <wx/grid.h>
#include <wx/bmpbuttn.h>
#include <wx/choice.h>
#include <wx/checkbox.h>
#include <wx/gauge.h>
#include <wx/listbox.h>
#include <wx/spinctrl.h>
#include <wx/scrolwin.h>
#include <wx/notebook.h>
#include <wx/statusbr.h>
#include <wx/frame.h>

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// Class MainFrame_Base
///////////////////////////////////////////////////////////////////////////////
class MainFrame_Base : public wxFrame 
{
	private:
	
	protected:
		enum
		{
			ID_IMPORT_MEDIA = 1000,
			ID_FULLSCREEN,
			ID_TOGGLE_IMAGE_ANALYSIS_WINDOW,
			ID_CAPTURE,
		};
		
		wxMenuBar* MenuBar;
		wxMenu* FileMenu;
		wxMenu* ViewMenu;
		wxMenu* HelpMenu;
		wxToolBar* MainToolBar;
		wxNotebook* MainNotebook;
		wxPanel* DataPane;
		
		wxStaticText* m_staticText72;
		wxTextCtrl* ExperimentTitle;
		wxStaticText* m_staticText92;
		wxTextCtrl* EmbeddedMedia;
		wxStaticText* m_staticText911;
		wxTextCtrl* TotalSize;
		wxStaticText* m_staticText1221;
		wxTextCtrl* ExperimentNotes;
		wxPanel* VideoPreviewPanel;
		wxButton* ExtractFrameButton;
		
		wxGrid* MediaGrid;
		wxPanel* CapturePane;
		wxPanel* CaptureImagePanel;
		wxStaticText* m_staticText11;
		
		wxBitmapButton* RecordButton;
		wxBitmapButton* SaveRecordingButton;
		wxPanel* AnalysisPane;
		
		wxChoice* ChosenMicroscopeName;
		wxChoice* ChosenMicroscopeTotalZoom;
		
		wxTextCtrl* FieldOfViewDiameter;
		wxStaticText* m_staticText121;
		
		wxChoice* ChosenAnalysisType;
		wxCheckBox* AccumulateCheckBox;
		
		wxGauge* AnalysisGauge;
		wxListBox* AnalysisStatusList;
		wxStaticText* m_staticText12;
		wxTextCtrl* AnalysisCurrentFrameStatus;
		wxButton* BeginAnalysisButton;
		
		wxStaticText* m_staticText13;
		wxTextCtrl* AnalysisRateStatus;
		wxButton* CancelAnalysisButton;
		
		wxStaticText* m_staticText14;
		wxTextCtrl* AnalysisWormsTrackingStatus;
		wxGrid* AnalysisGrid;
		wxScrolledWindow* AISettingsScrolledWindow;
		wxStaticText* m_staticText131;
		wxSpinCtrl* ThresholdSpinner;
		wxStaticText* m_staticText141;
		wxSpinCtrl* MaxThresholdValueSpinner;
		
		wxStaticText* m_staticText1311;
		wxSpinCtrl* MinimumCandidateSizeSpinner;
		wxStaticText* m_staticText13111;
		wxStaticText* m_staticText1411;
		wxSpinCtrl* MaximumCandidateSizeSpinner;
		wxStaticText* m_staticText131111;
		
		wxCheckBox* InletDetectionCheckBox;
		wxStaticText* m_staticText19;
		wxSpinCtrl* InletCorrectionSpinner;
		wxButton* ResetAIToDefaultsButton;
		wxStatusBar* MainFrame_Base_statusbar;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnSize( wxSizeEvent& event ){ event.Skip(); }
		virtual void OnNew( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnOpen( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnImportMedia( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnSave( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnSaveAs( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnRevert( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnClose( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnQuit( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnFullScreen( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnToggleImageAnalysisWindow( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnAbout( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnCapture( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnPageChanging( wxNotebookEvent& event ){ event.Skip(); }
		virtual void OnExperimentChange( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnTitleEdit( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnExtractFrame( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnExperimentChangeCell( wxGridEvent& event ){ event.Skip(); }
		virtual void OnMediaCellLeftClick( wxGridEvent& event ){ event.Skip(); }
		virtual void OnMediaCellDoubleLeftClick( wxGridEvent& event ){ event.Skip(); }
		virtual void OnMediaCellRightClick( wxGridEvent& event ){ event.Skip(); }
		virtual void OnChooseMicroscopeName( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnChooseMicroscopeTotalZoom( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnChooseFieldOfViewDiameter( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnChooseAnalysisType( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnBeginAnalysis( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnCancelAnalysis( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnAnalysisCellRightClick( wxGridEvent& event ){ event.Skip(); }
		virtual void OnResetAIToDefaults( wxCommandEvent& event ){ event.Skip(); }
		
	
	public:
		MainFrame_Base( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Slither"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxDEFAULT_FRAME_STYLE );
		~MainFrame_Base();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class ImageAnalysisWindow_Base
///////////////////////////////////////////////////////////////////////////////
class ImageAnalysisWindow_Base : public wxFrame 
{
	private:
	
	protected:
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnClose( wxCloseEvent& event ){ event.Skip(); }
		virtual void OnPaint( wxPaintEvent& event ){ event.Skip(); }
		
	
	public:
		ImageAnalysisWindow_Base( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Image Analysis"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 320,240 ), long style = wxCAPTION|wxCLOSE_BOX|wxFRAME_FLOAT_ON_PARENT|wxFULL_REPAINT_ON_RESIZE|wxTAB_TRAVERSAL );
		~ImageAnalysisWindow_Base();
	
};

#endif //__Resources__
