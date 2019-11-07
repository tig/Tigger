Global DispAuto As Object

Sub Main ()

    Set DispAuto = CreateObject("autodude.CFileMgr")   ' Create new instance of calculator.

    DispAuto.About
    DispAuto = "Hello World"


End Sub

