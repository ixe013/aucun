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
reg add HKLM\Software\Paralint.com\Aucun\Groups /v "Unlock" 1> nul 2> nul
reg add HKLM\Software\Paralint.com\Aucun\Groups /v "Force logoff" 1> nul 2> nul

set /p TEST_USER_PASSWORD="Enter the password of %1 : " 
echo.

rem The tests are ordered so that the hamming distance between 
rem any two tests is one. In other words, if we add the user to
rem a group, we don't touch the registry, and we never add or 
rem remove more than one user or group at a time. 
rem
rem More on this on
rem http://www.paralint.com/blog/2008/08/21/easy-test-coverage-with-constant-hamming-distance-of-1/

echo                %d   User  Registry
echo                     U  F   U  F

rem *------------------------------------------------------------*
echo Running test :  0   0  0   0  0
rem  Expected result : eLetMSGINAHandleIt
rem *------------------------------------------------------------*
echo %TEST_USER_PASSWORD%| %TEST_EXE% %1
if NOT ERRORLEVEL %eLetMSGINAHandleIt% echo ***FAILED*** (return code %ERRORLEVEL%, not %eLetMSGINAHandleIt%)
echo.


rem *------------------------------------------------------------*
echo Running test :  1   0  0   0  1
rem  Expected result : eLetMSGINAHandleIt
rem *------------------------------------------------------------*
reg add HKLM\Software\Paralint.com\Aucun\Groups /v "Force logoff" /f /d %FORCE_LOGOFF_GROUP%  1> nul 2> nul
echo %TEST_USER_PASSWORD%| %TEST_EXE% %1
if NOT ERRORLEVEL %eLetMSGINAHandleIt% echo ***FAILED*** (return code %ERRORLEVEL%, not %eLetMSGINAHandleIt%)
echo.


rem *------------------------------------------------------------*
echo Running test :  3   0  0   1  1
rem  Expected result : eLetMSGINAHandleIt
rem *------------------------------------------------------------*
reg add HKLM\Software\Paralint.com\Aucun\Groups /v "Unlock" /f /d %UNLOCK_GROUP%  1> nul 2> nul
echo %TEST_USER_PASSWORD%| %TEST_EXE% %1
if NOT ERRORLEVEL %eLetMSGINAHandleIt% echo ***FAILED*** (return code %ERRORLEVEL%, not %eLetMSGINAHandleIt%)
echo.


rem *------------------------------------------------------------*
echo Running test :  2   0  0   1  0
rem  Expected result : eLetMSGINAHandleIt
rem *------------------------------------------------------------*
reg add HKLM\Software\Paralint.com\Aucun\Groups /v "Force logoff" /f 1> nul 2> nul
echo %TEST_USER_PASSWORD%| %TEST_EXE% %1
if NOT ERRORLEVEL %eLetMSGINAHandleIt% echo ***FAILED*** (return code %ERRORLEVEL%, not %eLetMSGINAHandleIt%)
echo.


rem *------------------------------------------------------------*
echo Running test :  6   0  1   1  0
rem  Expected result : eLetMSGINAHandleIt
rem *------------------------------------------------------------*
net localgroup %FORCE_LOGOFF_GROUP% %1 /add
echo %TEST_USER_PASSWORD%| %TEST_EXE% %1
if NOT ERRORLEVEL %eLetMSGINAHandleIt% echo ***FAILED*** (return code %ERRORLEVEL%, not %eLetMSGINAHandleIt%)
echo.


rem *------------------------------------------------------------*
echo Running test :  7   0  1   1  1
rem  Expected result : eForceLogoff
rem *------------------------------------------------------------*
reg add HKLM\Software\Paralint.com\Aucun\Groups /v "Force logoff" /f /d %FORCE_LOGOFF_GROUP%  1> nul 2> nul
echo %TEST_USER_PASSWORD%| %TEST_EXE% %1
if NOT ERRORLEVEL %eForceLogoff% echo ***FAILED*** (return code %ERRORLEVEL%, not %eForceLogoff%)
echo.


rem *------------------------------------------------------------*
echo Running test :  5   0  1   0  1
rem  Expected result : eForceLogoff
rem *------------------------------------------------------------*
reg add HKLM\Software\Paralint.com\Aucun\Groups /v "Unlock" /f 1> nul 2> nul
echo %TEST_USER_PASSWORD%| %TEST_EXE% %1
if NOT ERRORLEVEL %eForceLogoff% echo ***FAILED*** (return code %ERRORLEVEL%, not %eLetMSGINAHandleIt%)
echo.


rem *------------------------------------------------------------*
echo Running test :  4   0  1   0  0
rem  Expected result : eLetMSGINAHandleIt
rem *------------------------------------------------------------*
reg add HKLM\Software\Paralint.com\Aucun\Groups /v "Force logoff" /f 1> nul 2> nul
echo %TEST_USER_PASSWORD%| %TEST_EXE% %1
if NOT ERRORLEVEL %eLetMSGINAHandleIt% echo ***FAILED*** (return code %ERRORLEVEL%, not %eLetMSGINAHandleIt%)
echo.


rem *------------------------------------------------------------*
echo Running test : 12   1  1   0  0
rem  Expected result : eLetMSGINAHandleIt
rem *------------------------------------------------------------*
reg add HKLM\Software\Paralint.com\Aucun\Groups /v Unlock /f /d  %UNLOCK_GROUP% /f 1> nul 2> nul
echo %TEST_USER_PASSWORD%| %TEST_EXE% %1
if NOT ERRORLEVEL %eLetMSGINAHandleIt% echo ***FAILED*** (return code %ERRORLEVEL%, not %eLetMSGINAHandleIt%)
echo.


rem *------------------------------------------------------------*
echo Running test : 13   1  1   0  1
rem  Expected result : eForceLogoff
rem *------------------------------------------------------------*
reg add HKLM\Software\Paralint.com\Aucun\Groups /v "Force logoff" /f /d %FORCE_LOGOFF_GROUP%  1> nul 2> nul
echo %TEST_USER_PASSWORD%| %TEST_EXE% %1
if NOT ERRORLEVEL %eForceLogoff% echo ***FAILED*** (return code %ERRORLEVEL%, not %eForceLogoff%)
echo.


rem *------------------------------------------------------------*
echo Running test : 15   1  1   1  1
rem  Expected result : eUnlock
rem *------------------------------------------------------------*
reg add HKLM\Software\Paralint.com\Aucun\Groups /v "Unlock" /f /d %UNLOCK_GROUP%  1> nul 2> nul
echo %TEST_USER_PASSWORD%| %TEST_EXE% %1
if NOT ERRORLEVEL %eUnlock% echo ***FAILED*** (return code %ERRORLEVEL%, not %eUnlock%)
echo.


rem *------------------------------------------------------------*
echo Running test : 14   1  1   1  0
rem  Expected result : eUnlock
rem *------------------------------------------------------------*
reg add HKLM\Software\Paralint.com\Aucun\Groups /v "Force logoff" /f 1> nul 2> nul
echo %TEST_USER_PASSWORD%| %TEST_EXE% %1
if NOT ERRORLEVEL %eUnlock% echo ***FAILED*** (return code %ERRORLEVEL%, not %eUnlock%)
echo.


rem *------------------------------------------------------------*
echo Running test : 10   1  0   1  0
rem  Expected result : eUnlock
rem *------------------------------------------------------------*
net localgroup %FORCE_LOGOFF_GROUP% %1 /delete
echo %TEST_USER_PASSWORD%| %TEST_EXE% %1
if NOT ERRORLEVEL %eUnlock% echo ***FAILED*** (return code %ERRORLEVEL%, not %eUnlock%)
echo.


rem *------------------------------------------------------------*
echo Running test : 11   1  0   1  1
rem  Expected result : eUnlock
rem *------------------------------------------------------------*
reg add HKLM\Software\Paralint.com\Aucun\Groups /v "Force logoff" /f /d %FORCE_LOGOFF_GROUP%  1> nul 2> nul
echo %TEST_USER_PASSWORD%| %TEST_EXE% %1
if NOT ERRORLEVEL %eUnlock% echo ***FAILED*** (return code %ERRORLEVEL%, not %eUnlock%)
echo.


rem *------------------------------------------------------------*
echo Running test :  9   1  0   0  1
rem  Expected result : eLetMSGINAHandleIt
rem *------------------------------------------------------------*
reg add HKLM\Software\Paralint.com\Aucun\Groups /v "Unlock" /f 1> nul 2> nul
echo %TEST_USER_PASSWORD%| %TEST_EXE% %1
if NOT ERRORLEVEL %eLetMSGINAHandleIt% echo ***FAILED*** (return code %ERRORLEVEL%, not %eLetMSGINAHandleIt%)
echo.


rem *------------------------------------------------------------*
echo Running test :  8   1  0   0  0
rem  Expected result : eLetMSGINAHandleIt
rem *------------------------------------------------------------*
reg add HKLM\Software\Paralint.com\Aucun\Groups /v "Force logoff" /f 1> nul 2> nul
echo %TEST_USER_PASSWORD%| %TEST_EXE% %1
if NOT ERRORLEVEL %eLetMSGINAHandleIt% echo ***FAILED*** (return code %ERRORLEVEL%, not %eLetMSGINAHandleIt%)
echo.


echo TEST FINISHED !!!
goto END

:TESTS_HELP
echo Pass the username to use for tests

:END

endlocal
