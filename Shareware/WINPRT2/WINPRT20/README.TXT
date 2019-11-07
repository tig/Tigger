========================================================================
        OLE Control DLL : WINPRT20
========================================================================

ControlWizard has created this project for your WINPRT20 OLE Control DLL,
which contains 2 Controls.

This skeleton project not only demonstrates the basics of writing an OLE
Control, but is also a starting point for writing the specific features
of your control.

This file contains a summary of what you will find in each of the files
that make up your WINPRT20 OLE Control DLL.


WINPRT20.MAK
    This project file is compatible with the Visual C++ Workbench.
    It is also compatible with the NMAKE program provided with the
    Professional Edition of Visual C++.
    To build a debug version of the program from the MS-DOS prompt, type
        nmake DEBUG=1 /f WINPRT20.MAK
    or to build a release version of the program, type
        nmake DEBUG=0 /f WINPRT20.MAK

WINPRT20.H
    This is the main include file for the OLE Control DLL.  It includes
    other project-specific includes such as RESOURCE.H.

WINPRT20.CPP
    This is the main source file that contains the OLE DLL initialization,
    termination and other bookkeeping.

WINPRT20.RC
    This is a listing of all of the Microsoft Windows resources that the
    project uses.  This file can be directly edited with App Studio.

WINPRT20.RC2
    This file contains resources that are not edited by App Studio.  Initially
    this contains a VERSIONINFO resource that you can customize for your
    OLE Control DLL, and a TYPELIB resource for your DLL's type library.
    You should place other non-App Studio editable resources in this file.

WINPRT20.DEF
    This file contains information about the OLE Control DLL that must be
    provided to run with Microsoft Windows.  It defines parameters
    such as the name and description of the OLE Control DLL, and the
    size of the initial local heap.  The numbers in this file are typical
    for OLE Control DLLs.  The default stack size can be adjusted by
    editing the project file.

WINPRT20.CLW
    This file contains information used by ClassWizard to edit existing
    classes or add new classes.  ClassWizard also uses this file to store
    information needed to generate and edit message maps and dialog data
    maps and to generate prototype member functions.

WINPRT20.ODL
    This file contains the Object Description Language source code for the
    type library of your OLE Controls.

WINPRT20.ICO
    This file contains an icon that will appear in the About box.  This icon
    is included by the main resource file WINPRT20.RC.

/////////////////////////////////////////////////////////////////////////////

ENGINE.H
    This file contains the declaration of the CWinPrintEngine C++ class.

ENGINE.CPP
    This file contains the implementation of the CWinPrintEngine C++ class.

ENGPPG.H
    This file contains the declaration of the CEnginePropPage C++ class.

ENGPPG.CPP
    This file contains the implementation of the CEnginePropPage C++ class.

ENGINE.BMP
    This file contains a bitmap that a container will use to represent the
    CWinPrintEngine control when it appears on a tool palette.  This bitmap
    is included by the main resource file WINPRT20.RC.
/////////////////////////////////////////////////////////////////////////////

CONFIG.H
    This file contains the declaration of the CWinPrintConfig C++ class.

CONFIG.CPP
    This file contains the implementation of the CWinPrintConfig C++ class.

CFGPPG.H
    This file contains the declaration of the CConfigPropPage C++ class.

CFGPPG.CPP
    This file contains the implementation of the CConfigPropPage C++ class.

CONFIG.BMP
    This file contains a bitmap that a container will use to represent the
    CWinPrintConfig control when it appears on a tool palette.  This bitmap
    is included by the main resource file WINPRT20.RC.

/////////////////////////////////////////////////////////////////////////////
Licensing support:
WINPRT20.LIC
    This is the user license file.  This file must be present in the same
    directory as the control's DLL to allow an instance of the control to be
    created in a design-time environment.  Typically, you will distribute
    this file with your control, but your customers will not distribute it.

/////////////////////////////////////////////////////////////////////////////
Other standard files:

STDAFX.H, STDAFX.CPP
    These files are used to build a precompiled header (PCH) file
    named STDAFX.PCH and a precompiled types (PCT) file named STDAFX.OBJ.

RESOURCE.H
    This is the standard header file, which defines new resource IDs.
    App Studio reads and updates this file.

/////////////////////////////////////////////////////////////////////////////
Other notes:

ControlWizard uses "TODO:" to indicate parts of the source code you
should add to or customize.

/////////////////////////////////////////////////////////////////////////////
