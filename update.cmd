@echo off

setlocal
rem ------------------------------
rem Make a time based file name
rem ------------------------------
set AUCUN_DLL_NAME=AUCUN-%RANDOM%.dll

rem ------------------------------
rem Delete any old DLL, but one...
rem ------------------------------
del /q %windir%\system32\aucun*.dll

echo F | xcopy /v .\debug\aucun.dll "%windir%\system32\%AUCUN_DLL_NAME%"

rem ------------------------------
rem Edit the registry
rem ------------------------------
reg add "HKLM\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Winlogon" /v GinaDLL /t REG_SZ /d "%AUCUN_DLL_NAME%" /f > nul

if %ERRORLEVEL%==0 (

reg query "HKLM\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Winlogon" /v GinaDLL
dir /od %windir%\system32\aucun*.dll | findstr /I /c:aucun

shutdown /r /t 5 /f

)

endlocal


