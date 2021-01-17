# RestApiToText for <a href="https://notepad-plus-plus.org">Notepad++</a>

RestApiToText is a plugin that will use the text from a Notepad++ editor tab to make a REST call and display the results in a new editor tab, eliminating the need for calling an outside REST tool and pasting the results into Notepad++.  Special thanks to Don Ho and the many developers and contributors for making Notepad++ possible.

License
-------
All files for this package are licensed under the GPL v3 license.&nbsp;&nbsp;See the LICENSE file for more details.

Break Fixes
-----------
Notepad++ currently provides version 1.0 of RestApiToText.  Version 1.1 of RestApiToText.dll is available for 32-bit <a href="https://github.com/eljefe7000/RestApiToText/raw/master/Release/v1.1/RestApiToText.dll">here</a> and for 64-bit <a href="https://github.com/eljefe7000/RestApiToText/raw/master/x64/Release/v1.1/RestApiToText.dll">here</a> or by navigating the __Release__ and __x64/Release__ folders above. You just need to download the DLL and overwrite it in the appropriate subfolder of the Notepad++ plugins admin folder.

Here are the steps to overwrite the DLL:

1. Open Notepad++ and go to __Plugins->Open Plugins Folder...__
2. Close Notepad++.
3. In the Explorer window you created from Step 1, open the RestApiToText subfolder.
4. Overwrite the RestApiToText DLL with the one you downloaded.
5. Reopen Notepad++.

Version 1.1 fixes incorrect HTTP/S configurations that would result in "400 - Bad Request" or "405 - Method Not Allowed" in some situations.  It also disables default WinInet caching.

Usage
-----
1. Add code in the format shown below.
2. Go to Plugins -> REST API To Text -> Make REST Call.
3. A new editor tab with the results of the call should appear. (Results depend on the API you call)

![screenshot](/Screenshot1.png?raw=true "Example of a REST payload for RestApiToText")
![screenshot](/Screenshot2.png?raw=true "Example of a REST response for RestApiToText")

Notes
-----
- RestApiToText always creates a new tab for the results, so you can easily keep different versions for reference.
- Only the verb and the URL are required.&nbsp;&nbsp;Headers and body are always optional.
- The HTTP/S protocol at the beginning of the URL is not required.
- The current version of the plugin will only make one call at a time.
- If the editor tab contains more content than your REST call, then select all of the REST text before making the call.
- You can also make calls to regular web pages.
- ApiToText also supports the other REST verbs.&nbsp;&nbsp;The content returned depends on the API call.&nbsp;&nbsp;Some verbs might not return content, depending on the REST API you're calling.&nbsp;&nbsp;Here are some examples:

![screenshot](/Screenshot3.png?raw=true "Examples for other REST verbs")



