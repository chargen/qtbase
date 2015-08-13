/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the plugins of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL21$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** As a special exception, The Qt Company gives you certain additional
** rights. These rights are described in The Qt Company LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QWINDOWSMOUSEHANDLER_H
#define QWINDOWSMOUSEHANDLER_H

#include "qtwindowsglobal.h"
#include "qtwindows_additional.h"

#include <QtCore/QPointer>
#include <QtCore/QHash>

QT_BEGIN_NAMESPACE

class QWindow;
class QTouchDevice;

class QWindowsMouseHandler
{
    Q_DISABLE_COPY(QWindowsMouseHandler)
public:
    QWindowsMouseHandler();

    QTouchDevice *touchDevice() const { return m_touchDevice; }

    bool translateMouseEvent(QWindow *widget, HWND hwnd,
                             QtWindows::WindowsEventType t, MSG msg,
                             LRESULT *result);
    bool translateTouchEvent(QWindow *widget, HWND hwnd,
                             QtWindows::WindowsEventType t, MSG msg,
                             LRESULT *result);
    bool translateScrollEvent(QWindow *window, HWND hwnd,
                              MSG msg, LRESULT *result);

    static inline Qt::MouseButtons keyStateToMouseButtons(int, bool);
    static inline Qt::KeyboardModifiers keyStateToModifiers(int);
    static inline int mouseButtonsToKeyState(Qt::MouseButtons);

    static Qt::MouseButtons queryMouseButtons();
    QWindow *windowUnderMouse() const { return m_windowUnderMouse.data(); }
    void clearWindowUnderMouse() { m_windowUnderMouse = 0; }

private:
    inline bool translateMouseWheelEvent(QWindow *window, HWND hwnd,
                                         MSG msg, LRESULT *result);

    QPointer<QWindow> m_windowUnderMouse;
    QPointer<QWindow> m_trackedWindow;
    QHash<DWORD, int> m_touchInputIDToTouchPointID;
    QHash<int, QPointF> m_lastTouchPositions;
    QTouchDevice *m_touchDevice;
    bool m_leftButtonDown;
    QWindow *m_previousCaptureWindow;
#ifdef Q_OS_WINCE
//This is required to send a touch up if we don't get a second touch position any more
    bool m_had2ndTouchPoint;
#endif
};

Qt::MouseButtons QWindowsMouseHandler::keyStateToMouseButtons(int wParam, bool extraButtons)
{
    Qt::MouseButtons mb(Qt::NoButton);
    wParam = GET_KEYSTATE_WPARAM(wParam);
    if (wParam & MK_LBUTTON)
        mb |= Qt::LeftButton;
    if (wParam & MK_MBUTTON)
        mb |= Qt::MiddleButton;
    if (wParam & MK_RBUTTON)
        mb |= Qt::RightButton;
    if (extraButtons) {
        if (wParam & MK_XBUTTON1 && !(wParam & ~MK_XBUTTON1))
            mb |= Qt::XButton1;
        if (wParam & MK_XBUTTON2 && !(wParam & ~MK_XBUTTON2))
            mb |= Qt::XButton2;
    }
    return mb;
}

Qt::KeyboardModifiers QWindowsMouseHandler::keyStateToModifiers(int wParam)
{
    Qt::KeyboardModifiers mods(Qt::NoModifier);
    if (wParam & MK_CONTROL)
      mods |= Qt::ControlModifier;
    if (wParam & MK_SHIFT)
      mods |= Qt::ShiftModifier;
    if (GetKeyState(VK_MENU) < 0)
      mods |= Qt::AltModifier;
    return mods;
}

int QWindowsMouseHandler::mouseButtonsToKeyState(Qt::MouseButtons mb)
{
    int result = 0;
    if (mb & Qt::LeftButton)
        result |= MK_LBUTTON;
    if (mb & Qt::MiddleButton)
        result |= MK_MBUTTON;
    if (mb & Qt::RightButton)
        result |= MK_RBUTTON;
    if (mb & Qt::XButton1)
        result |= MK_XBUTTON1;
    if (mb & Qt::XButton2)
        result |= MK_XBUTTON2;
    return result;
}

QT_END_NAMESPACE

#endif // QWINDOWSMOUSEHANDLER_H
