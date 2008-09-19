@echo off
setlocal
msbuild AnyUserUnlockGina.sln /nologo /v:m /p:Configuration=Release /t:Rebuild
msbuild AnyUserUnlockGina.sln /nologo /v:m /p:Configuration=Debug   /t:Rebuild

SET TEMP_FILE=%RANDOM%-%RANDOM%.tmp
svn info | findstr URL | gawk '{print $2}' > %TEMP_FILE%

SET /P SVN_URL= < %TEMP_FILE%
del %TEMP_FILE%

svn co -q %SVN_URL% aucun

del aucun-src.zip
del aucun.zip

zip -rp -q aucun-src.zip aucun\*
rd /s /q aucun 

zip -j -q aucun.zip README.txt release\aucun.dll sample.reg

endlocal
