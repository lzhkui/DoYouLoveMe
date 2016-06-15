// MyStatusBar.cpp : 实现文件
// 这个类创建的初衷是为了重写AssertValid()函数, 否则在多线程操作时，传入控件指针，Assert_Valid 宏在debug下会阻止其运行

#include "stdafx.h"
#include "FlowNavigator.h"
#include "MyStatusBar.h"


// MyStatusBar

IMPLEMENT_DYNAMIC(MyStatusBar, CStatusBar)

MyStatusBar::MyStatusBar()
{

}

MyStatusBar::~MyStatusBar()
{
}


BEGIN_MESSAGE_MAP(MyStatusBar, CStatusBar)
END_MESSAGE_MAP()



// MyStatusBar 消息处理程序




void MyStatusBar::AssertValid() const
{
	if (m_hWnd == NULL)
        return;     // null (unattached) windows are valid

    // check for special wnd??? values
    ASSERT(HWND_TOP == NULL);       // same as desktop
    if (m_hWnd == HWND_BOTTOM)
        ASSERT(this == &CWnd::wndBottom);
    else if (m_hWnd == HWND_TOPMOST)
        ASSERT(this == &CWnd::wndTopMost);
    else if (m_hWnd == HWND_NOTOPMOST)
        ASSERT(this == &CWnd::wndNoTopMost);
    else
    {
        // should be a normal window
        ASSERT(::IsWindow(m_hWnd));

        // should also be in the permanent or temporary handle map
/*        CHandleMap* pMap = afxMapHWND();
        ASSERT(pMap != NULL);

        CObject* p;
        ASSERT((p = pMap->LookupPermanent(m_hWnd)) != NULL ||
            (p = pMap->LookupTemporary(m_hWnd)) != NULL);
        ASSERT((CWnd*)p == this);   // must be us
*/
        // Note: if either of the above asserts fire and you are
        // writing a multithreaded application, it is likely that
        // you have passed a C++ object from one thread to another
        // and have used that object in a way that was not intended.
        // (only simple inline wrapper functions should be used)
        //
        // In general, CWnd objects should be passed by HWND from
        // one thread to another.  The receiving thread can wrap
        // the HWND with a CWnd object by using CWnd::FromHandle.
        //
        // It is dangerous to pass C++ objects from one thread to
        // another, unless the objects are designed to be used in
        // such a manner.
    }

}
