/*
  Name:         Experiment.h (definition)
  Author:       Kip Warner (Kip@TheVertigo.com)
  Description:  Experiment class...
*/

// Multiple include protection...
#ifndef _EXPERIMENT_H_
#define _EXPERIMENT_H_

// Includes...

    // For updating UI...
    #include "MainFrame.h"
    
    // wxWidgets...
    #include <wx/dir.h>
    #include <wx/progdlg.h>

// Forward declarations...
class MainFrame;

// Experiment class...
class Experiment
{
    // Public stuff...
    public:

        // Methods...

            // Constructor...
            Experiment(MainFrame *_pMainFrame);

            // Clear need save flag...
            void ClearNeedSave();

            // Get new unique cache file name...
            wxString GetUniqueCacheFileName() const;

            // Get the path to experiment cache...
            wxString &GetCachePath();

            // Get the full path, file name, and extension to file on disk...
            wxString &GetPath();

            // Has this file ever been saved?
            bool IsEverBeenSaved() const;

            // Does the experiment need to be saved?
            bool IsNeedSave() const;

            // Experiment loaded ok?
            bool IsLoadOk() const;

            // Load experiment...
            bool Load(const wxString _sPath);

            // Save experiment...
            bool Save();
            
            // Save the experiment under a new file name...
            bool SaveAs(const wxString _sPath);

            // Flag experiment as needing a save...
            void TriggerNeedSave();

            // Deconstructor...
           ~Experiment();

    // Protected stuff...
    protected:
    
        // Methods...    
        
            // Get a temporary directory...
            wxString CreateTempDirectory() const;

            // Enable or disable UI, and optionally reset GUI...
            void EnableUI(bool bEnable, bool bReset = false);

            // Recursively remove directory and all of its contents... (be careful)
            bool RecursivelyRemoveDirectory(wxString sPath);

        // Attributes...
        
            // Main frame...
            MainFrame  *pMainFrame;
            
            // Path to file on disk...
            wxString    sPath;

            // The location of the unpacked experiment cached on disk...
            wxString    sCachePath;

            // Loaded ok...
            bool        bLoadOk;
            
            // Needs save...
            bool        bNeedSave;

        // Helper classes...
            
            // Recursive scan of directories and their contents...
            class wxDirectoryScanner : public wxDirTraverser 
            { 
                // Public stuff...
                public:

                    // Constructor takes output location for findings...
                    wxDirectoryScanner(wxArrayString &_FilesFoundArray,
                                       wxArrayString &_DirectoriesFoundArray)
                        : FilesFoundArray(_FilesFoundArray),
                          DirectoriesFoundArray(_DirectoriesFoundArray)
                    { }
                    
                    // File found callback...
                    virtual wxDirTraverseResult OnFile(const wxString& sFileName) 
                    { 
                        // Add it to the list...
                        FilesFoundArray.Add(sFileName);
                        
                        // Carry on...
                        return wxDIR_CONTINUE; 
                    }

                    // Directory found callback...
                    virtual wxDirTraverseResult OnDir(const wxString& sDirectory) 
                    {
                        // Add it to the list...
                        DirectoriesFoundArray.Add(sDirectory);
                        
                        // Carry on...
                        return wxDIR_CONTINUE;
                    }


                // Protected stuff...
                protected:
                
                    // List of files and directories...
                    wxArrayString  &FilesFoundArray;
                    wxArrayString  &DirectoriesFoundArray;
            };
};

#endif

