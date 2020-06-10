/*
  Name:         SlitherApp.cpp (implementation)
  Author:       Kip Warner (Kip@TheVertigo.com)
  Description:  Every wxWidgets application declares an application class...
*/

// Includes...

    // SlitherApp declaration...    
    #include "SlitherApp.h"

    // We create an instance of MainFrame...
    #include "MainFrame.h"
    
    // We also need to know about loading experiments when invoked from shell...
    #include "Experiment.h"
    
    // Command line parsing...
    #include <wx/cmdline.h>
    
    // For initializing OpenCV...
    // $VR$: 2020/06/10 - updated for OpenCV 4
    //#include <opencv/cv.h>
    #include <opencv2/opencv.hpp>

// Give wxWidgets the means to create a SlitherApp object...
IMPLEMENT_APP(SlitherApp)

// Command line descriptions...
static const wxCmdLineEntryDesc g_CommandLineDescriptions[] =
{
    // Help...
    {   
        wxCMD_LINE_SWITCH, 
        wxT("h"), 
        wxT("help"),
        wxT("displays help on the command line parameters")
    },
    
    // Version...
    {
        wxCMD_LINE_SWITCH, 
        wxT("v"), 
        wxT("version"),
        wxT("print version")
    },

    // Experiment file to open...
    {
        wxCMD_LINE_PARAM,
        NULL, 
        NULL,
        wxT("experiment file"), 
        wxCMD_LINE_VAL_STRING, 
        wxCMD_LINE_PARAM_OPTIONAL 
    },

    // End of descriptions...
    {
        wxCMD_LINE_NONE
    }
};

// Invoked on application startup... (analagous to main or WinMain)
bool SlitherApp::OnInit()
{
    //$VR$: 2020/06/10 - initialise standard path handler
    wxStandardPaths StandardPaths = wxStandardPaths::Get();

    // Enable fatal signal handler...
//  ::wxHandleFatalExceptions(true);

    // Create a command line parser...
    wxCmdLineParser CommandLineParser(g_CommandLineDescriptions, argc, argv);

    // Parsing failed or user requested help...
    if(CommandLineParser.Parse(false) == -1 || 
       CommandLineParser.Found(wxT("h")))
    {
        // Display usage...
        CommandLineParser.Usage();
        return false;
    }

    // Check if the user asked for the version
    if(CommandLineParser.Found(wxT("v")))
    {
        // Format...
        wxString sVersion;
        sVersion.Printf(wxT("%d.%d\nSubversion %s"), 
            VERSION_MAJOR, VERSION_MINOR, VERSION_SVN);

        // Display version...
        printf("%s\n", (char const *) sVersion.mb_str(wxConvUTF8));
        return false;
    }

    // Create configuration object...
    pConfiguration = new wxConfig(wxT("Slither"), wxT("Vertigo"));

    // Create the main application frame...
    pMainFrame = new MainFrame((wxWindow *) NULL);
    
    // Show it...
    pMainFrame->Show(true);

    // Check if experiment was passed on command line by OS or user...
    if(CommandLineParser.GetParamCount() > 0)
    {
        // Extract unnormalized file name...
        wxString sFileName = CommandLineParser.GetParam(0);

        // Win32 passes short form, normalize...
        wxFileName NormalizedFileName(sFileName);
        NormalizedFileName.Normalize(wxPATH_NORM_LONG | wxPATH_NORM_DOTS |
                                     wxPATH_NORM_TILDE | wxPATH_NORM_ABSOLUTE);

        // Store the experiment name...
        sExperimentRequestedFromShell = NormalizedFileName.GetFullPath();

        // Tell the main frame to try opening the experiment now...
        pMainFrame->ProcessCommand(wxID_OPEN);
    }

    // Start the event loop...
    return true;
}

// Fatal exception handler...
void SlitherApp::OnFatalException()
{
    // Display error message...
    wxMessageDialog 
        ErrorMessage(NULL, wxT("I apologize for the inconvenience, but I have"
                               " encountered a fatal error and must be shut"
                               " down."), wxT("Fatal Error"), 
                     wxOK | wxICON_ERROR);
    ErrorMessage.ShowModal();

    // Try to cleanup...

        // Experiment...
        if(pMainFrame->pExperiment)
        {
            // Force it to cleanup its mess in the cache...
            pMainFrame->pExperiment->ClearNeedSave();
            delete pMainFrame->pExperiment;
        }
    
        // Configuration...
        delete pConfiguration;

    // Terminate now...
    return;
}

// Exiting Slither...
int SlitherApp::OnExit()
{
    // Cleanup configuration...
    delete pConfiguration;
    
    // Done...
    return true;
}

// Mac open file request... (Finder doesn't use command line)
#ifdef __WXMAC__
void SlitherApp::MacOpenFile(const wxString &sFileName)
{
    // Store the experiment name...
    sExperimentRequestedFromShell = sFileName;

    // Tell the main frame to try opening the experiment now...
    pMainFrame->ProcessCommand(wxID_OPEN);
}
#endif

