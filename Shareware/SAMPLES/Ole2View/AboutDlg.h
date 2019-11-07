
#ifndef __ABOUTDLG_H__
#define __ABOUTDLG_H__

#include "resource.h"
/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
    CAboutDlg();

// Dialog Data
    //{{AFX_DATA(CAboutDlg)
    enum { IDD = IDD_ABOUTBOX };
    CButton m_Cancel;
    CStatic m_WrittenBy;
    CStatic m_Version;
    CStatic m_Group;
    CStatic m_Copyright;
    CStatic m_AppName;
    CStatic m_AllRightsReserved;
    //}}AFX_DATA

// Implementation
protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //{{AFX_MSG(CAboutDlg)
    virtual BOOL OnInitDialog();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

#endif //__ABOUTDLG_H__
