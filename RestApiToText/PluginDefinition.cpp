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

//#include "pch.h"

#pragma once

#pragma comment(lib, "wininet.lib")


#include "PluginDefinition.h"
#include "menuCmdID.h"
#include <iostream>
#include <string>
#include <algorithm>
#include <array>
#include <cctype>
#include <map>
#include <WinInet.h>
#include "resource.h"
#include <CommCtrl.h>

using namespace std;

HANDLE _hModule;

FuncItem funcItem[nbFunc];

//
// The data of Notepad++ that you can use in your plugin commands
//
NppData nppData;

//
// Initialize your plugin data here
// It will be called while plugin loading   
void pluginInit(HANDLE hModule)
{
    _hModule = hModule;
}

//
// Here you can do the clean up, save the parameters (if any) for the next session
//
void pluginCleanUp()
{
}

//
// Initialization of your plugin commands
// You should fill your plugins commands here
void commandMenuInit()
{

    //--------------------------------------------//
    //-- STEP 3. CUSTOMIZE YOUR PLUGIN COMMANDS --//
    //--------------------------------------------//
    // with function :
    // setCommand(int index,                      // zero based number to indicate the order of command
    //            TCHAR *commandName,             // the command name that you want to see in plugin menu
    //            PFUNCPLUGINCMD functionPointer, // the symbol of function (function pointer) associated with this command. The body should be defined below. See Step 4.
    //            ShortcutKey *shortcut,          // optional. Define a shortcut to trigger this command
    //            bool check0nInit                // optional. Make this menu item be checked visually
    //            );

    ShortcutKey* makeApiCallShortcutKey = new ShortcutKey();
    makeApiCallShortcutKey->_isAlt = TRUE;
    makeApiCallShortcutKey->_isCtrl = TRUE;
    makeApiCallShortcutKey->_isShift = FALSE;
    makeApiCallShortcutKey->_key = 'A';

    setCommand(0, TEXT("&Make REST Call"), MakeRestCall, makeApiCallShortcutKey, false);
    setCommand(1, TEXT("&About..."), AboutDialog, NULL, false);
    setCommand(2, TEXT("&Help..."), HelpDialog, NULL, false);
}

//
// Here you can do the clean up (especially for the shortcut)
//
void commandMenuCleanUp()
{
    // Don't forget to deallocate your shortcut here
    delete funcItem[0]._pShKey;
    delete funcItem[1]._pShKey;
    delete funcItem[2]._pShKey;
}

void AboutDialog()
{
    DialogBox((HINSTANCE)_hModule, MAKEINTRESOURCE(IDD_RESTAPITOTEXT), nppData._nppHandle, GenericProcessor);
}

void HelpDialog()
{
    DialogBox((HINSTANCE)_hModule, MAKEINTRESOURCE(IDD_HELP), nppData._nppHandle, GenericProcessor);
}


// Message handler for About and Help dialog boxes.
INT_PTR CALLBACK GenericProcessor(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);

    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;

    case WM_NOTIFY:
        switch (((LPNMHDR)lParam)->code)
        {
        case NM_CLICK:          // Fall through to the next case.

        case NM_RETURN:
        {
            PNMLINK pNMLink = (PNMLINK)lParam;
            LITEM   item = pNMLink->item;
            HWND hLink = GetDlgItem(hDlg, IDC_SYSLINK_PROJECT);

            if ((((LPNMHDR)lParam)->hwndFrom == hLink) && (item.iLink == 0))
            {
                ShellExecute(NULL, L"open", item.szUrl, NULL, NULL, SW_SHOW);
            }

            break;
        }
        }
        break;
    }
    return (INT_PTR)FALSE;
}

//
// This function help you to initialize your plugin commands
//
bool setCommand(size_t index, LPCTSTR cmdName, PFUNCPLUGINCMD pFunc, ShortcutKey* sk, bool check0nInit)
{
    if (index >= nbFunc)
        return false;

    if (!pFunc)
        return false;

    lstrcpy(funcItem[index]._itemName, cmdName);
    funcItem[index]._pFunc = pFunc;
    funcItem[index]._init2Check = check0nInit;
    funcItem[index]._pShKey = sk;

    return true;
}

//----------------------------------------------//
//-- STEP 4. DEFINE YOUR ASSOCIATED FUNCTIONS --//
//----------------------------------------------//
void MakeRestCall()
{
    CHAR* selectedText = NULL;
    // Open a new document
    //::SendMessage(nppData._nppHandle, NPPM_MENUCOMMAND, 0, IDM_FILE_NEW);

    // Get the current scintilla
    int which = -1;

    ::SendMessage(nppData._nppHandle, NPPM_GETCURRENTSCINTILLA, 0, (LPARAM)&which);

    if (which == -1)
        return;

    HWND curScintilla = (which == 0) ? nppData._scintillaMainHandle : nppData._scintillaSecondHandle;

    size_t start = ::SendMessage(curScintilla, SCI_GETSELECTIONSTART, 0, 0);
    size_t end = ::SendMessage(curScintilla, SCI_GETSELECTIONEND, 0, 0);

    if (end < start)
    {
        size_t tmp = start;
        start = end;
        end = tmp;
    }

    size_t asciiTextLen = end - start;

    if (asciiTextLen == 0)
    {
        size_t allTextLength = ::SendMessage(curScintilla, SCI_GETLENGTH, 0, (LPARAM)selectedText);
        ::SendMessage(curScintilla, SCI_SETSELECTIONSTART, 0, (LPARAM)selectedText);
        ::SendMessage(curScintilla, SCI_SETSELECTIONEND, allTextLength, (LPARAM)selectedText);

        selectedText = new CHAR[allTextLength + 1];
    }
    else
        selectedText = new CHAR[asciiTextLen + 1];

    ::SendMessage(curScintilla, SCI_GETSELTEXT, 0, (LPARAM)selectedText);

    array<string, 5> restVerbs = { "GET", "POST", "PUT", "PATCH", "DELETE" };
    string s(selectedText);
    string eol("\r\n");
    string headerSeparator(": ");
    string portSeparator(":");
    string slashSeparator("/");
    string domain;
    string path;
    string verb;
    string url;
    string headerName;
    string headerValue;
    string body;
    string response;
    string::iterator tokenIterator;
    const string httpProtocol("http://");
    const string httpsProtocol("https://");
    LPSTR lpSelectedText = const_cast<char*>(s.c_str());
    BOOL firstLine = TRUE;
    BOOL doneWithHeaders = FALSE;
    BOOL doneWithBody = FALSE;
    BOOL workingOnHeaders = FALSE;
    BOOL workingOnBody = FALSE;
    BOOL httpsProtocolFound = FALSE;
    map<string, string> headers;
    map<string, string> ::iterator headerIterator;
    char* nextToken;
    rsize_t strmax = sizeof selectedText;
    size_t slashIndex = 0;
    int port = 0;

    char* token = ::strtok_s(lpSelectedText, eol.c_str(), &nextToken);

    while (token != NULL)
    {
        string strToken(token);

        // Left trim any whitespace
        strToken.erase(strToken.begin(), std::find_if(strToken.begin(), strToken.end(), [](unsigned char c) {
            return !std::isspace(c);
            }));

        string tmpToken = strToken;
        std::transform(tmpToken.begin(), tmpToken.end(), tmpToken.begin(),
            [](unsigned char c) { return std::tolower(c); });

        // Check for a header or body indicator
        if (!firstLine && !workingOnHeaders && tmpToken.rfind("**headers**", 0) != string::npos)
        {
            workingOnHeaders = TRUE;
            workingOnBody = FALSE;
            token = ::strtok_s(NULL, eol.c_str(), &nextToken);
            continue;
        }
        else if (!firstLine && !workingOnBody && tmpToken.rfind("**body**", 0) != string::npos)
        {
            workingOnBody = TRUE;
            workingOnHeaders = FALSE;
            token = ::strtok_s(NULL, eol.c_str(), &nextToken);
            continue;
        }

        // Are we processing the verb and URL?
        if (firstLine)
        {
            firstLine = FALSE;

            // Look for the space(s) separating the verb and URL
            size_t index = strToken.find_first_of(' ', 0);

            if (index > 0)
            {
                verb = strToken.substr(0, index);

                // Make the verb lowercase for string comparisons
                std::transform(verb.begin(), verb.end(), verb.begin(),
                    [](unsigned char c) { return std::toupper(c); });

                if (!any_of(restVerbs.begin(), restVerbs.end(), [verb](string v) {return v == verb; }))
                {
                    ::MessageBox(NULL, L"Verb not found.  Please open the help dialog for instructions on sending the request", L"ApiToText", MB_OK | MB_ICONERROR);
                    return;
                }

                strToken = strToken.substr(index);

                // Left trim any whitespace before the URL
                strToken.erase(strToken.begin(), std::find_if(strToken.begin(), strToken.end(), [](unsigned char c) {
                    return !std::isspace(c);
                    }));

                // What's left of strToken should be the URL
                url = strToken;

                // Remove the HTTP/S protocol if it's there
                if (strToken.find(httpProtocol) != string::npos)
                {
                    url = url.substr(httpProtocol.length());
                    port = INTERNET_DEFAULT_HTTP_PORT;
                }
                else if (strToken.find(httpsProtocol) != string::npos)
                {
                    httpsProtocolFound = TRUE;
                    url = url.substr(httpsProtocol.length());
                    port = INTERNET_DEFAULT_HTTPS_PORT;
                }

                // Find the port if it was included
                size_t portIndex = url.find(portSeparator.c_str());

                if (portIndex != string::npos)
                {
                    string portString = url.substr(portIndex + 1, url.length());
                    port = atoi(portString.c_str());
                    domain = url.substr(0, portIndex);
                    slashIndex = portString.find(slashSeparator.c_str());

                    if (slashIndex != string::npos)
                        path = portString.substr(slashIndex, portString.length());
                }
                else
                {
                    slashIndex = url.find(slashSeparator.c_str());
                    domain = url.substr(0, slashIndex);
                    if (slashIndex != string::npos)
                        path = url.substr(slashIndex, url.length());
                }
            }
        }
        else
        {
            if (workingOnHeaders)
            {
                // Left trim any whitespace before the header
                strToken.erase(strToken.begin(), std::find_if(strToken.begin(), strToken.end(), [](unsigned char c) {
                    return !std::isspace(c);
                    }));

                size_t headerIndex = strToken.find_first_of(headerSeparator.c_str(), 0);

                if (headerIndex != string::npos)
                {
                    headerName = strToken.substr(0, headerIndex);
                    strToken = strToken.substr(headerIndex + 1);

                    // Left trim any whitespace before the header value
                    strToken.erase(strToken.begin(), std::find_if(strToken.begin(), strToken.end(), [](unsigned char c) {
                        return !std::isspace(c);
                        }));

                    headerValue = strToken;
                    headers.insert(pair<string, string>(headerName, headerValue));
                }
            }
            else if (workingOnBody)
            {
                body += strToken;
            }
        }

        token = ::strtok_s(NULL, eol.c_str(), &nextToken);
    }

    if (httpsProtocolFound && port == 0)
        port = INTERNET_DEFAULT_HTTPS_PORT;

    wstring wsDomain = wstring(domain.begin(), domain.end());
    wstring wsPath = wstring(path.begin(), path.end());
    wstring wsVerb = wstring(verb.begin(), verb.end());
    DWORD dwInternetOptions = (httpsProtocolFound || port == INTERNET_DEFAULT_HTTPS_PORT ? INTERNET_FLAG_SECURE : 0) | INTERNET_FLAG_NO_CACHE_WRITE;

    HINTERNET hSession = InternetOpen(TEXT("Mozilla/5.0"), INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
    HINTERNET hConnect = InternetConnect(hSession, wsDomain.c_str(), port, TEXT(""), TEXT(""), INTERNET_SERVICE_HTTP, 0, 0);
    HINTERNET hRequest = HttpOpenRequest(hConnect, wsVerb.c_str(), wsPath.c_str(), NULL, NULL, NULL, dwInternetOptions, 0);
    //WCHAR szHeader[BUFSIZ] = { 0 };
    wstring wsHeaders;

    for (headerIterator = headers.begin(); headerIterator != headers.end(); headerIterator++)
    {
        wstring wsHeader = wstring(headerIterator->first.begin(), headerIterator->first.end());
        wstring wsHeaderValue = wstring(headerIterator->second.begin(), headerIterator->second.end());
        wsHeaders.append(wsHeader);
        wsHeaders.append(L": ");
        wsHeaders.append(wsHeaderValue);
        wsHeaders.append(L"\r\n");
    }

    wsHeaders.append(L"\r\n");


    HttpAddRequestHeaders(hRequest, wsHeaders.c_str(), -1, HTTP_ADDREQ_FLAG_ADD);

    LPVOID lpOptions = 0;
    int bodyLength = 0;

    if (verb == "POST" || verb == "PUT")
    {
        lpOptions = (LPVOID)body.c_str();
        bodyLength = body.length();
    }


    if (!HttpSendRequest(hRequest, NULL, 0, lpOptions, bodyLength))
    {
        string errorMessage = CheckForError(hRequest);

        if (!errorMessage.empty())
            response = errorMessage;
    }
    else
    {
        string errorMessage = CheckForError(hRequest);

        if (!errorMessage.empty())
            response = errorMessage;
        else
        {
            DWORD dwBufSize = BUFSIZ + 1;
            char* buffer = new char[BUFSIZ + 1];
            memset(buffer, 0x00, sizeof(buffer));

            while (true)
            {
                DWORD dwBytesRead;
                BOOL bRead;

                bRead = InternetReadFile(hRequest, buffer, BUFSIZ, &dwBytesRead);

                string strBuffer(buffer);
                wstring wStrBuffer(strBuffer.begin(), strBuffer.end());

                if (dwBytesRead == 0)
                    break;

                if (!bRead)
                {
                    DWORD dwError = GetLastError();
                    response.append("InternetReadFile error: ");
                    break;
                }
                else
                {
                    buffer[dwBytesRead] = 0;
                    response += strBuffer.substr(0, dwBytesRead);
                }
            }
        }
    }

    InternetCloseHandle(hRequest);
    InternetCloseHandle(hConnect);
    InternetCloseHandle(hSession);

    ::SendMessage(nppData._nppHandle, NPPM_MENUCOMMAND, 0, IDM_FILE_NEW);
    curScintilla = (which == 0) ? nppData._scintillaMainHandle : nppData._scintillaSecondHandle;
    ::SendMessage(curScintilla, SCI_SETTEXT, 0, (LPARAM)response.c_str());

    delete[] selectedText;
}


string CheckForError(HINTERNET hRequest)
{
    LPVOID lpOutBuffer = NULL;
    DWORD dwSize = 0;
    string response("");
    char* outBuffer = NULL;
    DWORD dwError = 0;
    DWORD dwLen = 1024;
    TCHAR* pBuff = new TCHAR[dwLen];
    int statusCode = 0;

    memset(pBuff, 0x00, sizeof(pBuff));

    while (!HttpQueryInfo(hRequest, HTTP_QUERY_STATUS_CODE, (LPVOID)lpOutBuffer, &dwSize, NULL))
    {
        dwError = GetLastError();

        if (dwError == ERROR_INSUFFICIENT_BUFFER)
            lpOutBuffer = new wchar_t[dwSize];
        else
        {
            char errorBuffer[100];
            sprintf_s(errorBuffer, "Call failed.  Error = %d (0x%x)\n", dwError, dwError);
            response.append(errorBuffer);
            break;
        }
    }

    if (response.length() < 1 && ((wchar_t*)lpOutBuffer)[0] != '0')
    {
        wstring wResponseCode((wchar_t*)lpOutBuffer);
        statusCode = _wtoi(wResponseCode.c_str());

        if (statusCode != HTTP_STATUS_OK && statusCode != 0)
        {
            wResponseCode.append(L" - ");
            response.append(wResponseCode.begin(), wResponseCode.end());
        }
    }

    delete[] lpOutBuffer;

    if (statusCode != HTTP_STATUS_OK)
    {
        while (!HttpQueryInfo(hRequest, HTTP_QUERY_STATUS_TEXT, pBuff, &dwLen, NULL))
        {
            dwError = GetLastError();

            if (dwError == ERROR_INSUFFICIENT_BUFFER)
            {
                delete[] pBuff;
                pBuff = new TCHAR[dwLen];
            }
            else
            {
                if (dwError != 0)
                {
                    char errorBuffer[100];
                    sprintf_s(errorBuffer, "Call failed.  Error = %d (0x%x)\n", dwError, dwError);
                    response.append(errorBuffer);
                }
                break;
            }
        }

        if (pBuff[0] != '\0')
        {
            wstring wBuff(pBuff);
            response.append(wBuff.begin(), wBuff.end());
        }


        delete[] pBuff;
    }

    return response;
}

