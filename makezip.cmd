@echo off                            

setlocal

SET PROJECT_NAME=aucun

echo Getting repository URL
SET TEMP_FILE=%RANDOM%-%RANDOM%.tmp
svn info | findstr URL | gawk "{print $2}" > %TEMP_FILE%

SET /P SVN_URL= < %TEMP_FILE%
del %TEMP_FILE%

echo Checking out files
svn co -q %SVN_URL% %PROJECT_NAME%

echo Creating source zip
zip -rp -q %PROJECT_NAME%-src.zip %PROJECT_NAME%\*

pushd %PROJECT_NAME%

echo Building...
vcbuild /nologo

echo Creating binary zip
zip -j -q ..\%PROJECT_NAME%.zip README.txt release\%PROJECT_NAME%.dll x64\release\%PROJECT_NAME%64.dll sample.reg

popd

rd /s /q %PROJECT_NAME% 

echo.
dir *.zip | findstr zip
echo.
echo Done.
endlocal
echo.
