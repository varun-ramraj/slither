-- Add command line options...
addoption("installer", "Generate a redistributable installer for your platform...")
addoption("addpath", "Add a path to look for libs, includes, and bins, eg. ~/local")

-- Configure project...
project.name = "Slither"
project.path = "./"
project.bindir = "./"

-- Some global variables...
PACKAGE             = "slither"
PACKAGE_NAME        = "Slither"
PACKAGE_VERSION     = "0.0.33a"
PACKAGE_STRING      = "Slither 0.0.33a"

wxBinaryToolsPrefix = ""

-- Check for compatible OS...
if not(OS == "macosx" or OS == "windows" or OS == "linux") then
    error("\nYou appear to be running " .. OS .. ", but Slither doesn't run on that yet.")
end

-- Configure a C/C++ package to use wxWidgets...
function ConfigureForWxWidgets(package)

    -- Configure for DLL...    
    if(package.kind == "dll") then
        tinsert(package.defines, "WXMAKINGDLL")
    end

    -- Configure for Linux...
    if (OS == "linux") then
  
        -- Alert user...
        print("Configuring source for Linux...")

    end
    
    -- Configure for Mac OS X...
    if (OS == "macosx") then
  
        -- Alert user...
        print("Configuring source for Mac OS X...")
        
    end

    -- Configure for Win32...
    if(OS == "windows") then
        
        -- Alert user...
        print("Configuring source for M$ Win32...")

        -- Setup preprocessor defines and linker options for release and debug...
        tinsert(package.defines, { "WIN32", "_WINDOWS", "WINVER=0x400", "_MT", "wxUSE_GUI=1" })
        tinsert(package.links,   { "comctl32", "rpcrt4", "wsock32" })
        tinsert(package.config["Debug"].defines, { "_DEBUG", "__WXDEBUG", "WXDEBUG=1" })
        tinsert(package.config["Release"].defines, "NDEBUG")

        -- Configure for static library...
        tinsert(package.config["Debug"].links, "wxmswd")
        tinsert(package.config["Release"].links, "wxmsw")
    end
end

-- Configure Slither application...
package = newpackage()
package.name            = "slither"

    -- Set package kind...

        -- Win32 should not have a console window...
        if(windows) then
            package.kind = "winexe"
        
        -- Linux and Mac OS X are fine as just exe...
        else
            package.kind = "exe"
        end

    -- Find wxWidgets wxrc and wx-config...
    print("Probing around your system for wxWidgets...")

        -- User requested to add a path, check in there first...
        if(options["addpath"]) then
            
            -- wx-config under the new path...
            if(os.execute(options["addpath"] .. "/bin/./wx-config --version") == 0) then
                
                -- Store new path...
                wxBinaryToolsPrefix = options["addpath"] .. "/bin/./"
                
                -- Alert user...
                print("wx-config detected in " .. options["addpath"] .. "/bin/")
            
            -- Check if it is in any system path at all then...
            elseif(os.execute("wx-config --version") == 0) then
            
                -- No need for full path then, since it is in path...
                wxBinaryToolsPrefix = ""
                
                -- Alert user...
                print("wx-config detected in path...")
            
            -- wx-config not under new path...
            else

                -- Abort and alert user...
                error("\nI cannot seem to find wx-config. Check to make sure it" ..
                      " is in your path. You may need to use --addpath /foo to" ..
                      " find wx-config if it is located at /foo/bin/wx-config.")

            end
            
            
            -- Add paths...
            package.includepaths    = { options["addpath"] .. "/include" }
            package.libpaths        = { options["addpath"] .. "/lib" }
            package.binpaths        = { options["addpath"] .. "/bin" }

            -- Alert user...
            print("Using additional search path for libs, includes, and bins... " ..
                  options["addpath"])

        -- Check for wx-config...
        else

            -- Check if it is in any system path at all then...
            if(os.execute("wx-config --version") == 0) then
            
                -- No need for full path then, since it is in path...
                wxBinaryToolsPrefix = ""
                
                -- Alert user...
                print("wx-config detected in path...")

            -- wx-config not under new path...
            else
            
                -- Alert user...
                error("\nI cannot seem to find wx-config. Check to make sure it" ..
                      " is in your path. You may need to use --addpath /foo to" ..
                      " find wx-config if it is located at /foo/bin/wx-config.")

            end
        end

    -- Build options...
        
        -- Using a Mac OS X, build universal binary...
        if(macosx) then
            
            -- Set options...
            package.buildoptions
                = {"-arch ppc -arch i386",
                   "`" .. wxBinaryToolsPrefix .. "wx-config --cxxflags`"}

        -- Use normal build options...
        else
        
            -- Set options...
            package.buildoptions
                = {"`" .. wxBinaryToolsPrefix .. "wx-config --cxxflags`"}

        end

    -- Build flags...
    package.buildflags      = {"unicode", "extra-warnings", "fatal-warnings"}

    -- Prebuild commands... (hack until xrc is implemented)
    package.prebuildcommands
        = {"-@if test ! -e src/Resources.h ; " ..
           "then " ..
               "printf 'Generating relevant C++ source from XRC for the first time...\\n' ; " ..
               wxBinaryToolsPrefix .. "wxrc src/Resources.xrc -e -c -o src/Resources.cpp ; " ..
               "mv Resources.h src/ ; " ..
           "elif test src/Resources.h -ot src/Resources.xrc ; " ..
           "then " ..
               "printf 'XRC has been updated, regenerating relevant C++ source...\\n' ; " ..
               wxBinaryToolsPrefix .. "wxrc src/Resources.xrc -e -c -o src/Resources.cpp ; " ..
               "mv Resources.h src/ ; " ..
           "else " ..
               "printf 'XRC is still up to date...\\n' ; " ..
           "fi"}

    -- Linker options...
    
        -- Mac OS X needs Quartz for some reason and wx-config bitches about
        --  being unable to find the media lib even though it's built as a 
        --  single monolithic static lib...
        if(macosx) then
            package.linkoptions
                = {"`" .. wxBinaryToolsPrefix .. 
                   "wx-config --libs std,core,base`", 
                   "-framework QuartzCore"}
        else
            package.linkoptions
                = {"`" .. wxBinaryToolsPrefix .. 
                   "wx-config --libs media,std,core,base`"}
        end

    -- Libraries we must link against...
    package.links = {"cxcore", "cv", "highgui", "cvaux"}

    -- Language we wrote package in...
    package.language = "c++"
    
    -- Defines...
    tinsert(package.defines, { "PACKAGE=\\\"" .. PACKAGE .. "\\\"",
                               "PACKAGE_NAME=\\\"" .. PACKAGE_NAME .. "\\\"",
                               "PACKAGE_VERSION=\\\"" .. PACKAGE_VERSION .. "\\\"",
                               "PACKAGE_STRING=\\\"" .. PACKAGE_STRING .. "\\\"" })

    -- Source code...
    package.files = {
        "src/AnalysisThread.cpp",
        "src/src/AnalysisThread.h",
        "src/CaptureThread.cpp",
        "src/CaptureThread.h",
        "src/Experiment.cpp",
        "src/Experiment.h",
        "src/MainFrame.cpp",
        "src/MainFrame.h",
        "src/Resources.cpp",
        "src/Resources.h",
        "src/Resources.rc",
        "src/Resources.xrc",
        "src/SlitherApp.cpp",
        "src/SlitherApp.h",
        "src/VideosGridDropTarget.cpp",
        "src/VideosGridDropTarget.h",
        "src/Worm.cpp",
        "src/Worm.h",
        "src/WormTracker.cpp",
        "src/WormTracker.h"}

-- Configure Slither for wxWidgets and OpenCV...
ConfigureForWxWidgets(package)

-- Generate an installer for their respective platform...
function doinstaller(Command, OutputPath)

    -- Set output location, if there isn't one...
    if(not OutputPath) then
        OutputPath = "./install"
    end
    
    -- Build an Ubuntu package for Linux
    if (OS == "linux") then
  
        -- Make sure slither binary is built... (release has a bunch of compile
        --  time warnings in the wx headers)
        if not(os.execute("make CONFIG=Debug") == 0) then
            error("\nCompilation must finish successfully to generate installer...")
        end

        -- Prepare package...
        print("Preparing to generate Ubuntu package...")
        os.execute("mkdir install/temp")
        os.execute("svn export install/Ubuntu/ install/temp/Ubuntu --non-interactive")
        os.copyfile("./slither", "install/temp/Ubuntu/usr/bin")
        os.execute("strip -S install/temp/Ubuntu/usr/bin/slither")
        os.copyfile("AUTHORS", "install/temp/Ubuntu/usr/share/doc/slither")
        os.copyfile("LICENSE", "install/temp/Ubuntu/usr/share/doc/slither")
        os.copyfile("doc/tips.txt", "install/temp/Ubuntu/usr/share/slither")
        os.copyfile("doc/README", "install/temp/Ubuntu/usr/share/doc/slither")
        os.copyfile("src/resources/slither.png", "install/temp/Ubuntu/usr/share/pixmaps")
        
        -- Generate package...
        print("Generating Ubuntu package...")
        os.execute("dpkg-deb -z9 --build install/temp/Ubuntu install")

        -- Cleanup...
        print("Cleaning up...")
        os.execute("rm -Rf install/temp")

    end
    
    -- Build a Mac OS X application bundle...
    if (OS == "macosx") then
  
        -- The bundle properties XML...
        BundleProperties = 
            "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n" ..
            "<!DOCTYPE plist SYSTEM \"file://localhost/System/Library/DTDs/PropertyList.dtd\">\n" ..
            "<plist version=\"0.9\">\n" ..
            " <dict>\n" ..
            "   <key>CFBundleInfoDictionaryVersion</key>\n" ..
            "       <string>6.0</string>\n" ..
            "	<key>CFBundleIdentifier</key>\n" ..
            "	    <string>com.kip.Slither</string>\n" ..
            "	<key>CFBundleDevelopmentRegion</key>\n" ..
            "	    <string>English</string>\n" ..
            "	<key>CFBundleDocumentTypes</key>\n" ..
	        "	<array>\n" ..
            "	     <dict>\n" ..
			"	     <key>CFBundleTypeExtensions</key>\n" ..
			"	     <array>\n" ..
			"            <string>sex</string>\n" ..
			"	     </array>\n" ..
			"	     <key>CFBundleTypeIconFile</key>\n" ..
			"        	<string>experiment.icns</string>\n" ..
			"        	<key>CFBundleTypeName</key>\n" ..
			"        	<string>Slither Experiment File</string>\n" ..
			"        	<key>CFBundleTypeRole</key>\n" ..
			"        	<string>Editor</string>\n" ..
			"	     </dict>\n" ..
			"	</array>\n" ..
            "	<key>CFBundleExecutable</key>\n" ..
            "	    <string>slither</string>\n" ..
            "	<key>CFBundleIconFile</key>\n" ..
            "	    <string>slither.icns</string>\n" ..
            "	<key>CFBundleName</key>\n" ..
            "	    <string>Slither</string>\n" ..
            "	<key>CFBundlePackageType</key>\n" ..
            "	    <string>APPL</string>\n" ..
            "	<key>CFBundleSignature</key>\n" ..
            "	    <string>????</string>\n" ..
            "	<key>CFBundleVersion</key>\n" ..
            "	    <string>" .. PACKAGE_VERSION .. "</string>\n" ..
            "	<key>CFBundleShortVersionString</key>\n" ..
            "	    <string>" .. PACKAGE_VERSION .. "</string>\n" ..
            "	<key>CFBundleGetInfoString</key>\n" ..
            "	    <string>" .. PACKAGE_STRING .. ", Kip Warner</string>\n" ..
            "	<key>CFBundleLongVersionString</key>\n" ..
            "	    <string>" .. PACKAGE_VERSION .. ", Kip Warner</string>\n" ..
            "	<key>NSHumanReadableCopyright</key>\n" ..
            "	    <string>Released under the GPL.</string>\n" ..
            "	<key>LSRequiresCarbon</key>\n" ..
            "	    <true/>\n" ..
            "	<key>CSResourcesFileMapped</key>\n" ..
            "	    <true/>\n" ..
            " </dict>\n" ..
            "</plist>"

        -- Make sure slither binary is built...
        if not(os.execute("make") == 0) then
            error("\nCompilation must finish successfully to generate installer...")
        end

        -- Create directory structure for bundle...
        os.execute("mkdir -p install/Mac/Slither.app/Contents/MacOS")
        os.execute("mkdir -p install/Mac/Slither.app/Contents/Resources")
        
        -- Grab documentation...
        os.copyfile("LICENSE", "install/Mac/")
        os.copyfile("AUTHORS", "install/Mac/")
        os.copyfile("doc/README", "install/Mac/")

        -- Populate bundle with executable and needed resources...
        print("Generating application bundle...")
        os.copyfile("./slither", "install/Mac/Slither.app/Contents/MacOS")
        print("    Stripping executable to reduce size...")
        os.execute("strip -S install/Mac/Slither.app/Contents/MacOS/slither")
        os.copyfile("doc/tips.txt", "install/Mac/Slither.app/Contents/Resources/")
        os.copyfile("src/resources/experiment.icns", "install/Mac/Slither.app/Contents/Resources/")
        os.copyfile("src/resources/slither.icns", "install/Mac/Slither.app/Contents/Resources/")
        os.execute("echo -n 'APPL????' > install/Mac/Slither.app/Contents/PkgInfo")
        CreateFileFromString("install/Mac/Slither.app/Contents/Info.plist", BundleProperties)
        
        -- Create disc image...
        print("Generating DMG from application bundle...")
        os.execute("hdiutil create -ov -srcfolder install/Mac -volname " 
                   .. "\"" .. PACKAGE_STRING .. "\" -format UDZO " ..
                   OutputPath .. "/Slither_" .. PACKAGE_VERSION .. ".dmg")

        -- Cleanup...
        print("Cleaning up...")
        os.execute("cd install/Mac; rm -Rf Slither.app")

    end

    -- Build a NSIS for Win32...
    if(OS == "windows") then
        
        -- Stubbed...
        error("\nNullsoft installer support not implemented yet...")

    end

end

-- Create a file from a string...
function CreateFileFromString(Path, Output)
    
    -- Create the file...
    file = io.open(Path, "w+")
    
    -- Dump contents into it...
    file.write(file, Output)
    
    -- Close the file...
    io.close(file)

end

-- Custom clean hook...
function doclean(Command, Argument)

    -- Execute default clean code for '--clean'
    docommand(Command, Argument)

    -- Remove installers
    os.remove("src/Resources.h")
    os.remove("src/Resources.cpp")
    os.execute("rm -f install/*.deb")
    os.execute("rm -f install/*.exe")
    os.execute("rm -f install/*.dmg")
end

-- Custom target hook...
function dotarget(Command, Argument)

    -- Only support real compilers...
    if not(Argument == "gnu" or Argument == "cb-gcc") then
        error("\nSlither only builds with a real compiler. Please install gcc...")
    end

    -- Execute default target code for '--target'
    docommand(Command, Argument)

end

