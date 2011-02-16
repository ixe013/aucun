------------------------------------------------
Any user can unlock now, a replacement GINA DLL
------------------------------------------------

This file is a summary. Please refer to http://www.paralint.com/projects/aucun/
for more information.

Purpose
-------
This replacement GINA will allow members of any group you specify to force 
logoff any user who locked his workstation. You can also allow members of any
other group you specify to unlock a locked session. 

The GUI is provided by entierly by the original MSGINA.DLL. My replacement GINA
just wraps the original DLL and overrides unlock decisions. 
                                            
Installation
------------
1. Copy AUCUN.DLL anywhere. 
2. Add a string value key named GinaDLL under 
   HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Winlogon 
3. Set the value of the new GinaDLL key to aucun.dll (add the path if it is not 
   in system32.
4. Modify the file Sample.reg to use your groups. Everything is optionnal. If an
   entry is removed, that feature is silently disable (like the warning message).
5. Merge Sample.reg to the registry and reboot

Messed up ?
-----------
You can boot in Safe Mode, it will disable any custom GINA.

Security
--------
You can break the security of your system if you are not careful:
  . If you use the unlock feature, your users will be able to impersonate others
  . If you put Aucun.DLL in a user writable directory, they will be able to
elevate their privileges

Please refer to http://www.paralint.com/projects/aucun/ for more information.


--
Guillaume (at paralint.com)
