#define MyAppName "Faidworks Input Viewer"
#define MyAppExeName "faidworks-input-viewer.exe"
#define MyAppPublisher "Faidworks"
#define MyAppVersion GetEnv('APP_VERSION')
#if MyAppVersion == ""
  #define MyAppVersion "0.0.0"
#endif

[Setup]
AppId={{B3F7A2E1-9C4D-4F8A-B5E6-1A2D3C4E5F6A}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
AppPublisher={#MyAppPublisher}
DefaultDirName={localappdata}\FaidworksInputViewer
DefaultGroupName={#MyAppName}
PrivilegesRequired=lowest
OutputDir=..\build_installer
OutputBaseFilename=FaidworksInputViewer-Setup-v{#MyAppVersion}
LicenseFile=..\LICENSE
Compression=lzma2
SolidCompression=yes
WizardStyle=modern

[Files]
Source: "..\build_win\bin\Release\{#MyAppExeName}"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\resources\*"; DestDir: "{app}\resources"; Flags: ignoreversion recursesubdirs createallsubdirs

[Icons]
Name: "{group}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"
Name: "{group}\Uninstall {#MyAppName}"; Filename: "{uninstallexe}"

[Run]
Filename: "{app}\{#MyAppExeName}"; Description: "Launch {#MyAppName}"; Flags: nowait postinstall skipifsilent
