@echo off

start "" windbg -k com:pipe,port=\\.\pipe\vpc1,reconnect -srcpath "%~dp0" 
