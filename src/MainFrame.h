/*
  Name:         MainFrame.h (definition)
  Author:       Kip Warner (Kip@TheVertigo.com)
  Description:  The main window...
*/

// Multiple include protection...
#ifndef _MAINFRAME_H_
#define _MAINFRAME_H_

// Includes...
    
    // Application class...
    #include "SlitherApp.h"
    
    // Capture thread...
    #include "CaptureThread.h"
    
    // Analysis thread...
    #include "AnalysisThread.h"
    
    // OpenCV...
    #include <opencv/cv.h>

    // STL stuff...
    #include <list>
    #include <map>
    #include <vector>

// Forward declarations...
class Experiment;
class ImageAnalysisWindow;

// Helper classes...

    // Microscope class...
    class Microscope
    {
        // Protected stuff...
        protected:
        
            // Microscope name...
            wxString  sName;
            
            // Magnification -> field of view diameter (e.g. 40x -> 0.123mm)
            std::map<int, float>    FieldOfViewDiameterTable;
            
            // Supported magnifications...
            std::vector<int>        SupportedMagnifications;
        
        // Public stuff...
        public:
        
            // Constructor...
            Microscope(wxString _sName)
                       : sName(_sName) { }
                       
            // Add a field of view...
            void AddDiameter(int nMagnification, float fFieldOfView)
            {
                // Add to hash table...
                FieldOfViewDiameterTable[nMagnification]
                    = fFieldOfView;

                // Remember this magnification as supported...
                SupportedMagnifications.push_back(nMagnification);
            }

            // Get the nth total zoom factor capability...
            int GetZoom(unsigned int unCapabilityIndex) const
            {
                // Return it...
                return SupportedMagnifications.at(unCapabilityIndex);
            }

            // Get total number of zooms...
            unsigned int GetNumberOfZooms() const
            {
                // Return it...
                return FieldOfViewDiameterTable.size();
            }
            
            // Get name...
            wxString const &GetName() const
            {
                // Return it...
                return sName;
            }
            
            // Get a diameter for a given total magnification...
            float GetDiameter(int nMagnification)
            {
                // Lookup...
                return FieldOfViewDiameterTable[nMagnification];
            }
    };

// Main frame class...
class MainFrame : public MainFrame_Base
{
    // Friends...
    friend class SlitherApp;
    friend class AnalysisThread;
    friend class CaptureThread;
    friend class Experiment;
    friend class MediaGridDropTarget;
    friend class ImageAnalysisWindow;

    // Public stuff...
    public:

        // Constructor...
        MainFrame(wxWindow *Parent);

        // Compare two integers. Used for sorting rows in the media grid...
        static int wxCMPFUNC_CONV 
            CompareIntegers(int *pnFirst, int *pnSecond);

        // Get the analysis results formatted into a string...
        wxString const GetAnalysisResults();

        // Get the total size of all media in the media grid...
        wxULongLong GetTotalMediaSize();

        // Is experiment contain a media by a specific name?
        bool IsExperimentContainMedia(wxString sName);

        // Is experiment contain a media by a specific name, except a row?
        bool IsExperimentContainMediaExceptRow(wxString sName, int nSkipRow);

        // Is there an experiment loaded?
        bool IsExperimentLoaded() const;
        
        // System handlers...
        void OnSystemClose(wxCloseEvent &Event);
        
        // Menu and button handlers...
        void OnNew(wxCommandEvent &Event);
        void OnOpen(wxCommandEvent &Event);
        void OnImportMedia(wxCommandEvent &Event);
        void OnSave(wxCommandEvent &Event);
        void OnSaveAs(wxCommandEvent &Event);
        void OnRevert(wxCommandEvent &Event);
        void OnClose(wxCommandEvent &Event);
        void OnQuit(wxCommandEvent &Event);
        void OnFullScreen(wxCommandEvent &Event);
        void OnAbout(wxCommandEvent &Event);

        // Notebook event handlers...
        void OnPageChanging(wxNotebookEvent &Event);

        // Media grid popup menu events...
        void OnPlay(wxCommandEvent &Event);
        void OnStop(wxCommandEvent &Event);
        void OnRemove(wxCommandEvent &Event);
        void OnRename(wxCommandEvent &Event);

        // Extract frame button...
        void OnExtractFrame(wxCommandEvent &Event);

        // Experiment has changed handler...
        void OnExperimentChange(wxCommandEvent &Event);
        void OnExperimentChangeCell(wxGridEvent &Event);

        // Frame moving or resizing...
        void OnMove(wxMoveEvent &Event);
        void OnSize(wxSizeEvent &Event);

        // Media grid event handlers...
        void OnMediaCellDoubleLeftClick(wxGridEvent &Event);
        void OnMediaCellLeftClick(wxGridEvent &Event);
        void OnMediaCellRightClick(wxGridEvent &Event);

        // Media media player event handlers...
        void OnMediaLoaded(wxMediaEvent& Event);
        void OnMediaStop(wxMediaEvent& Event);
        void OnMediaPlay(wxMediaEvent& Event);

        // Capture event handlers...
        void OnCapture(wxCommandEvent &Event); /* toggle button */
        void OnCaptureFrameReadyTimer(wxTimerEvent &Event);

        // Analysis event handlers...
        void OnChooseMicroscopeName(wxCommandEvent &Event);
        void OnChooseMicroscopeTotalZoom(wxCommandEvent &Event);
        void OnChooseFieldOfViewDiameter(wxCommandEvent &Event);
        void OnChooseAnalysisType(wxCommandEvent &Event);
        void OnAnalyze(wxCommandEvent &Event);
        void OnBeginAnalysis(wxCommandEvent &Event);
        void OnAnalysisCellRightClick(wxGridEvent &Event);
        void OnAnalysisCopyClipboard(wxCommandEvent &Event);
        void OnAnalysisSaveToDisk(wxCommandEvent &Event);
        void OnAnalysisFrameReadyTimer(wxTimerEvent &Event);
        void OnCancelAnalysis(wxCommandEvent &Event);
        void OnEndAnalysis(wxCommandEvent &Event);

        // Image analysis window has been toggled...
        void OnToggleImageAnalysisWindow(wxCommandEvent &Event);

        // Show the tip window...
        void ShowTip();

        // This class handles wxWidgets events...
        DECLARE_EVENT_TABLE()

        // Notebook panes...
        enum
        {
            DATA_PANE = 0,
            CAPTURE_PANE,
            ANALYSIS_PANE
        };

        // Media grid...
        
            // The columns...
            enum
            {
                TITLE = 0,
                DATE,
                TIME,
                TECHNICIAN,
                LENGTH,
                SIZE,
                NOTES
            };
        
            // The number of columns...
            #define MEDIA_GRID_COLUMNS 7

        // Event IDs...
        enum
        {
            ID_ANALYZE = wxID_HIGHEST + 1,
            ID_CHECK_FOR_UPDATE,
            ID_CHECK_FOR_UPDATE_DONE,
            ID_VIDEO_PLAYER,
            ID_PLAY,
            ID_REMOVE,
            ID_RENAME,
            ID_STOP,
            ID_CAPTURE_FRAME_READY,
            ID_ANALYSIS_ENDED,
            ID_ANALYSIS_COPY_CLIPBOARD,
            ID_ANALYSIS_SAVE_TO_DISK
        };
        
        // Timer IDs...
        enum
        {
            TIMER_ANALYSIS  = 100,
            TIMER_CAPTURE
        };
        
        // Analysis types...
        enum
        {
            ANALYSIS_BODY_SIZE                  = 0,
            ANALYSIS_LONG_TERM_HABITUATION,
            ANALYSIS_SHORT_TERM_HABITUATION
        };
        
        // Analysis type columns...
        enum
        {
            ANALYSIS_BODY_SIZE_COLUMN_LENGTH    = 0,
            ANALYSIS_BODY_SIZE_COLUMN_WIDTH,
            ANALYSIS_BODY_SIZE_COLUMN_AREA
        };
        
        // Microscope table...
        std::vector<Microscope> MicroscopeTable;

    // Protected stuff...
    protected:
        
        // The path to the current media in the playback window...
        wxString                sCurrentMediaPath;
        
        // Image analysis window...
        ImageAnalysisWindow    *pImageAnalysisWindow;

        // Experiment...
        Experiment             *pExperiment;

        // Media player...
        wxMediaCtrl            *pMediaPlayer;

        // Capture thread timer...
        wxTimer                 CaptureTimer;

        // Capture frame queue...
        std::list<IplImage *>   CaptureFrameBuffer;
        
        // Analysis thread and timer...
        AnalysisThread         *pAnalysisThread;
        wxTimer                 AnalysisTimer;
        
        // Worm tracker...
        WormTracker             Tracker;
};

#endif

