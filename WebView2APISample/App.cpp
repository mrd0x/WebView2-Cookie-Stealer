// Copyright (C) Microsoft Corporation. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "stdafx.h"

#include "App.h"

#include <map>
#include <shellapi.h>
#include <shellscalingapi.h>
#include <shobjidl.h>
#include <string.h>
#include <vector>
#include "ScenarioCookieManagement.h"
#include "AppWindow.h"
#include "DpiUtil.h"

HINSTANCE g_hInstance;
int g_nCmdShow;
bool g_autoTabHandle = true;
static std::map<DWORD, HANDLE> s_threads;

static int RunMessagePump();
static DWORD WINAPI ThreadProc(void* pvParam);
static void WaitForOtherThreads();

#define NEXT_PARAM_CONTAINS(command)                                                           \
    _wcsnicmp(nextParam.c_str(), command, ARRAYSIZE(command) - 1) == 0

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, int nCmdShow)
{
    g_hInstance = hInstance;
    UNREFERENCED_PARAMETER(hPrevInstance);
    g_nCmdShow = nCmdShow;

    // Default DPI awareness to PerMonitorV2. The commandline parameters can
    // override this.
    DPI_AWARENESS_CONTEXT dpiAwarenessContext = DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2;
    std::wstring appId(L"Application");
    std::wstring userDataFolder(L""); //C:\\Users\\<username>\\AppData\\Local\\Google\\Chrome\\User Data
    std::wstring initialUri;
    DWORD creationModeId = IDM_CREATION_MODE_WINDOWED;

    new AppWindow(creationModeId, WebViewCreateOption(), initialUri, userDataFolder, false);
    
    int retVal = RunMessagePump();
    

    WaitForOtherThreads();

    return retVal;
}

// Run the message pump for one thread.
static int RunMessagePump()
{
    HACCEL hAccelTable = LoadAccelerators(g_hInstance, MAKEINTRESOURCE(IDC_WEBVIEW2APISAMPLE));

    MSG msg;

    // Main message loop:
    //! [MoveFocus0]
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            // Calling IsDialogMessage handles Tab traversal automatically. If the
            // app wants the platform to auto handle tab, then call IsDialogMessage
            // before calling TranslateMessage/DispatchMessage. If the app wants to
            // handle tabbing itself, then skip calling IsDialogMessage and call
            // TranslateMessage/DispatchMessage directly.
            if (!g_autoTabHandle || !IsDialogMessage(GetAncestor(msg.hwnd, GA_ROOT), &msg))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
    }
    //! [MoveFocus0]

    DWORD threadId = GetCurrentThreadId();
    auto it = s_threads.find(threadId);
    if (it != s_threads.end())
    {
        CloseHandle(it->second);
        s_threads.erase(threadId);
    }

    return (int)msg.wParam;
}

// Make a new thread.
void CreateNewThread(AppWindow* app)
{
    DWORD threadId;
    app->AddRef();
    HANDLE thread = CreateThread(
        nullptr, 0, ThreadProc, reinterpret_cast<LPVOID>(app),
        STACK_SIZE_PARAM_IS_A_RESERVATION, &threadId);
    s_threads.insert(std::pair<DWORD, HANDLE>(threadId, thread));
}

// This function is the starting point for new threads. It will open a new app window.
static DWORD WINAPI ThreadProc(void* pvParam)
{
    AppWindow* app = static_cast<AppWindow*>(pvParam);
    new AppWindow(app->GetCreationModeId(), app->GetWebViewOption());
    app->Release();
    return RunMessagePump();
}

// Called on the main thread.  Wait for all other threads to complete before exiting.
static void WaitForOtherThreads()
{
    while (!s_threads.empty())
    {
        std::vector<HANDLE> threadHandles;
        for (auto it = s_threads.begin(); it != s_threads.end(); ++it)
        {
            threadHandles.push_back(it->second);
        }

        HANDLE* handleArray = threadHandles.data();
        DWORD dwIndex = MsgWaitForMultipleObjects(
            static_cast<DWORD>(threadHandles.size()), threadHandles.data(), FALSE, INFINITE,
            QS_ALLEVENTS);

        if (dwIndex == WAIT_OBJECT_0 + threadHandles.size())
        {
            MSG msg;
            while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
    }
}
