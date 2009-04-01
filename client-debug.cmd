@start "" windbg -v -n -b -k com:pipe,port=\\.\pipe\vpc1,reconnect -srcpath "%~dp0" -y c:\windows\system32;c:\windows\symbols
