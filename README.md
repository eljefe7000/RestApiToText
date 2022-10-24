# RestApiToText for <a href="https://notepad-plus-plus.org">Notepad++</a>

RestApiToText is a plugin that will use the text from a Notepad++ editor tab to make a REST call and display the results in a new editor tab, eliminating the need for calling an outside REST tool and pasting the results into Notepad++.  Special thanks to Don Ho and the many developers and contributors for making Notepad++ possible.

License
-------
All files for this package are licensed under the GPL v3 license.&nbsp;&nbsp;See the LICENSE file for more details.

What's New in RestApiToText
---------------------------
1. Added the ability to use an environment variable when specifying header values, so that you can use a private API key without showing it in your REST call.
2. RestApiToText will automatically pretty-print JSON responses if the response header's __Content-Type__ header is set to __application/json__.
3. Added new __RestApiToTextOptions__ section to allow users to control the behavior of RestApiToText.
4. Added the __ShowResponseHeaders__ RestApiToText option to display response headers in addition to the response itself.
5. Querystrings are now url-encoded before they are sent.
6. Added the ability to pass a body with the __DELETE__ verb.
7. Fixed issue where the __PATCH__ verb was not passing the body.
8. Fixed issue where __InternetReadFile__ was stopping prematurely because the output was being formatted to JSON as it was being read.
9. Fixed issue where the __201 (Created)__ response was not returning the body.
10. Added ability to use environment variables in the body and the URL.
11. Added ability to show response on the same page, after the REST call.

New Releases
------------
You can download the latest version of RestApiToText.dll for 32-bit <a href="https://github.com/eljefe7000/RestApiToText/raw/master/Release/v1.4.0.0/RestApiToText.zip">here</a> and for 64-bit <a href="https://github.com/eljefe7000/RestApiToText/raw/master/x64/Release/v1.4.0.0/RestApiToText.zip">here</a> or by navigating the __Release__ and __x64/Release__ folders above. You just need to download the DLL and overwrite it in the appropriate subfolder of the Notepad++ plugins admin folder.

Here are the steps to overwrite the DLL:

1. Open Notepad++ and go to __Plugins->Open Plugins Folder...__
2. Close Notepad++.
3. In the Explorer window you created from Step 1, open the RestApiToText subfolder.
4. Overwrite the RestApiToText DLL with the one you downloaded.
5. Reopen Notepad++.

See [Changes by Version](#changes-by-version) below for a list of changes.

Usage
-----
1. Add code in the format shown below.
2. Go to Plugins -> REST API To Text -> Make REST Call.
3. A new editor tab with the results of the call should appear. (Results depend on the API you call)

![screenshot](/Screenshot1.png?raw=true "Example of a REST payload for RestApiToText")
![screenshot](/Screenshot2.png?raw=true "Example of a REST response for RestApiToText")

4. If the response's __Content-Type__ header is __application/json__ then the output will be automatically formatted:

![screenshot](/Screenshot4.png?raw=true "Example of a formatted REST response for RestApiToText")



Notes
-----
- RestApiToText always creates a new tab for the results, so you can easily keep different versions for reference.
- Only the verb and the URL are required.&nbsp;&nbsp;Headers, RestApiToTextOptions and Body are always optional.
- The HTTP/S protocol at the beginning of the URL is not required.  The port is also optional.
- The current version of the plugin will only make one call at a time.
- If the editor tab contains more content than your REST call (or other non-REST content), then select all of the REST text before making the call.
- You can also make calls to regular web pages.
- RestApiToText also supports the most commonly used REST verbs.&nbsp;&nbsp;The content returned depends on the API call.&nbsp;&nbsp;Some verbs might return no content, depending on the REST API you're calling and how you've configured the call.&nbsp;&nbsp;Here are some examples:

![screenshot](/Screenshot5.png?raw=true "Example of a formatted REST response and response headers for RestApiToText")

![screenshot](/Screenshot6.png?raw=true "Example of a formatted REST response and response headers for RestApiToText")

![screenshot](/Screenshot3.png?raw=true "Examples for other REST verbs")

![screenshot](/Screenshot7.png?raw=true "Example of a formatted REST response and response headers for RestApiToText")

- Use an environment variable whenever you want to send a private API key but you don't want to show it in your REST call.&nbsp;&nbsp;Just create the variable and wrap it in a __$(env:<span style="color:green">*variable name*</span>)__ expression, like so:

![screenshot](/Screenshot8.png?raw=true "Example of a REST call that uses an environment variable")
![screenshot](/Screenshot9.png?raw=true "Example of a REST call that uses an environment variable")

![screenshot](/Screenshot10.png?raw=true "Example of a REST call that uses an environment variable")

![screenshot](/Screenshot11.png?raw=true "Example of a REST call that uses an environment variable in the URL, a header, and the body")
![screenshot](/Screenshot12.png?raw=true "Example of a REST call that uses an environment variable in the URL, a header, and the body")
![screenshot](/Screenshot13.png?raw=true "Example of a REST call that uses an environment variable in the URL, a header, and the body")
![screenshot](/Screenshot14.png?raw=true "Example of a REST call that uses an environment variable in the URL, a header, and the body")
![screenshot](/Screenshot15.png?raw=true "Example of a REST call that uses an environment variable in the URL, a header, and the body")
![screenshot](/Screenshot16.png?raw=true "Example of a REST call that uses an environment variable in the URL, a header, and the body")

- Use the **RestApiToTextOptions** section for custom __RestApiToText__ features:
*(The following two features are currently available)*

![screenshot](/Screenshot17.png?raw=true "Examples of RestApiToTextOptions")



## Changes by Version
<u>1.4.0.0</u>
1. Added the ability to use an environment variable in the body and the URL.
2. Added the ability to show the response on the same page as the REST call.

<u>1.3.1.2</u>
1. Added the ability to use an environment variable in the body.

<u>1.3.1.1</u>
1. Fixed bug where 201 responses did not include the body.

<u>1.3.1.0</u>
1. Added the ability to use an environment variable when specifying header values, so that you can use a private API key without showing it in your REST call.

<u>1.3.0.0</u>
1. Added new __**RestApiToTextOptions**__ section to allow users to control the behavior of RestApiToText.
2. Added RestApiToText option called __ShowResponseHeaders__ to display response headers in addition to the response itself.
3. URL querystrings are now encoded before they are sent.
4. Added the ability to pass a body with the __DELETE__ verb.
5. Fixed issue where the __PATCH__ verb was not passing the body.

<u>1.2.1</u>
1. Fixed issue where InternetReadFile was stopping prematurely because the output was being formatted to JSON as it was being read.

<u>1.2</u>
1. Added JSON pretty-printing for responses with a Content-Type response header containing "application/json".
2. Added support for the HEAD and OPTIONS verbs.
3. URLs with no verb now default to a GET.

<u>1.1</u>
1. Fixed incorrect HTTP/S configurations that would result in "400 - Bad Request" or "405 - Method Not Allowed" in some situations.
2. Disabled default WinInet caching.

<u>1.0</u>
1. Initial release.