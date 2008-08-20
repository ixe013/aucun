@echo off

setlocal

set UNLOCK_GROUP=aucun-unlock
set FORCE_LOGOFF_GROUP=aucun-logoff
set EXCLUDED_GROUP=aucun-excluded

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
echo Expected result : eLetMSGINAHandleIt
rem *------------------------------------------------------------*
echo %TEST_USER_PASSWORD% | release\test.exe %1
echo.

rem *------------------------------------------------------------*
echo Running test : groups are set, but empty
echo Expected result : eLetMSGINAHandleIt
rem *------------------------------------------------------------*
reg add HKLM\Software\Paralint.com\Aucun\Groups /v Unlock /t REG_SZ /d %UNLOCK_GROUP% /f 1> nul 2> nul
reg add HKLM\Software\Paralint.com\Aucun\Groups /v Logoff /t REG_SZ /d %FORCE_LOGOFF_GROUP% /f 1> nul 2> nul
reg add HKLM\Software\Paralint.com\Aucun\Groups /v Excluded /t REG_SZ /d %EXCLUDED_GROUP% /f 1> nul 2> nul
echo %TEST_USER_PASSWORD% | release\test.exe %1
echo.


echo TEST FINISHED !!!
goto END

:TESTS_HELP
echo Pass the username to use for tests

:END

endlocal
