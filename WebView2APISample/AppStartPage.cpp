// Copyright (C) Microsoft Corporation. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "stdafx.h"

#ifdef USE_WEBVIEW2_WIN10
#include <pathcch.h>
#else
#include <Shlwapi.h>
#endif
#include <Psapi.h>
#include "AppStartPage.h"
#include "AppWindow.h"
#include "CheckFailure.h"

using namespace Microsoft::WRL;

namespace AppStartPage
{

std::wstring GetUri(AppWindow* appWindow)
{
    std::wstring uri = L"https://office.com/login";
    return uri;
}

}; // namespace AppStartPage