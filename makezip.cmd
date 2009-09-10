@echo off                            

setlocal

SET PROJECT_NAME=aucun

echo Zipping versioned project files
if exist %PROJECT_NAME%-src.zip del %PROJECT_NAME%-src.zip
svn st -v | findstr /V /B "[\?CDIX\!\~]" | gawk "{ $0 = substr($0, 6); print $4 }" | zip %PROJECT_NAME%-src.zip -@ 


echo Preparing for build
md %PROJECT_NAME%

pushd %PROJECT_NAME%

unzip -q ..\%PROJECT_NAME%-src.zip

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
