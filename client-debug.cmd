@echo start "" windbg -v -n -k com:pipe,port=\\.\pipe\com_1,reconnect -srcpath SRV*;"%~dp0" -y c:\windows\system32;c:\windows\symbols;%~dp0Debug
