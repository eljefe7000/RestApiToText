//this file is part of notepad++
//Copyright (C)2003 Don HO <donho@altern.org>
//
//This program is free software; you can redistribute it and/or
//modify it under the terms of the GNU General Public License
//as published by the Free Software Foundation; either
//version 2 of the License, or (at your option) any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program; if not, write to the Free Software
//Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

#pragma once

//
// All difinitions of plugin interface
//
#include "PluginInterface.h"
#include <WinInet.h>
#include <string>

using namespace std;

//-------------------------------------//
//-- STEP 1. DEFINE YOUR PLUGIN NAME --//
//-------------------------------------//
// Here define your plugin name
//
const TCHAR NPP_PLUGIN_NAME[] = TEXT("REST API To Text");

//-----------------------------------------------//
//-- STEP 2. DEFINE YOUR PLUGIN COMMAND NUMBER --//
//-----------------------------------------------//
//
// Here define the number of your plugin commands
//
const int nbFunc = 3;

//
// Initialization of your plugin data
// It will be called while plugin loading
//
void pluginInit(HANDLE hModule);

//
// Cleaning of your plugin
// It will be called while plugin unloading
//
void pluginCleanUp();

//
//Initialization of your plugin commands
//
void commandMenuInit();

//
//Clean up your plugin commands allocation (if any)
//
void commandMenuCleanUp();

//
// Function which sets your command 
//
bool setCommand(size_t index, LPCTSTR cmdName, PFUNCPLUGINCMD pFunc, ShortcutKey *sk = NULL, bool check0nInit = false);


//
// Your plugin command functions
//
BOOL IsContentTypeResponseHeaderForJson(HINTERNET hRequest);
BOOL EndsWith(string const& fullString, string const& ending);
BOOL GetEnvironmentVar(string var, string tag, string& returnValue);
BOOL InjectEnvironmentVars(string tmpToken, string varStartTag, string varEndTag, string envTag, string& returnValue);
INT_PTR CALLBACK GenericProcessor(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
string CheckForError(HINTERNET hRequest);
string GetResponseHeaders(HINTERNET hRequest);
string FormatResponseIntoJson(string response);
string UrlEncode(string url);
void MakeRestCall();
void AboutDialog();
void HelpDialog();
void LTrim(string* token);
void ToUpper(string* token);

