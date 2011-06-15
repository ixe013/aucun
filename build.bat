@echo off
for %%i in (*.sln) do @msbuild %%i /v:m /nologo %*
