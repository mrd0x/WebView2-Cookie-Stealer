// Copyright (C) Microsoft Corporation. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "stdafx.h"

#include "CustomStatusBar.h"
#include "AppWindow.h"


CustomStatusBar::CustomStatusBar()
{
}

CustomStatusBar::~CustomStatusBar()
{
}

void CustomStatusBar::Initialize(AppWindow* appWindow)
{

}

void CustomStatusBar::Show(std::wstring value)
{
}

void CustomStatusBar::Hide()
{
    SetWindowTextW(m_statusBarWindow, L"");
    ShowWindow(m_statusBarWindow, SW_HIDE);
}