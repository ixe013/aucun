@echo off                            

setlocal

SET PROJECT_NAME=aucun
SET VCBUILD_DEFAULT_CFG=

echo Zipping versioned project files
if exist %PROJECT_NAME%.zip del %PROJECT_NAME%.zip
if exist %PROJECT_NAME%-src.zip del %PROJECT_NAME%-src.zip
svn st -v | findstr /V /B "[\?CDIX\!\~]" | gawk "{ $0 = substr($0, 6); print $4 }" | zip %PROJECT_NAME%-src.zip -@ 

echo.
echo Preparing for build
md %PROJECT_NAME%

pushd %PROJECT_NAME%

unzip -q ..\%PROJECT_NAME%-src.zip

echo.
findstr /s /n DebugBreak *.c *.cpp *.h 
if ERRORLEVEL 1 (
echo Building...
vcbuild /nologo

echo Creating binary zip
zip -j -q ..\%PROJECT_NAME%.zip README.txt release\%PROJECT_NAME%.dll x64\release\%PROJECT_NAME%64.dll sample.reg
) else (
echo.
echo DebugBreak found in source code. Fix it or die.
if exist ..\%PROJECT_NAME%.zip del ..\%PROJECT_NAME%.zip
if exist ..\%PROJECT_NAME%-src.zip del ..\%PROJECT_NAME%-src.zip
)

popd

rd /s /q %PROJECT_NAME% 

echo.
dir *.zip | findstr zip
echo.
echo Done.
endlocal
echo.
