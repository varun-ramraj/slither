/*
  Name:         SlitherSetup.nsi
  Author:       Kip Warner [kip@thevertigo.com]
  Description:  Nullsoft Installer script to generate Slither installer for
                32-bit Windoze platform.
  Notes:        This is just a stub and hasn't been tested yet, since it takes
                forever to setup a build environment on Windoze without a
                package manager. Use it as a starting point for a script to
                package Slither for distribution on Windoze.
*/

; Pre-procs...
!define PRODUCT_NAME            "Slither"
!define PRODUCT_MAJOR_VERSION   /* TODO: Pass from scons */
!define PRODUCT_MINOR_VERSION   /* TODO: Pass from scons */
!define PRODUCT_PUBLISHER       "Small Neural Systems Group"
!define PRODUCT_WEB_SITE        "http://slither.thevertigo.com"
!define PRODUCT_DIR_REGKEY      "Software\Microsoft\Windows\CurrentVersion\App Paths\makensis.exe"
!define PRODUCT_UNINST_KEY      "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
!define PRODUCT_UNINST_ROOT_KEY "HKLM"

; Plugins...
!addplugindir ".\Files\Support\Plugins"

; General attributes...
CompletedText "${PRODUCT_NAME} installed successfully..."
CRCCheck force
DirText "Setup will install $(^Name) in the following folder.$\r$\n$\r$\nTo install in a different folder, click Browse and select another folder."
Icon "Shell.ico"
InstallDir "$PROGRAMFILES\${PRODUCT_NAME}"
InstallDirRegKey HKLM "${PRODUCT_DIR_REGKEY}" ""
InstProgressFlags "smooth"
LicenseData "../../LICENSE"
LicenseForceSelection checkbox
LicenseText "If you accept all the terms of the agreement, choose I Agree to continue. You must accept the agreement to install $(^Name)."
LoadLanguageFile "${NSISDIR}\Contrib\Language files\English.nlf"
Name "${PRODUCT_NAME} ${PRODUCT_VERSION}"
OutFile "SlitherSetup.exe"
ShowInstDetails show
ShowUnInstDetails show
UninstallIcon "Shell.ico"
XPStyle on

; Version image stuff...
VIProductVersion "${PRODUCT_VERSION}"
VIAddVersionKey /LANG=${LANG_ENGLISH} "ProductName" ${PRODUCT_NAME}
VIAddVersionKey /LANG=${LANG_ENGLISH} "Comments" "Installer compiled on ${__DATE__} at ${__TIME__}."
VIAddVersionKey /LANG=${LANG_ENGLISH} "CompanyName" "${PRODUCT_PUBLISHER}"
VIAddVersionKey /LANG=${LANG_ENGLISH} "LegalTrademarks" "${PRODUCT_NAME} is a trademark of ${PRODUCT_PUBLISHER}."
VIAddVersionKey /LANG=${LANG_ENGLISH} "LegalCopyright" "© ${PRODUCT_PUBLISHER}"
VIAddVersionKey /LANG=${LANG_ENGLISH} "FileDescription" "${PRODUCT_NAME} installer for version ${PRODUCT_VERSION}."
VIAddVersionKey /LANG=${LANG_ENGLISH} "FileVersion" "${PRODUCT_VERSION}"

; Page display order...
Page instfiles

; GUI initializing...
Function .onInit

    ; Prevent multiple instances of installer...

        ; Create a mutex...
        System::Call 'kernel32::CreateMutexA(i 0, i 0, t "${PRODUCT_NAME}Setup") i .r1 ?e'
        Pop $R0

        ; Installer already running...
        StrCmp $R0 0 +3
        MessageBox MB_OK|MB_ICONEXCLAMATION "The ${PRODUCT_NAME} installer is already running."
        Quit

    ; Set default installation directory...
    StrCpy $INSTDIR "$PROGRAMFILES\${PRODUCT_NAME}"

FunctionEnd

; Installation stuff...

    ; Main application section...
    Section "!Slither" SF_SELECTED

        ; Display system information...
        DetailPrint "System information:"

            ; System name...
            System::Call "kernel32::GetComputerName(t .R0, *i ${NSIS_MAX_STRLEN} R1) i.R2"
            DetailPrint " Machine Name: $R0"

            ; Login name...
            System::Call "advapi32::GetUserName(t .r0, *i ${NSIS_MAX_STRLEN} r1) i.r2"
            DetailPrint " User: $0"

            ; CPU name and speed...
            HwInfo::GetCpuNameAndSpeed
            DetailPrint " CPU: $0"

            ; System memory...
            HwInfo::GetSystemMemory
            DetailPrint " System Memory: $0MB"

            ; Video adapter name...
            HwInfo::GetVideoCardName
            DetailPrint " Video Adapter: $0"

            ; Video adapter memory...
            HwInfo::GetVideoCardMemory
            DetailPrint " Video Memory: $0MB"

        ; Install program files...
        DetailPrint "Installing ${PRODUCT_NAME}..."
        SetOverwrite on
        CreateDirectory "$INSTDIR"
        SetOutPath "$INSTDIR"
        
            ; Main executable...
            File "../../*.exe"
    
        ; Create URL icon...
        WriteIniStr "$INSTDIR\${PRODUCT_NAME}.url" "InternetShortcut" "URL" "${PRODUCT_WEB_SITE}"
    
        ; Write uninstaller and registry stuff...
        DetailPrint "Creating uninstaller..."
        WriteUninstaller "$INSTDIR\Uninstall.exe"
        WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayName" "$(^Name)"
        WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString" "$INSTDIR\Uninstall.exe"
        WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayIcon" "$INSTDIR\slither.exe"
        WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayVersion" "${PRODUCT_VERSION}"
        WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "URLInfoAbout" "${PRODUCT_WEB_SITE}"
        WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "Publisher" "${PRODUCT_PUBLISHER}"
        
        ; Initialize registry...
        DetailPrint "Initializing registry..."
        WriteRegStr HKLM "Software\${PRODUCT_PUBLISHER}\${PRODUCT_NAME}\Paths" "AppPath" "$INSTDIR"
        WriteRegStr HKLM "Software\${PRODUCT_PUBLISHER}\${PRODUCT_NAME}\WindowPos" "Dummy" ""
        DeleteRegValue HKLM "Software\${PRODUCT_PUBLISHER}\${PRODUCT_NAME}\WindowPos" "Dummy"
    
    SectionEnd

    ; Desktop icon section...
    Section "Desktop icon"
        CreateShortCut "$DESKTOP\Slither.lnk" "$INSTDIR\slither.exe"
    SectionEnd

    ; Start menu icon section...
    Section "Start menu icon"

        CreateDirectory "$SMPROGRAMS\Slither"
        CreateShortCut  "$SMPROGRAMS\Slither\Slither.lnk" "$INSTDIR\slither.exe"
        CreateShortCut  "$SMPROGRAMS\Slither\Website.lnk" "$INSTDIR\${PRODUCT_NAME}.url"
        CreateShortCut  "$SMPROGRAMS\Slither\Uninstall.lnk" "$INSTDIR\Uninstall.exe"

    SectionEnd

; Installation successful...
Function .onInstSuccess

    ; Save log...
    StrCpy $0 "$INSTDIR\Install.log"
    Push $0
    Delete "$INSTDIR\Install.log"
    Call DumpLog
    SetFileAttributes "$INSTDIR\Install.log" FILE_ATTRIBUTE_NORMAL|FILE_ATTRIBUTE_READONLY

FunctionEnd

; Uninstall stuff...

    ; Initialize uninstall...
    Function un.onInit
    
        ; Prompt user for confirmation. Abort if requested...
        MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "Are you sure you want to completely remove $(^Name), your user license, and all of the contents of its program directory?" IDYES +2
        Quit
        
    FunctionEnd
    
    ; Uninstall...
    Section Uninstall
    
        ; Remove start menu icons...
        RMDir /r "$SMPROGRAMS\Slither"
      
        ; Remove desktop icon...
        Delete "$DESKTOP\Slither.lnk"
        
        ; Remove installation directory and everything in it recursively...
        RmDir /r "$INSTDIR"
    
        ; Cleanup registry...
        
            ; Slither stuff...
            DeleteRegKey HKLM "Software\${PRODUCT_PUBLISHER}\${PRODUCT_NAME}"
            
            ; Cleanup uninstaller specific stuff...
            DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
            DeleteRegKey HKLM "${PRODUCT_DIR_REGKEY}"
            
        ; Alert user...
        DetailPrint "Uninstall successful..."
        Sleep 1500
        
        ; Close uninstaller...
        SetAutoClose true
        
    SectionEnd

!define LVM_GETITEMCOUNT 0x1004
!define LVM_GETITEMTEXT 0x102D

; Dump contents of detail view to log file in $0 register...
Function DumpLog
  Exch $5
  Push $0
  Push $1
  Push $2
  Push $3
  Push $4
  Push $6

  FindWindow $0 "#32770" "" $HWNDPARENT
  GetDlgItem $0 $0 1016
  StrCmp $0 0 error
  FileOpen $5 $5 "w"
  StrCmp $5 0 error
    SendMessage $0 ${LVM_GETITEMCOUNT} 0 0 $6
    System::Alloc ${NSIS_MAX_STRLEN}
    Pop $3
    StrCpy $2 0
    System::Call "*(i, i, i, i, i, i, i, i, i) i \
      (0, 0, 0, 0, 0, r3, ${NSIS_MAX_STRLEN}) .r1"
    loop: StrCmp $2 $6 done
      System::Call "User32::SendMessageA(i, i, i, i) i \
        ($0, ${LVM_GETITEMTEXT}, $2, r1)"
      System::Call "*$3(&t${NSIS_MAX_STRLEN} .r4)"
      FileWrite $5 "$4$\r$\n"
      IntOp $2 $2 + 1
      Goto loop
    done:
      FileClose $5
      System::Free $1
      System::Free $3
      Goto exit
  error:
    MessageBox MB_OK error
  exit:
    Pop $6
    Pop $4
    Pop $3
    Pop $2
    Pop $1
    Pop $0
    Exch $5
FunctionEnd

