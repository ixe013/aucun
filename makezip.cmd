@echo off                            

setlocal

SET PROJECT_NAME=aucun

echo Getting repository URL
SET TEMP_FILE=%RANDOM%-%RANDOM%.tmp
svn info | findstr URL > %TEMP_FILE%

SET /P SVN_URL= < %TEMP_FILE%
del %TEMP_FILE%

echo Checking out files
svn co -q %SVN_URL:~5% %PROJECT_NAME%

echo Creating source zip
zip -rp -q %PROJECT_NAME%-src.zip %PROJECT_NAME%\*

pushd %PROJECT_NAME%

echo Building release configuration
for %%i in (*.sln) do msbuild %%i /nologo /v:q /p:Configuration=Release /t:Rebuild
echo Building debug configuration
for %%i in (*.sln) do msbuild %%i /nologo /v:q /p:Configuration=Debug   /t:Rebuild

echo Creating binary zip
zip -j -q ..\%PROJECT_NAME%.zip README.txt release\%PROJECT_NAME%.dll sample.reg

popd

rd /s /q %PROJECT_NAME% 

dir *.zip | findstr zip
echo.
echo Done.
endlocal
echo.
