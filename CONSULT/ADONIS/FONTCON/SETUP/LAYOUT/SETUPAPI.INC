GLOBAL hSetup AS INTEGER
GLOBAL fFrameInit AS INTEGER
GLOBAL fInstallInit AS INTEGER
CONST SM_CXSCREEN = 0
CONST SM_CYSCREEN = 1
CONST WF_80x87    = 1024
CONST WF_CPU186   =  128
CONST WF_CPU286   =    2
CONST WF_CPU386   =    4
CONST WF_CPU486   =    8
CONST WF_STANDARD =   16
CONST WF_ENHANCED =   32
CONST MB_OK          = 0
CONST MB_ICONHAND    = 16
CONST MB_TASKMODAL   = 8192
DECLARE FUNCTION DoMsgBox LIB "mscomstf.dll" (lpText$,lpCaption$,wType%) As INTEGER
DECLARE SUB SetWindowText LIB "User" (hwnd%, lpString$)
DECLARE FUNCTION GetProfileString LIB "kernel" (szSect$, szKey$, szDefault$, szBuf$, cbBuf%) AS INTEGER
CONST cmoVital       = 1
CONST cmoCopy        = 2
CONST cmoUndo        = 4
CONST cmoRoot        = 8
CONST cmoDecompress  = 16
CONST cmoTimeStamp   = 32
CONST cmoReadOnly    = 64
CONST cmoBackup      = 128
CONST cmoForce       = 256
CONST cmoRemove      = 512
CONST cmoOverwrite   = 1024
CONST cmoAppend      = 2048
CONST cmoPrepend     = 4096
CONST cmoNone        = 0
CONST cmoAll         = 65535
CONST femExists     = 0
CONST femRead       = 1
CONST femWrite      = 2
CONST femReadWrite  = 3
CONST ynrcNo         = 0
CONST ynrcYes        = 1
CONST ynrcErr1       = 2
CONST ynrcErr2       = 3
CONST ynrcErr3       = 4
CONST ynrcErr4       = 5
CONST ynrcErr5       = 6
CONST ynrcErr6       = 7
CONST ynrcErr7       = 8
CONST ynrcErr8       = 9
CONST ynrcErr9       = 10
CONST grcOkay       = 0
CONST grcNotOkay    = 1
CONST grcUserQuit   = 48
CONST cbSymValMax = 512
CONST STFERR      = 1024   
CONST STFQUIT     = 1025   
CONST scmOff      = 0
CONST scmOnIgnore = 1
CONST scmOnFatal  = 2
DECLARE FUNCTION FOpenInf LIB "mscomstf.dll" (szFile$, fCheck%, fCheckSyms%) AS INTEGER
DECLARE FUNCTION CbGetInfSectionKeyField LIB "mscomstf.dll" (szSect$, szKey$, iField%, szBuf$, cbBuf%) AS INTEGER
DECLARE FUNCTION FSetBitmap LIB "msshlstf.dll" (szDll$, Bitmap%) AS INTEGER
DECLARE FUNCTION FDoDialog LIB "msuilstf.dll" (hwnd%, szDll$, Dlg%, szDlgProc$, HelpDlg%, szHelpProc$) AS INTEGER
DECLARE FUNCTION FKillNDialogs LIB "msuilstf.dll" (n%) AS INTEGER
DECLARE FUNCTION FSetSymbolValue LIB "msshlstf.dll" (szSymbol$, szValue$) AS INTEGER
DECLARE FUNCTION FRemoveSymbol LIB "mscomstf.dll" (szSym$) AS INTEGER
DECLARE FUNCTION CbGetSymbolValue LIB "mscomstf.dll" (szSymbol$, szValue$, Length%) AS INTEGER
DECLARE FUNCTION InitializeFrame LIB "msshlstf.dll" (szCmdLine$) AS INTEGER
DECLARE FUNCTION HwndFrame LIB "msshlstf.dll" AS INTEGER
DECLARE FUNCTION HinstFrame LIB "msshlstf.dll" AS INTEGER
DECLARE FUNCTION FCreateDir LIB "msinsstf.dll" (szDir$, cmo%) AS INTEGER
DECLARE FUNCTION FAddSectionFilesToCopyList LIB "mscomstf.dll" (szSect$, szSrc$, szDest$) AS INTEGER
DECLARE FUNCTION FAddSectionKeyFileToCopyList LIB "mscomstf.dll" (szSect$, szKey$, szSrc$, szDest$) AS INTEGER
DECLARE FUNCTION FAddSpecialFileToCopyList LIB "mscomstf.dll" (szSect$, szKey$, szSrc$, szDest$) AS INTEGER
DECLARE FUNCTION GrcCopyFilesInCopyList LIB "msinsstf.dll" (hInstance%) AS INTEGER
DECLARE FUNCTION FCreateIniKeyValue LIB "msinsstf.dll" (szFile$, szSect$, szKey$, szValue$, cmo%) AS INTEGER
DECLARE FUNCTION FCreateProgManGroup LIB "msinsstf.dll" (szGroup$, szPath$, cmo%) AS INTEGER
DECLARE FUNCTION FCreateProgManItem LIB "msinsstf.dll" (szGroup$, szItem$, szCmd$, cmo%) AS INTEGER
DECLARE FUNCTION FShowProgManGroup LIB "msinsstf.dll" (szGroup$, szCmd$, cmo%) AS INTEGER
DECLARE FUNCTION LcbGetCopyListCost LIB "msinsstf.dll" (szExtraList$, szCostList$, szNeedList$) AS LONG
DECLARE FUNCTION FCopyOneFile LIB "msinsstf.dll" (szSrc$, szDest$, cmo%, fAppend%) AS INTEGER
DECLARE FUNCTION YnrcRemoveFile LIB "msinsstf.dll" (szFullPathSrc$, cmo%) AS INTEGER
DECLARE FUNCTION YnrcBackupFile LIB "msinsstf.dll" (szFullPath$, szBackup$, cmo%) AS INTEGER
DECLARE FUNCTION FInitializeInstall LIB "msinsstf.dll" (hinst%, hwndFrame%) AS INTEGER
DECLARE FUNCTION WFindFileUsingFileOpen LIB "msinsstf.dll" (szFile$, szBuf$, cbBuf%) AS INTEGER
DECLARE FUNCTION FIsDirWritable LIB "msinsstf.dll" (szDir$) AS INTEGER
DECLARE FUNCTION FIsFileWritable LIB "msinsstf.dll" (szFile$) AS INTEGER
DECLARE FUNCTION FPrependToPath LIB "msinsstf.dll" (szSrc$, szDst$, szDir$, cmo%) AS INTEGER
DECLARE SUB SetBitmap(szDll$, Bitmap%)
DECLARE FUNCTION UIStartDlg(szDll$, Dlg%, szDlgProc$, HelpDlg%, szHelpProc$) AS STRING
DECLARE SUB UIPop (n%)
DECLARE SUB UIPopAll
DECLARE SUB SetTitle (sz$)
DECLARE SUB ReadInfFile (szFile$)
DECLARE SUB SetSymbolValue (szSymbol$, szValue$)
DECLARE SUB RemoveSymbol (szSym$)
DECLARE FUNCTION GetSymbolValue (szSymbol$) AS STRING
DECLARE FUNCTION InitFrame (szCmdLine$) AS INTEGER
DECLARE FUNCTION InitSetup (szCmdLine$) AS INTEGER
DECLARE SUB CreateDir (szDir$, cmo%)
DECLARE SUB AddSectionFilesToCopyList (szSect$, szSrc$, szDest$)
DECLARE SUB AddSectionKeyFileToCopyList (szSect$, szKey$, szSrc$, szDest$)
DECLARE SUB AddSpecialFileToCopyList (szSect$, szKey$, szSrc$, szDest$)
DECLARE SUB ClearCopyList
DECLARE SUB      ResetCopyList LIB "msinsstf.dll"
DECLARE SUB CopyFilesInCopyList
DECLARE SUB CreateIniKeyValue (szFile$, szSect$, szKey$, szValue$, cmo%)
DECLARE SUB CreateProgmanGroup (szGroup$, szPath$, cmo%)
DECLARE SUB CreateProgmanItem (szGroup$, szItem$, szCmd$, szOther$, cmo%)
DECLARE SUB ShowProgmanGroup (szGroup$, Cmd%, cmo%)
DECLARE FUNCTION GetCopyListCost (szExtraList$, szCostList$, szNeedList$) AS LONG
DECLARE SUB CopyFile (szFullPathSrc$, szFullPathDst$, cmo%, fAppend%)
DECLARE SUB RemoveFile (szFullPathSrc$, cmo%)
DECLARE SUB BackupFile (szFullPath$, szBackup$)
DECLARE SUB InitInstall
DECLARE FUNCTION FindFileUsingFileOpen (szFile$) AS STRING
DECLARE FUNCTION IsDirWritable (szDir$) AS INTEGER
DECLARE FUNCTION IsFileWritable (szFile$) AS INTEGER
DECLARE SUB PrependToPath (szSrc$, szDst$, szDir$, cmo%)
SUB ClearCopyList STATIC
    ResetCopyList
END SUB

FUNCTION InitSetup(szCmdLine$) STATIC AS INTEGER
    fFrameInit = 0
    fInstallInit = 0
    IF hSetup > 0 THEN
    END IF
    i% = InitFrame(szCmdLine$)
    fFrameInit = 1
    InitInstall
    fInstallInit = 1
    InitSetup = i%
END FUNCTION
FUNCTION InitFrame(szCmdLine$) STATIC AS INTEGER
    IF hSetup > 0 THEN
    ELSE
        i% = InitializeFrame(szCmdLine$)
        IF i% = -1 THEN
            END   
        ELSEIF i% = 0 THEN
            END
        ELSE
            InitFrame = i%
        END IF
    END IF
END FUNCTION
SUB SetBitmap(szDll$, Bitmap%) STATIC
    IF FSetBitmap(szDll$, Bitmap%) = 0 THEN
        ERROR STFERR
    END IF
END SUB
SUB SetTitle(sz$) STATIC
    SetWindowText HwndFrame(), sz$
END SUB
SUB ReadInfFile (szFile$) STATIC
    IF FOpenInf(szFile$, 1, 0) = 0 THEN
        ERROR STFERR
    END IF
END SUB
FUNCTION UIStartDlg(szDll$, Dlg%, szDlgProc$, HelpDlg%, szHelpProc$) STATIC AS STRING
    IF FDoDialog(HwndFrame(), szDll$, Dlg%, szDlgProc$, HelpDlg%, szHelpProc$) = 0 THEN
        ERROR STFERR
    ELSE
        UIStartDlg = GetSymbolValue("DLGEVENT")
    END IF
END FUNCTION
SUB UIPop (n%) STATIC
    IF FKillNDialogs(n%) = 0 THEN
        ERROR STFERR
    END IF
END SUB
SUB UIPopAll STATIC
    IF FKillNDialogs(65535) = 0 THEN
        ERROR STFERR
    END IF
END SUB
FUNCTION GetSymbolValue(szSymbol$) STATIC AS STRING
    szValue$ = string$(cbSymValMax,32)
    Length% = CbGetSymbolValue(szSymbol$, szValue$, cbSymValMax)
    GetSymbolValue = szValue$
    IF Length% >= cbSymValMax THEN
        res% = DoMsgBox("Buffer Overflow", "MS-Setup Error", MB_ICONHAND+MB_OK)
        ERROR STFERR
    END IF
    IF szSymbol$ = "" THEN
        ERROR STFERR
    END IF
    szValue$ = ""
END FUNCTION
SUB SetSymbolValue(szSymbol$, szValue$) STATIC
    IF FSetSymbolValue(szSymbol$, szValue$) = 0 THEN
        ERROR STFERR
    END IF
END SUB
SUB RemoveSymbol(szSym$) STATIC
    IF FRemoveSymbol(szSym$) = 0 THEN
        ERROR STFERR
    END IF
END SUB
SUB InitInstall STATIC
    IF hSetup > 0 THEN
    ELSEIF FInitializeInstall(HinstFrame(), HwndFrame()) = 0 THEN
        END
    END IF
END SUB
SUB CreateDir (szDir$, cmo%) STATIC
    IF FCreateDir(szDir$, cmo%) = 0 THEN
        ERROR STFERR
    END IF
END SUB
SUB CreateIniKeyValue (szFile$, szSect$, szKey$, szValue$, cmo%) STATIC
    IF FCreateIniKeyValue(szFile$, szSect$, szKey$, szValue$, cmo%) = 0 THEN
        ERROR STFERR
    END IF
END SUB
SUB CreateProgmanGroup (szGroup$, szPath$, cmo%) STATIC
    IF FCreateProgManGroup(szGroup$, szPath$, cmo%) = 0 THEN
        ERROR STFERR
    END IF
END SUB
SUB ShowProgmanGroup (szGroup$, Cmd%, cmo%) STATIC
    IF FShowProgManGroup(szGroup$, STR$(Cmd%), cmo%) = 0 THEN
        ERROR STFERR
    END IF
END SUB
FUNCTION GetCopyListCost (szExtraList$, szCostList$, szNeedList$) STATIC AS LONG
    lNeed& = LcbGetCopyListCost (szExtraList$, szCostList$, szNeedList$)
    IF lNeed& < 0 THEN
        ERROR STFERR
    END IF
    GetCopyListCost = lNeed&
END FUNCTION
SUB CreateProgmanItem (szGroup$, szItem$, szCmd$, szOther$, cmo%) STATIC
    szItemNew$ = szItem$
    IF szOther$ <> "" THEN
        szItemNew$ = szItem$ + "," + szOther$
    END IF
    IF FCreateProgManItem(szGroup$, szItemNew$, szCmd$, cmo%) = 0 THEN
        ERROR STFERR
    END IF
    szItemNew$ = ""
END SUB
SUB CopyFilesInCopyList STATIC
    grc% = GrcCopyFilesInCopyList (HinstFrame())
    IF grc% = grcUserQuit THEN
        ERROR STFQUIT
    ELSEIF grc% > 0 THEN
        ERROR STFERR
    END IF
END SUB
SUB CopyFile (szFullPathSrc$, szFullPathDst$, cmo%, fAppend%) STATIC
    IF  FCopyOneFile(szFullPathSrc$, szFullPathDst$, (cmo OR cmoCopy), fAppend%) = 0 THEN
        ERROR STFERR
    END IF
END SUB
SUB RemoveFile (szFullPathSrc$, cmo%) STATIC
    IF  YnrcRemoveFile(szFullPathSrc$, cmo%) = ynrcNo THEN
        ERROR STFERR
    END IF
END SUB
SUB BackupFile (szFullPath$, szBackup$) STATIC
    IF YnrcBackupFile(szFullPath$, szBackup$, cmoNone) = ynrcNo THEN
        ERROR STFERR
    END IF
END SUB
SUB AddSectionFilesToCopyList (szSect$, szSrc$, szDest$) STATIC
    IF FAddSectionFilesToCopyList (szSect$, szSrc$, szDest$) = 0 THEN
        ERROR STFERR
    END IF
END SUB
SUB AddSectionKeyFileToCopyList (szSect$, szKey$, szSrc$, szDest$) STATIC
    IF FAddSectionKeyFileToCopyList (szSect$, szKey$, szSrc$, szDest$) = 0 THEN
        ERROR STFERR
    END IF
END SUB
SUB AddSpecialFileToCopyList (szSect$, szKey$, szSrc$, szDest$) STATIC
    IF FAddSpecialFileToCopyList (szSect$, szKey$, szSrc$, szDest$) = 0 THEN
        ERROR STFERR
    END IF
END SUB
FUNCTION FindFileUsingFileOpen (szFile$) STATIC AS STRING
    szBuf$ = STRING$(512, 32)
    wRet% = WFindFileUsingFileOpen(szFile$, szBuf$, len(szBuf$))
    IF wRet% = 0 THEN
        FindFileUsingFileOpen = szBuf$
    ELSEIF wRet% = 1 THEN
        FindFileUsingFileOpen = ""
    ELSE
        ERROR STFERR
    END IF
    szBuf$ = ""
END FUNCTION
FUNCTION IsDirWritable (szDir$) STATIC AS INTEGER
    IsDirWritable = FIsDirWritable(szDir$)
END FUNCTION
FUNCTION IsFileWritable (szFile$) STATIC AS INTEGER
    IsFileWritable = FIsFileWritable(szFile$)
END FUNCTION
SUB PrependToPath (szSrc$, szDst$, szDir$, cmo%) STATIC
    IF FPrependToPath (szSrc$, szDst$, szDir$, cmo%) = 0 THEN
        ERROR STFERR
    END IF
END SUB
TRAP CleanupTrap From "MSSHLSTF.DLL"
End Trap
hSetup = InitSetup(COMMAND$)
ON ERROR GOTO QUIT
