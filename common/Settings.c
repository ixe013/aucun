/*
 Copyright (c) 2008, Guillaume Seguin (guillaume@paralint.com)
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:

 1. Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.

 2. Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.

 THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 SUCH DAMAGE.
*/

#include <windows.h>
#include "settings.h"

const wchar_t* gUnlockGroupName = L"unlock";
const wchar_t* gForceLogoffGroupName = L"force logoff";
const wchar_t* gExcludedGroupName = L"excluded";



//Will return one of the following
//S_OK We have retrieved a group name
//E_FAIL We have not retrieved a value
HRESULT GetGroupName(const wchar_t *name, wchar_t *group, DWORD size)
{
   return GetSettingText(L"SOFTWARE\\Paralint.com\\Aucun\\Groups", name, group, size);
}

HRESULT GetNoticeText(const wchar_t *name, wchar_t *text, DWORD size)
{
   return GetSettingText(L"SOFTWARE\\Paralint.com\\Aucun\\Notice", name, text, size);
}

HRESULT GetDebugSetting(const wchar_t *name, wchar_t *text, DWORD size)
{
   return GetSettingText(L"SOFTWARE\\Paralint.com\\Aucun\\Debug", name, text, size);
}

HRESULT GetSelfServeSetting(const wchar_t *name, wchar_t *text, DWORD size)
{
   return GetSettingText(L"SOFTWARE\\Paralint.com\\Aucun\\SelfServe", name, text, size);
}

HRESULT SetSelfServeSetting(const wchar_t *name, wchar_t *text)
{
   return SetSettingText(L"SOFTWARE\\Paralint.com\\Aucun\\SelfServe", name, text);
}

HRESULT GetSettingText(const wchar_t *key, const wchar_t *name, wchar_t *text, DWORD size)
{
   HRESULT result = E_FAIL;
   DWORD type;
   DWORD returnedsize = size;

   HKEY reg;

   if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, key, 0, KEY_READ, &reg) == ERROR_SUCCESS)
   {
      if (RegQueryValueEx(reg, name, 0, &type, (LPBYTE)text, &returnedsize) == ERROR_SUCCESS)
      {
         if ((type == REG_SZ) && (returnedsize < size) && (returnedsize > 0))
         {
            result = S_OK;
         }
      }

      RegCloseKey(reg);
   }

   return result;
}

HRESULT SetSettingText(const wchar_t *key, const wchar_t *name, const wchar_t *text)
{
   HRESULT result = E_FAIL;

   HKEY reg;

   if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, key, 0, KEY_WRITE, &reg) == ERROR_SUCCESS)
   {
      if (RegSetValueEx(reg, name, 0, REG_SZ, (LPBYTE)text, (DWORD)wcslen(text)+1) == ERROR_SUCCESS)
      {
            result = S_OK;
      }

      RegCloseKey(reg);
   }

   return result;
}

HRESULT GetSettingBinary(const wchar_t *key, const wchar_t *name, LPBYTE text, DWORD size)
{
   HRESULT result = E_FAIL;
   DWORD type;
   DWORD returnedsize = size;

   HKEY reg;

   if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, key, 0, KEY_READ, &reg) == ERROR_SUCCESS)
   {
      if (RegQueryValueEx(reg, name, 0, &type, (LPBYTE)text, &returnedsize) == ERROR_SUCCESS)
      {
         if ((type == REG_BINARY) && (returnedsize <= size) && (returnedsize > 0))
         {
            result = S_OK;
         }
      }

      RegCloseKey(reg);
   }

   return result;
}

HRESULT SetSettingBinary(const wchar_t *key, const wchar_t *name, LPBYTE data, DWORD len)
{
   HRESULT result = E_FAIL;

   HKEY reg;

   if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, key, 0, KEY_WRITE, &reg) == ERROR_SUCCESS)
   {
      if (RegSetValueEx(reg, name, 0, REG_BINARY, data, len) == ERROR_SUCCESS)
      {
            result = S_OK;
      }

      RegCloseKey(reg);
   }

   return result;
}

//Replaces any \n in the string with 0x0d 0x0a
//return the pointer to the received string
wchar_t *InterpretCarriageReturn(wchar_t *text)
{
   wchar_t *read = (wchar_t *)text;
   wchar_t *write = (wchar_t *)text;

   //Insert real \n caracters from the \n found in the string.
   while (*read)
   {
      if ((*read == '\\') && (*(read+1) == 'n'))
      {
         *write++ = '\n';
         read += 2;
      }
      else
      {
         *write++ = *read++;
      }
   }

   *write = 0;

   return text;
}

