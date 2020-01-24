#ifndef __VIEW_H__
#define __VIEW_H__


class CMainWindow : public CFrameWnd
{
public:
    // Constructor
    CMainWindow() ;

    afx_msg void OnPaint() ;

    // Handler function for About dlg.
    afx_msg void OnAbout() ;

    // Macro to declare message map
    //
    DECLARE_MESSAGE_MAP()
} ;


class CTheApp : public CWinApp
{
public:
    // Override InitInstance
    BOOL InitInstance() ;
} ;


#endif 
