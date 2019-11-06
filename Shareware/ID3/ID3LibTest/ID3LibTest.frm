VERSION 5.00
Object = "{831FDD16-0C5C-11D2-A9FC-0000F8754DA1}#2.0#0"; "mscomctl.ocx"
Begin VB.Form Main 
   Caption         =   "Kindel Systems Visual ID3"
   ClientHeight    =   6300
   ClientLeft      =   165
   ClientTop       =   735
   ClientWidth     =   6960
   Icon            =   "ID3LibTest.frx":0000
   LinkTopic       =   "Form1"
   ScaleHeight     =   6300
   ScaleWidth      =   6960
   StartUpPosition =   3  'Windows Default
   Begin MSComctlLib.ListView listFrames 
      Height          =   3075
      Left            =   60
      TabIndex        =   4
      ToolTipText     =   "Frame & Field List"
      Top             =   3180
      Width           =   6855
      _ExtentX        =   12091
      _ExtentY        =   5424
      View            =   3
      LabelEdit       =   1
      LabelWrap       =   -1  'True
      HideSelection   =   0   'False
      FullRowSelect   =   -1  'True
      GridLines       =   -1  'True
      _Version        =   393217
      ForeColor       =   -2147483640
      BackColor       =   -2147483643
      BorderStyle     =   1
      Appearance      =   1
      NumItems        =   4
      BeginProperty ColumnHeader(1) {BDD1F052-858B-11D1-B16A-00C0F0283628} 
         Text            =   "Frame"
         Object.Width           =   3704
      EndProperty
      BeginProperty ColumnHeader(2) {BDD1F052-858B-11D1-B16A-00C0F0283628} 
         SubItemIndex    =   1
         Text            =   "Field"
         Object.Width           =   2540
      EndProperty
      BeginProperty ColumnHeader(3) {BDD1F052-858B-11D1-B16A-00C0F0283628} 
         SubItemIndex    =   2
         Text            =   "Type"
         Object.Width           =   1411
      EndProperty
      BeginProperty ColumnHeader(4) {BDD1F052-858B-11D1-B16A-00C0F0283628} 
         SubItemIndex    =   3
         Text            =   "Data"
         Object.Width           =   8819
      EndProperty
   End
   Begin VB.DriveListBox drive 
      Height          =   315
      Left            =   60
      TabIndex        =   3
      ToolTipText     =   "Drive list"
      Top             =   2280
      Width           =   2655
   End
   Begin VB.FileListBox file 
      Height          =   2625
      Left            =   2760
      TabIndex        =   1
      ToolTipText     =   "File list"
      Top             =   0
      Width           =   4155
   End
   Begin VB.DirListBox dir 
      Height          =   2340
      Left            =   60
      TabIndex        =   0
      ToolTipText     =   "Directory list"
      Top             =   0
      Width           =   2655
   End
   Begin VB.Label lblCurFile 
      Height          =   375
      Left            =   60
      TabIndex        =   2
      Top             =   2760
      Width           =   6795
   End
   Begin VB.Menu mnuTag 
      Caption         =   "&Tag"
      Index           =   1
      Begin VB.Menu mnuReload 
         Caption         =   "&Reload"
      End
      Begin VB.Menu mnuUpdate 
         Caption         =   "&Update"
         Enabled         =   0   'False
      End
      Begin VB.Menu sep 
         Caption         =   "-"
      End
      Begin VB.Menu mnuExit 
         Caption         =   "E&xit"
      End
   End
   Begin VB.Menu mnuFrame 
      Caption         =   "&Frame"
      Begin VB.Menu mnuEditFrame 
         Caption         =   "&Edit"
      End
      Begin VB.Menu mnuAddFrame 
         Caption         =   "&Add"
      End
      Begin VB.Menu mnuClearFrame 
         Caption         =   "&Clear"
      End
      Begin VB.Menu mnuRemoveFrame 
         Caption         =   "&Remove"
      End
   End
   Begin VB.Menu mnuField 
      Caption         =   "F&ield"
      Begin VB.Menu mnuClearField 
         Caption         =   "&Clear"
      End
      Begin VB.Menu mnuSaveField 
         Caption         =   "&Save to file..."
      End
      Begin VB.Menu mnuLoadField 
         Caption         =   "&Load from file..."
      End
   End
   Begin VB.Menu mnuHelp 
      Caption         =   "&Help"
      Begin VB.Menu mnuAbout 
         Caption         =   "&About"
      End
   End
   Begin VB.Menu mnuQuick 
      Caption         =   "&Quick-Edit"
      Enabled         =   0   'False
   End
End
Attribute VB_Name = "Main"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
' VisID3 - A Visual ID3 Editor
'
' Copyright (c) 1999 Charles E. Kindel, Jr.
' All Rights Reserved
' See http://www.kindel.com/mp3/id3com for more information
'

Option Explicit

Dim g_Tag As ID3Tag

Private Sub mnuAddFrame_Click()
    If AddFrame() Then
        LoadList
        mnuUpdate.Enabled = True
    End If
End Sub

Private Sub mnuClearFrame_Click()
    If listFrames.ListItems.Count > 0 Then
        Dim i As Integer
        i = listFrames.SelectedItem.Index
        Dim frame As ID3Frame
        Set frame = listFrames.SelectedItem.Tag
        If ClearFrame(frame) Then
            LoadList
            If listFrames.ListItems.Count > 0 And i > 0 Then
                listFrames.ListItems(i).Selected = True
                listFrames.ListItems.item(i).EnsureVisible
            End If
            Main.mnuUpdate.Enabled = True
        End If
    End If
End Sub

Private Sub mnuClearField_Click()
    If listFrames.ListItems.Count > 0 Then
        Dim i As Integer
        i = listFrames.SelectedItem.Index
        Dim frame As ID3Frame
        Dim field As ID3Field
        Set frame = listFrames.SelectedItem.Tag
        Set field = frame.GetFieldByID(listFrames.SelectedItem.ListSubItems(1).Tag)
        If ClearField(field) Then
            LoadList
            If listFrames.ListItems.Count > 0 And i > 0 Then
                listFrames.ListItems(i).Selected = True
                listFrames.ListItems.item(i).EnsureVisible
            End If
            Main.mnuUpdate.Enabled = True
        End If
    End If

End Sub

Private Sub mnuEditFrame_Click()
    listFrames_DblClick
End Sub


Private Sub mnuReload_Click()
    file_Click
End Sub

Private Sub mnuRemoveFrame_Click()
    If listFrames.ListItems.Count > 0 Then
        Dim i As Integer
        i = listFrames.SelectedItem.Index
        Dim frame As ID3Frame
        Set frame = listFrames.SelectedItem.Tag
        If DeleteFrame(frame) Then
            LoadList
            If listFrames.ListItems.Count > 0 And i > 0 Then
                listFrames.ListItems(i).Selected = True
                listFrames.ListItems.item(i).EnsureVisible
            End If
            Main.mnuUpdate.Enabled = True
        End If
    End If
End Sub

Private Sub mnuQuick_Click()
    On Error Resume Next
    MsgBox "Title: " & g_Tag.Title
    If (Err.Number > 0) Then
        MsgBox Err.Description
    End If
    
    MsgBox "Album: " & g_Tag.Album
    If (Err.Number > 0) Then
        MsgBox Err.Description
    End If
    
    MsgBox "Artist: " & g_Tag.Artist
    If (Err.Number > 0) Then
        MsgBox Err.Description
    End If
    
    MsgBox "Year: " & g_Tag.Year
    If (Err.Number > 0) Then
        MsgBox Err.Description
    End If
    
    MsgBox "Genre: " & g_Tag.Genre
    If (Err.Number > 0) Then
        MsgBox Err.Description
    End If
    
    MsgBox "Comment: " & g_Tag.Comment
    If (Err.Number > 0) Then
        MsgBox Err.Description
    End If

End Sub

Private Sub mnuUpdate_Click()
    If Main.mnuUpdate.Enabled = False Then Exit Sub
    MousePointer = vbHourglass
    g_Tag.Update
    mnuUpdate.Enabled = False
    MousePointer = vbDefault
End Sub

Private Sub dir_Change()
    MousePointer = vbHourglass
    file.Path = dir.Path
    MousePointer = vbDefault
End Sub

Private Sub InsertFieldInList(frame As ID3Frame, field As ID3Field)
    Dim item As ListItem
    Dim v, textItem As Variant
    Dim s As String
    
    'On Error GoTo lbl
    v = field.Value
    If field.TextItems.Count > 1 Then
        s = v & " ("
        For Each textItem In field.TextItems
            s = s & textItem & ", "
        Next textItem
        s = s & ")"
    Else
        s = v
    End If
    'On Error GoTo 0

    Set item = listFrames.ListItems.Add(Text:=GetFrameIDDesc(frame.ID))
    Set item.Tag = frame
    item.ListSubItems.Add(Text:=GetFieldIDDesc(field.ID)).Tag = field.ID
    item.ListSubItems.Add Text:=TypeName(v)
    item.ListSubItems.Add(Text:=s).Tag = v

    Exit Sub
lbl:

End Sub

Private Sub InsertFrameInList(frame As ID3Frame)
    Dim field As ID3Field
    For Each field In frame.Fields
        InsertFieldInList frame, field
    Next field
End Sub

Private Sub drive_Change()
    MousePointer = vbHourglass
    On Error Resume Next
    dir.Path = drive.drive
    MousePointer = vbDefault
End Sub

Private Sub LoadList()
    MousePointer = vbHourglass
    Dim f As ID3Frame
    listFrames.ListItems.Clear
    For Each f In g_Tag.Frames
        InsertFrameInList f
    Next f
    If listFrames.ListItems.Count > 0 Then
        listFrames.ListItems(1).Selected = True
        listFrames.ListItems(1).EnsureVisible
    End If
        
    MousePointer = vbDefault
End Sub


Private Sub file_Click()
    If Main.mnuUpdate.Enabled And lblCurFile <> "" Then
        If MsgBox("The current tag has been changed. Do you want to update it?", vbYesNo, "Visual ID3") = vbYes Then
            MousePointer = vbHourglass
            g_Tag.Update
            MousePointer = vbDefault
        End If
    End If
    g_Tag.Clear
    If file.FileName <> "" Then
        lblCurFile = dir.Path & "\" & file.FileName
        MousePointer = vbHourglass
        g_Tag.Attach lblCurFile
        MousePointer = vbDefault
    Else
        lblCurFile = ""
    End If
    Main.mnuUpdate.Enabled = False
    LoadList
End Sub

Private Sub Form_Load()
    InitFrameIDArrays
    
    ' dir.Path = "\"
    file.Path = dir.Path
    file.Pattern = "*.mp3;*.tag;*.mpg;*.mp2"
           
    Set g_Tag = New ID3Tag
End Sub

Private Sub Form_Resize()
    Dim w As Long
    Dim h As Long
    w = Main.ScaleWidth - (file.Left + listFrames.Left)
    h = Main.ScaleHeight - (listFrames.Top + dir.Top)
    If w > 1 And h > 1 Then
        file.Width = w
        listFrames.Width = Main.ScaleWidth - (listFrames.Left * 2)
        listFrames.Height = h
        lblCurFile.Width = Main.ScaleWidth - (lblCurFile.Left + listFrames.Left)
    End If
End Sub



Private Sub listFrames_DblClick()
    Dim f As ID3Frame
    Dim fi As ID3Field
    Dim selectedIndex As Integer
    If listFrames.ListItems.Count < 1 Then Exit Sub
    
    selectedIndex = listFrames.SelectedItem.Index
    Set f = listFrames.SelectedItem.Tag
    Set fi = f.GetFieldByID(listFrames.SelectedItem.ListSubItems(1).Tag)
    
    If EditField(f, fi) Then
        MousePointer = vbHourglass
        Main.mnuUpdate.Enabled = True
        LoadList
        listFrames.ListItems.item(selectedIndex).Selected = True
        listFrames.ListItems.item(selectedIndex).EnsureVisible
        MousePointer = vbDefault
    Else
        Main.mnuUpdate.Enabled = True
    End If
    
End Sub

Private Sub listFrames_KeyDown(KeyCode As Integer, Shift As Integer)
   
    If KeyCode = vbKeyDelete Then
        mnuRemoveFrame_Click
    End If
    
    If KeyCode = vbKeyReturn Then
        listFrames_DblClick
    End If
    

End Sub

Private Sub mnuAbout_Click()
    Dim v As ID3VersInfo
    
    Set v = g_Tag.GetVersionInfo

    frmAbout.lblTitle = "Kindel Systems Visual ID3"
    frmAbout.lblDisclaimer = "Copyright (c) 1998-1999 Charles E. Kindel, Jr." & vbCrLf & _
                             "This application is freeware." & vbCrLf & _
                             "Kindel Systems http://www.kindel.com/mp3/id3com"
                             
    
    frmAbout.lblDescription = "Based on " & v.Name & " Version " & v.Version & ", Revision " & v.Revision
    frmAbout.Show vbModal
    
End Sub

Private Sub mnuExit_Click()
  Unload Main
End Sub

Private Function DeleteFrame(frame As ID3Frame) As Boolean
    Dim Desc As String
    Desc = GetFrameIDDesc(frame.ID)
    DeleteFrame = False
    If MsgBox(Prompt:="Are you sure you want to delete the " & Desc & " frame?", Buttons:=vbYesNo) = vbYes Then
        g_Tag.Frames.Remove frame
        DeleteFrame = True
    End If
End Function

Private Function ClearFrame(frame As ID3Frame) As Boolean
    Dim Desc As String
    Desc = GetFrameIDDesc(frame.ID)
    ClearFrame = False
    If MsgBox(Prompt:="Are you sure you want to clear the " & Desc & " frame?", Buttons:=vbYesNo) = vbYes Then
        frame.Clear
        ClearFrame = True
    End If
End Function

Private Function ClearField(field As ID3Field) As Boolean
    Dim Desc As String
    Desc = GetFieldIDDesc(field.ID)
    ClearField = False
    If MsgBox(Prompt:="Are you sure you want to clear the " & Desc & " field?", Buttons:=vbYesNo) = vbYes Then
        field.Clear
        ClearField = True
    End If
End Function


Private Function EditField(frame As ID3Frame, field As ID3Field) As Boolean
    Dim v As Variant
    
    dlgEdit.SetFrame frame
    dlgEdit.SetField field
    dlgEdit.Show vbModal
    EditField = dlgEdit.fOKPressed
    Unload dlgEdit
End Function

Private Function AddFrame() As Boolean
    Dim v As Variant
    Dim s As String
    Dim fChanged As Boolean
    
    AddFrame = False
    Dim frame As New ID3Frame
    s = ChooseFrameType()
    If (s <> "") Then
        On Error Resume Next
        frame.ID = s
        If Err.Number <> 0 Then
            MsgBox "The version of ID3Lib that ID3COM is based on does not support the " & s & " frame ID."
            Err.Clear
            Exit Function
        End If
        fChanged = EditFrame(frame)
        If fChanged Then
            If lblCurFile = "" Then
                Dim f As String
                f = InputBox("Enter name for new file", "Create new ID3 tag file")
                If f = "" Then Exit Function
                lblCurFile = dir.Path & "\" & f
                MousePointer = vbHourglass
                g_Tag.Attach lblCurFile
                file.Refresh
                MousePointer = vbDefault
            End If
            g_Tag.Frames.Add frame
            AddFrame = True
        End If
    End If
End Function


Private Function EditFrame(frame As ID3Frame) As Boolean
    dlgEdit.SetFrame frame
    dlgEdit.Show vbModal
    EditFrame = dlgEdit.fOKPressed
    Unload dlgEdit
End Function


