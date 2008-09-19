@echo off
setlocal
msbuild AnyUserUnlockGina.sln /nologo /p:Configuration=Release /t:Rebuild
msbuild AnyUserUnlockGina.sln /nologo /p:Configuration=Debug   /t:Rebuild

SET TEMP_FILE=%RANDOM%-%RANDOM%.tmp
svn info | findstr URL | gawk '{print $2}' > %TEMP_FILE%

SET /P SVN_URL= < %TEMP_FILE%
del %TEMP_FILE%

svn co %SVN_URL% aucun

zip -rp aucun-src.zip aucun\*
rd /s /q aucun 

zip -j aucun.zip README.txt release\aucun.dll sample.reg

endlocal
