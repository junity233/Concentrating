Name "Concentrating V1.0"
OutFile "Concentrating.exe"
InstallDir $PROGRAMFILES64\Concentrating
Unicode true

Page directory 
Page instfiles


Section "main"
    SetOutPath $INSTDIR
    File /r "x64\Release\*.*"
SectionEnd