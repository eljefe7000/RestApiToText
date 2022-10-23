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
#include <unordered_set>
#include <sstream>
#include <iomanip>

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
            HWND hProjectLink = GetDlgItem(hDlg, IDC_SYSLINK_PROJECT);
            HWND hPluginNewsLink = GetDlgItem(hDlg, IDC_SYSLINK_PLUGIN_NEWS);
            HWND hClickedLink = ((LPNMHDR)lParam)->hwndFrom;

            if ((hClickedLink == hProjectLink || hClickedLink == hPluginNewsLink) && (item.iLink == 0))
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

    array<string, 7> restVerbs = { "GET", "HEAD", "POST", "PUT", "PATCH", "DELETE", "OPTIONS" };
    string s(selectedText);
    string eol("\r\n");
    string headerSeparator(":");
    string portSeparator(":");
    string slashSeparator("/");
    string varStartTag("$(");
    string varEndTag(")");
    string envTag("ENV:");
    string domain;
    string path;
    string verb;
    string url;
    string headerName;
    string headerValue;
    string body;
    string response;
    string userAgent;
    string::iterator tokenIterator;
    const string httpProtocol("http://");
    const string httpsProtocol("https://");
    LPSTR lpSelectedText = const_cast<char*>(s.c_str());
    BOOL firstLine = TRUE;
    BOOL doneWithHeaders = FALSE;
    BOOL doneWithBody = FALSE;
    BOOL workingOnHeaders = FALSE;
    BOOL workingOnBody = FALSE;
    BOOL workingOnOptions = FALSE;
    BOOL httpsProtocolFound = FALSE;
    BOOL showResponseHeaders = FALSE;
    BOOL showResponseOnSamePage = FALSE;
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
        LTrim(&strToken);

        string tmpToken = strToken;

        ToUpper(&tmpToken);

        // Check for a header or body indicator
        if (!firstLine && tmpToken.find("**HEADERS**", 0) != string::npos)
        {
            workingOnHeaders = TRUE;
            workingOnBody = FALSE;
            workingOnOptions = FALSE;
            token = ::strtok_s(NULL, eol.c_str(), &nextToken);
            continue;
        }
        else if (!firstLine && tmpToken.find("**BODY**", 0) != string::npos)
        {
            workingOnBody = TRUE;
            workingOnHeaders = FALSE;
            workingOnOptions = FALSE;
            token = ::strtok_s(NULL, eol.c_str(), &nextToken);
            continue;
        }
        else if (!firstLine && tmpToken.find("**RESTAPITOTEXTOPTIONS**", 0) != string::npos)
        {
            workingOnOptions = TRUE;
            workingOnBody = FALSE;
            workingOnHeaders = FALSE;
            token = ::strtok_s(NULL, eol.c_str(), &nextToken);
            continue;
        }

        // Are we processing the verb and URL?
        if (firstLine)
        {
            firstLine = FALSE;

            // Look for the space(s) separating the verb and URL
            size_t index = strToken.find_first_of(" \t", 0);

            if (index > 0)
            {
                verb = strToken.substr(0, index);

                // Make the verb lowercase for string comparisons
                ToUpper(&verb);

                if (!any_of(restVerbs.begin(), restVerbs.end(), [verb](string v) {return v == verb;}))
                {
                    verb = "GET";
                    index = 0;
                }

                strToken = strToken.substr(index);

                // Left trim any whitespace before the URL
                LTrim(&strToken);

                // What's left of strToken should be the URL
                string tmpToken = strToken;

                InjectEnvironmentVars(tmpToken, varStartTag, varEndTag, envTag, url);

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
                LTrim(&strToken);

                size_t headerIndex = strToken.find_first_of(headerSeparator.c_str(), 0);

                if (headerIndex != string::npos)
                {
                    headerName = strToken.substr(0, headerIndex);
                    strToken = strToken.substr(headerIndex + 1);

                    // Left trim any whitespace before the header value
                    LTrim(&strToken);

                    headerValue = strToken;

                    string tmpHeaderValue = headerValue;

                    ToUpper(&tmpHeaderValue);

                    // Is this a reference to a variable?
                    if ((tmpHeaderValue.length() > varStartTag.length() + varEndTag.length()) && tmpHeaderValue.find(varStartTag.c_str(), 0) == 0 && EndsWith(tmpHeaderValue, varEndTag))
                    {
                        string hv = tmpHeaderValue.substr(varStartTag.length(), tmpHeaderValue.length() - (varStartTag.length() + varEndTag.length()));
                        size_t tagPos = hv.find(":", 0);

                        if (tagPos > 0)
                        {
                            string tag = hv.substr(0, tagPos + 1);
                            ToUpper(&tag);

                            if (tag == envTag)
                            {
                                if (!GetEnvironmentVar(hv, envTag, headerValue))
                                {
                                    hv = hv.substr(envTag.length());
                                    wstring wsEnvVar(hv.begin(), hv.end());
                                    wstring errorMessage(L"Could not find environment variable \"" + wsEnvVar + L"\".  If Notepad++ was running when you created the variable, try closing and reopening it.");
                                    ::MessageBox(nppData._nppHandle, errorMessage.c_str(), TEXT("RestApiToText"), MB_OK);
                                }
                            }
                        }
                    }

                    headers.insert(pair<string, string>(headerName, headerValue));
                }
            }
            else if (workingOnBody)
            {
                string tmpToken = strToken;
                string tmpBody;
                int currPos = 0;

                InjectEnvironmentVars(tmpToken, varStartTag, varEndTag, envTag, body);
            }
            else if (workingOnOptions)
            {
                // Left trim any whitespace before the header
                LTrim(&strToken);
                ToUpper(&strToken);

                if (strToken == "SHOWRESPONSEHEADERS")
                    showResponseHeaders = TRUE;

                if (strToken == "SHOWRESPONSEONSAMEPAGE")
                    showResponseOnSamePage = TRUE;
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

    if (verb == "POST" || verb == "PUT" || verb == "DELETE" || verb == "PATCH")
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
        if (verb == "HEAD" || verb == "OPTIONS")
            response = GetResponseHeaders(hRequest);
        else
        {
            BOOL contentTypeIsJson = IsContentTypeResponseHeaderForJson(hRequest);

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

                if (contentTypeIsJson)
                    response = FormatResponseIntoJson(response);

                if (showResponseHeaders)
                    response = GetResponseHeaders(hRequest) + "\n\n" + response;
            }
        }
    }

    InternetCloseHandle(hRequest);
    InternetCloseHandle(hConnect);
    InternetCloseHandle(hSession);

    if (!showResponseOnSamePage)
    {
        ::SendMessage(nppData._nppHandle, NPPM_MENUCOMMAND, 0, IDM_FILE_NEW);
        end = 0;
    }

    curScintilla = (which == 0) ? nppData._scintillaMainHandle : nppData._scintillaSecondHandle;

    ::SendMessage(curScintilla, SCI_INSERTTEXT, end, (LPARAM)response.c_str());

    delete[] selectedText;
}

BOOL InjectEnvironmentVars(string tmpToken, string varStartTag, string varEndTag, string envTag, string& returnValue)
{
    BOOL result = FALSE;
    int currPos = 0;
    string tmpBody;

    while (tmpToken.length() >= varStartTag.length() + varEndTag.length() && tmpToken.find(varStartTag.c_str(), 0) >= 0)
    {
        size_t startTagPos = tmpToken.find(varStartTag, currPos);
        if (startTagPos != string::npos)
        {
            size_t endTagPos = tmpToken.find(varEndTag, startTagPos + 2);
            if (endTagPos != string::npos)
            {
                startTagPos += 2;
                string contents = tmpToken.substr(startTagPos, endTagPos - startTagPos);
                size_t colonPos = contents.find(":", 0);
                if (colonPos != string::npos && colonPos > 0)
                {
                    string tag = contents.substr(0, colonPos + 1);
                    ToUpper(&tag);
                    if (tag == envTag)
                    {
                        string returnValue;
                        if (!GetEnvironmentVar(contents, envTag, returnValue))
                        {
                            wstring wsEnvVar(contents.begin(), contents.end());
                            wstring errorMessage(L"Could not find environment variable \"" + wsEnvVar + L"\".  If Notepad++ was running when you created the variable, try closing and reopening it.");
                            ::MessageBox(nppData._nppHandle, errorMessage.c_str(), TEXT("RestApiToText"), MB_OK);
                            break;
                        }
                        else
                        {
                            string newContents = tmpToken.substr(0, startTagPos - 2);
                            newContents += returnValue;
                            //newContents += tmpToken.substr(endTagPos + 1);
                            tmpBody += newContents;
                            tmpToken = tmpToken.substr(endTagPos + 1);
                        }
                    }
                    else
                        break;
                }
                else
                    break;
            }
            else
                break;
        }
        else
            break;
    }

    returnValue += tmpBody += tmpToken;


    return result;
}

BOOL GetEnvironmentVar(string envVar, string tag, string& returnValue)
{
    BOOL success = FALSE;
    string sEnvVar = envVar.substr(tag.length());

    char* pzEnvVar;
    errno_t error = _dupenv_s(&pzEnvVar, NULL, sEnvVar.c_str());

    if (pzEnvVar)
    {
        success = TRUE;
        returnValue = pzEnvVar;
        free(pzEnvVar);
    }

    return success;
}

BOOL EndsWith(std::string const& stringToSearch, std::string const& stringToFind)
{
    return (stringToSearch.length() >= stringToFind.length() && stringToSearch.compare(stringToSearch.length() - stringToFind.length(), stringToFind.length(), stringToFind) == 0);
}

void ToUpper(string* token)
{
    std::transform(token->begin(), token->end(), token->begin(),
        [](unsigned char c) { return std::toupper(c); });

    return;
}

void LTrim(string* token)
{
    token->erase(token->begin(), std::find_if(token->begin(), token->end(), [](unsigned char c) {
        return !std::isspace(c);
        }));

    return;
}

string UrlEncode(string queryString) {
    ostringstream escaped;
    escaped.fill('0');
    escaped << hex;

    for (string::const_iterator i = queryString.begin(), n = queryString.end(); i != n; ++i) {
        string::value_type c = (*i);

        // Keep alphanumeric and other accepted characters intact
        if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            escaped << c;
            continue;
        }

        // Any other characters are percent-encoded
        escaped << uppercase;
        escaped << '%' << setw(2) << int((unsigned char)c);
        escaped << nouppercase;
    }

    return escaped.str();
}

string FormatResponseIntoJson(string response)
{
    string json;
    BOOL isEscaped = FALSE;
    BOOL insideQuotedString = FALSE;
    unordered_set<char> delimiters = { '{', '}', '[', ']', ',' };
    int nbrTabs = 0;

    for (int i = 0; i < response.length(); i++)
    {
        if (response[i] == '"' && !isEscaped)
            insideQuotedString = !insideQuotedString;

        isEscaped = (response[i] == '\\');

        if (!insideQuotedString && delimiters.find(response[i]) != delimiters.end())
        {
            switch (response[i])
            {
            case '{':
            case '[':
                nbrTabs++;
                json.push_back(response[i]);
                json.push_back('\n');

                for (int i = 0; i < nbrTabs; i++)
                    json.push_back('\t');

                break;

            case '}':
            case ']':
                nbrTabs--;
                json.push_back('\n');

                for (int i = 0; i < nbrTabs; i++)
                    json.push_back('\t');

                json.push_back(response[i]);
                break;

            case ',':
                json.push_back(response[i]);
                json.push_back('\n');

                for (int i = 0; i < nbrTabs; i++)
                    json.push_back('\t');

                break;
            }
        }
        else
            json.push_back(response[i]);
    }

    return json;
}

string GetResponseHeaders(HINTERNET hRequest)
{
    LPVOID lpOutBuffer = NULL;
    DWORD dwSize = 0;
    BOOL contentTypeIsJson = FALSE;
    string responseHeaders;

    while (!HttpQueryInfo(hRequest, HTTP_QUERY_RAW_HEADERS_CRLF, (LPVOID)lpOutBuffer, &dwSize, NULL))
    {
        if (GetLastError() == ERROR_HTTP_HEADER_NOT_FOUND)
            return "";
        else
        {
            if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
                lpOutBuffer = new wchar_t[dwSize];
            else
            {
                if (lpOutBuffer)
                    delete[] lpOutBuffer;

                return "";
            }
        }
    }

    if (lpOutBuffer)
    {
        wstring wResponseHeaders = (wchar_t*)lpOutBuffer;
        responseHeaders.append(wResponseHeaders.begin(), wResponseHeaders.end());

        delete[] lpOutBuffer;
    }

    return responseHeaders;
}

BOOL IsContentTypeResponseHeaderForJson(HINTERNET hRequest)
{
    LPVOID lpOutBuffer = NULL;
    DWORD dwSize = 0;
    BOOL contentTypeIsJson = FALSE;

    while (!HttpQueryInfo(hRequest, HTTP_QUERY_CONTENT_TYPE, (LPVOID)lpOutBuffer, &dwSize, NULL))
    {
        if (GetLastError() == ERROR_HTTP_HEADER_NOT_FOUND)
            return TRUE;
        else
        {
            if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
                lpOutBuffer = new wchar_t[dwSize];
            else
            {
                if (lpOutBuffer)
                    delete[] lpOutBuffer;

                return FALSE;
            }
        }
    }

    if (lpOutBuffer)
    {
        wstring responseCode((wchar_t*)lpOutBuffer);
        std::transform(responseCode.begin(), responseCode.end(), responseCode.begin(), towlower);
        contentTypeIsJson = (responseCode.find(L"application/json") != wstring::npos);

        delete[] lpOutBuffer;
    }

    return contentTypeIsJson;
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

        if (statusCode != HTTP_STATUS_OK && statusCode != HTTP_STATUS_CREATED && statusCode != 0)
        {
            wResponseCode.append(L" - ");
            response.append(wResponseCode.begin(), wResponseCode.end());
        }
    }

    delete[] lpOutBuffer;

    if (statusCode != HTTP_STATUS_OK && statusCode != HTTP_STATUS_CREATED)
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

