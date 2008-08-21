@echo off

setlocal

set TEST_EXE=release\test.exe
set UNLOCK_GROUP=aucun-unlock
set FORCE_LOGOFF_GROUP=aucun-logoff
set EXCLUDED_GROUP=aucun-excluded

set eLetMSGINAHandleIt=1
set eUnlock=2
set eForceLogoff=3

if "%1" == "" goto TESTS_HELP

echo Setting up tests....
echo.

rem *------------------------------------------------------------*
rem * Clean the old groups                                       *
rem *------------------------------------------------------------*
net localgroup %UNLOCK_GROUP% /delete 1> nul 2> nul
net localgroup %FORCE_LOGOFF_GROUP% /delete 1> nul 2> nul
net localgroup %EXCLUDED_GROUP% /delete 1> nul 2> nul
net localgroup %UNLOCK_GROUP% /add  1> nul 2> nul 
net localgroup %FORCE_LOGOFF_GROUP% /add  1> nul 2> nul 
net localgroup %EXCLUDED_GROUP% /add 1> nul 2> nul 

rem *------------------------------------------------------------*
rem * Removing the registry settings                             *
rem *------------------------------------------------------------*
reg delete HKLM\Software\Paralint.com\Aucun\Groups /va /f 1> nul 2> nul  

set /p TEST_USER_PASSWORD="Enter the password of %1 " 
echo.

rem *------------------------------------------------------------*
echo Running test : nothing in the registry
rem  Expected result : eLetMSGINAHandleIt
rem *------------------------------------------------------------*
echo %TEST_USER_PASSWORD%| %TEST_EXE% %1
if NOT ERRORLEVEL %eLetMSGINAHandleIt% echo ***FAILED*** (return code %ERRORLEVEL%, not %eLetMSGINAHandleIt%)
echo.


rem *------------------------------------------------------------*
echo Running test : groups are set, but empty
rem  Expected result : eLetMSGINAHandleIt
rem *------------------------------------------------------------*
reg add HKLM\Software\Paralint.com\Aucun\Groups /v Unlock /t REG_SZ /d %UNLOCK_GROUP% /f 1> nul 2> nul
reg add HKLM\Software\Paralint.com\Aucun\Groups /v Logoff /t REG_SZ /d %FORCE_LOGOFF_GROUP% /f 1> nul 2> nul
reg add HKLM\Software\Paralint.com\Aucun\Groups /v Excluded /t REG_SZ /d %EXCLUDED_GROUP% /f 1> nul 2> nul
echo %TEST_USER_PASSWORD%| %TEST_EXE% %1
if NOT ERRORLEVEL %eLetMSGINAHandleIt% echo ***FAILED*** (return code %ERRORLEVEL%, not %eLetMSGINAHandleIt%)
echo.


rem *------------------------------------------------------------*
echo Running test : user member of the unlock group
rem  Expected result : eLetMSGINAHandleIt
rem *------------------------------------------------------------*
net localgroup %UNLOCK_GROUP% %1 /add 1> nul 2> nul
rem echo %TEST_USER_PASSWORD% | %TEST_EXE% %1
echo %TEST_USER_PASSWORD%| %TEST_EXE% %1
if NOT ERRORLEVEL %eUnlock% echo ***FAILED*** (return code %ERRORLEVEL%, not %eUnlock%)
net localgroup %UNLOCK_GROUP% %1 /delete 1> nul 2> nul


echo TEST FINISHED !!!
goto END

:TESTS_HELP
echo Pass the username to use for tests

:END

endlocal
