;NSIS Modern User Interface
;Instalator gry "Stary Dom"
;Written by L.K. Progmar Marcin Za³êczny

Var DORESTART

;--------------------------------
;Include Modern UI

  !include "MUI2.nsh"

;--------------------------------
;General

  ;Name and file
  Name "Gra 'Stary Dom'"
  OutFile "InstalatorStaryDom.exe"
  Unicode True

  ;Default installation folder
  InstallDir "$LOCALAPPDATA\LKProgmar\Games\StaryDom"
  
  ;Get installation folder from registry if available
  InstallDirRegKey HKCU "Software\LKProgmar\Games\StaryDom\InstallDir" ""

  ;Request application privileges for Windows Vista
  RequestExecutionLevel user

;--------------------------------
;Interface Settings

  !define MUI_ABORTWARNING

;--------------------------------
;Pages

  ;!insertmacro MUI_PAGE_LICENSE "${NSISDIR}\Docs\Modern UI\License.txt"
  !insertmacro MUI_PAGE_COMPONENTS
  !insertmacro MUI_PAGE_DIRECTORY
  !insertmacro MUI_PAGE_INSTFILES
  
  !insertmacro MUI_UNPAGE_CONFIRM
  !insertmacro MUI_UNPAGE_INSTFILES
  
;--------------------------------
;Languages
 
  !insertmacro MUI_LANGUAGE "Polish"


;--------------------------------
; The stuff to install
Section "StaryDom (wymagany)"
  SectionIn RO
  
  ; Set output path to the installation directory.
  SetOutPath $INSTDIR

  ; Put file there
  File "f:\instalki\VC_redist.x64.exe"
  File "f:\instalki\StaryDom\StaryDom.exe"
  
  ; Write the installation path into the registry
  WriteRegStr HKCU "SOFTWARE\LKProgmar\Games\StaryDom" "InstallDir" "$INSTDIR"
  
  ; Write the uninstall keys for Windows
  WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\StaryDom" "DisplayName" "StaryDom"
  WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\StaryDom" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegDWORD HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\StaryDom" "NoModify" 1
  WriteRegDWORD HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\StaryDom" "NoRepair" 1
  WriteUninstaller "$INSTDIR\uninstall.exe"

SectionEnd

; Optional section (can be disabled by the user)
Section "Utwórz skróty w Menu Start"

  CreateDirectory "$SMPROGRAMS\StaryDom"
  CreateShortcut "$SMPROGRAMS\StaryDom\Uninstall.lnk" "$INSTDIR\uninstall.exe"
  CreateShortcut "$SMPROGRAMS\StaryDom\StaryDom.lnk" "$INSTDIR\StaryDom.exe"

SectionEnd

; Optional section (can be disabled by the user)
Section "Utwórz skrót na Pulpicie"

  CreateShortcut "$DESKTOP\StaryDom.lnk" "$INSTDIR\StaryDom.exe"

SectionEnd


Section "Instalacja wymaganych bibliotek (wymagany)"
  SectionIn RO

  SetOutPath "$INSTDIR"

  InitPluginsDir
  File /oname=$PLUGINSDIR\CheckRedist.exe f:\instalki\CheckRedist.exe
  nsExec::Exec '"$PLUGINSDIR\CheckRedist.exe"'
  Pop $0

  ${If} $0 != 0
      DetailPrint "Nie znaleziono wymagana biblioteka uruchomieniowej œrodowiska VC++! Instalujê ..."
      ExecWait '"$INSTDIR\VC_redist.x64.exe" /q /norestart'
      DetailPrint "Zainstalowano"
      Strcpy "$DORESTART" "Restart"
  ${Else}
      DetailPrint "Znaleziono bibliotekê uruchomieniow¹ œrodowiska VC++."
      Strcpy "$DORESTART" "NoRestart"
  ${EndIf}

  Strcmp $DORESTART "NoRestart" NoRestart
  MessageBox MB_YESNO|MB_ICONQUESTION "Zainstalowano sk³adniki, które wymagaj¹ restartu komputera. Czy chcesz to zrobiæ teraz?" IDNO +2
    Reboot

  NoRestart:

SectionEnd

;--------------------------------
;Uninstaller Section

Section "Uninstall"

  ;ADD YOUR OWN FILES HERE...
  Delete "$INSTDIR\StaryDom.exe"
  Delete "$INSTDIR\score.dat"
  Delete "$INSTDIR\output.log"
  Delete "$INSTDIR\VC_redist.x64.exe"
  Delete "$INSTDIR\Uninstall.exe"

  RMDir "$INSTDIR"

  Delete "$SMPROGRAMS\StaryDom\StaryDom.lnk"
  Delete "$SMPROGRAMS\StaryDom\Uninstall.lnk"
  RMDir "$SMPROGRAMS\StaryDom"
  Delete "$DESKTOP\StaryDom.lnk"

  DeleteRegKey HKCU "Software\LKProgmar\Games\StaryDom"
  DeleteRegKey HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\StaryDom"

SectionEnd