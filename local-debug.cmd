@echo off

setlocal

if exist %~dp0x64\Debug\test.exe (
    psexec -sid windbg -v -n -srcpath "%~dp0" -y %~dp0x64\Debug %~dp0x64\Debug\test.exe %*
)

if %errorlevel% EQU 5 (
    echo Batch must be run from an elevated command prompt
) else (
    echo Started windbg under the system account
)

endlocal

echo.
