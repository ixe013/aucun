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

echo Aucun Win32 Release
msbuild AnyUserUnlockGina.vcxproj /nologo /v:q /p:Platform=Win32;Configuration=Release
echo Aucun Win32 Debug
msbuild AnyUserUnlockGina.vcxproj /nologo /v:q /p:Platform=Win32;Configuration=Debug
echo Aucun x64 Release
msbuild AnyUserUnlockGina.vcxproj /nologo /v:q /p:Platform=x64;Configuration=Release
echo Aucun x64 Debug
msbuild AnyUserUnlockGina.vcxproj /nologo /v:q /p:Platform=x64;Configuration=Debug

echo Test Win32 Release
msbuild test.vcxproj /nologo /v:q /p:Platform=Win32;Configuration=Release
echo Test Win32 Debug
msbuild test.vcxproj /nologo /v:q /p:Platform=Win32;Configuration=Debug
echo Test x64 Release
msbuild test.vcxproj /nologo /v:q /p:Platform=x64;Configuration=Release
echo Test x64 Release
msbuild test.vcxproj /nologo /v:q /p:Platform=x64;Configuration=Debug

echo Creating binary zip
zip -j -q ..\%PROJECT_NAME%.zip README.txt Release\%PROJECT_NAME%.dll x64\Release\%PROJECT_NAME%64.dll sample.reg
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
unzip -l %PROJECT_NAME%.zip *.dll
if NOT ERRORLEVEL 0 (
echo.
echo Binary not found in distribution. Fix it or die.
if exist ..\%PROJECT_NAME%.zip del ..\%PROJECT_NAME%.zip
if exist ..\%PROJECT_NAME%-src.zip del ..\%PROJECT_NAME%-src.zip
)

echo.
echo Done.
endlocal
echo.
