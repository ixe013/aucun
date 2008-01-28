@echo off
echo F | xcopy /y /q release\aucun.dll release\aucun2.dll 

rem ------------------------------
rem Take a chance with Aucun.dll
rem ------------------------------
echo.
reg add "HKLM\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Winlogon" /v GinaDLL /t REG_SZ /d aucun.dll /f > nul
xcopy /y /f release\aucun.dll %WINDIR%\system32\.

if not %ERRORLEVEL%==0 (

rem ------------------------------
rem It must be aucun2.dll
rem ------------------------------
echo.
reg add "HKLM\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Winlogon" /v GinaDLL /t REG_SZ /d aucun2.dll /f > nul
xcopy /y /f release\aucun2.dll %WINDIR%\system32\.
)
:SHUTUPANDREBOOT

echo.
dir /od %windir%\system32\aucun*.dll | findstr /I /c:aucun
reg query "HKLM\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Winlogon" /v GinaDLL

shutdown /r /t 5 /f

