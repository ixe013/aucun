@echo off

setlocal
pushd %~dp0

rem ------------------------------
rem Make a time based file name
rem ------------------------------
set AUCUN_DLL_NAME=AUCUN-%RANDOM%.dll
set AUCUN_PDB_NAME=AUCUN-%RANDOM%.pdb

reg add "HKLM\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Winlogon" /v GinaDLL /t REG_SZ /d aucun.dll /f 

rem ------------------------------
rem Delete any old DLL, but one...
rem ------------------------------
del /q %windir%\system32\aucun-*.dll
del /q %windir%\system32\aucun-*.pdb

copy debug\aucun.dll "%windir%\system32\%AUCUN_DLL_NAME%"
copy debug\aucun.pdb "%windir%\system32\%AUCUN_PDB_NAME%"

rem ------------------------------
rem Edit the registry
rem ------------------------------
reg add "HKLM\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Winlogon" /v GinaDLL /t REG_SZ /d aucun.dll /f > nul

rem ------------------------------
rem Delete the old versions
rem ------------------------------
if exist %windir%\system32\aucun.dll movefile %windir%\system32\aucun.dll ""
if exist %windir%\system32\aucun.pdb movefile %windir%\system32\aucun.pdb ""

rem ------------------------------
rem Prepare the new version
rem ------------------------------
movefile "%windir%\system32\%AUCUN_DLL_NAME%" %windir%\system32\aucun.dll
movefile "%windir%\system32\%AUCUN_PDB_NAME%" %windir%\system32\aucun.pdb

dir /od %windir%\system32\aucun*.* | findstr /I /c:aucun

pendmoves 

copy selfserviceshell\debug\selfserviceshell.exe %windir%\system32
copy selfserviceshell\debug\selfserviceshell.pdb %windir%\system32

shutdown /r /t 5 /f

popd

endlocal


