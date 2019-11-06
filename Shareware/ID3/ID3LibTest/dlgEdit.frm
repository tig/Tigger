VERSION 5.00
Begin VB.Form dlgEdit 
   BorderStyle     =   3  'Fixed Dialog
   Caption         =   "View/Edit ID3 Field"
   ClientHeight    =   2385
   ClientLeft      =   2760
   ClientTop       =   3750
   ClientWidth     =   6030
   Icon            =   "dlgEdit.frx":0000
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   2385
   ScaleWidth      =   6030
   ShowInTaskbar   =   0   'False
   StartUpPosition =   1  'CenterOwner
   Begin VB.Frame Frame1 
      Caption         =   "&Value"
      Height          =   1215
      Left            =   120
      TabIndex        =   5
      Top             =   1080
      Width           =   5775
      Begin VB.OptionButton optionType 
         Caption         =   "File/&Binary"
         Height          =   255
         Index           =   2
         Left            =   2040
         TabIndex        =   10
         Top             =   300
         Width           =   1575
      End
      Begin VB.CommandButton cmdBrowse 
         Caption         =   "..."
         Height          =   315
         Left            =   5280
         TabIndex        =   9
         Top             =   660
         Width           =   375
      End
      Begin VB.TextBox txtValue 
         Height          =   315
         Left            =   120
         TabIndex        =   8
         Text            =   "Text1"
         Top             =   660
         Width           =   5115
      End
      Begin VB.OptionButton optionType 
         Caption         =   "&Numeric"
         Height          =   255
         Index           =   1
         Left            =   1020
         TabIndex        =   7
         Top             =   300
         Width           =   915
      End
      Begin VB.OptionButton optionType 
         Caption         =   "&String"
         Height          =   255
         Index           =   0
         Left            =   120
         TabIndex        =   6
         Top             =   300
         Width           =   915
      End
   End
   Begin VB.ComboBox comboField 
      Height          =   315
      Left            =   780
      Style           =   2  'Dropdown List
      TabIndex        =   4
      Top             =   510
      Width           =   1815
   End
   Begin VB.CommandButton CancelButton 
      Cancel          =   -1  'True
      Caption         =   "Cancel"
      Height          =   375
      Left            =   4680
      TabIndex        =   1
      Top             =   600
      Width           =   1215
   End
   Begin VB.CommandButton OKButton 
      Caption         =   "OK"
      Default         =   -1  'True
      Height          =   375
      Left            =   4680
      TabIndex        =   0
      Top             =   120
      Width           =   1215
   End
   Begin VB.Label lblFrame 
      Caption         =   "Label3"
      Height          =   255
      Left            =   720
      TabIndex        =   11
      Top             =   120
      Width           =   3855
   End
   Begin VB.Label Label2 
      Caption         =   "F&ield:"
      Height          =   255
      Left            =   120
      TabIndex        =   3
      Top             =   540
      Width           =   615
   End
   Begin VB.Label Label1 
      Caption         =   "&Frame:"
      Height          =   255
      Left            =   120
      TabIndex        =   2
      Top             =   120
      Width           =   615
   End
End
Attribute VB_Name = "dlgEdit"
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
Public fOKPressed As Boolean
Private frame As ID3Frame
Dim rgValues(0 To 256) As Variant
Dim rgChanged(0 To 256) As Boolean
Dim fUpdating As Boolean

Public Sub SetFrame(f As ID3Frame)
    Dim field As ID3Field
    Set frame = f
    
    ' Fill in the frame label
    lblFrame = GetFrameIDDesc(frame.ID)
    
    ' Fill in the fields combo box
    comboField.Clear
    Dim i As Integer
    For i = 1 To frame.Fields.Count
        comboField.AddItem GetFieldIDDesc(frame.Fields(i).ID)
        rgValues(i - 1) = frame.Fields(i).Value
        rgChanged(i - 1) = False
    Next i
    comboField.ListIndex = 0
    
    comboField_Click
    
    OKButton.Enabled = False
End Sub

Public Sub SetField(field As ID3Field)
    Dim f As ID3Field
    Dim i As Integer
    For i = 1 To frame.Fields.Count
        If frame.Fields(i).ID = field.ID Then comboField.ListIndex = i - 1
    Next
    comboField_Click
    OKButton.Enabled = False
End Sub

Private Sub comboField_Click()
    Dim v As Variant
    v = rgValues(comboField.ListIndex)
    
    If VarType(v) = vbString Then
        optionType(0).Value = True
    Else
        If VarType(v) = vbLong Then
            optionType(1).Value = True
        Else
            optionType(2).Value = True
        End If
    End If
    
    fUpdating = True
    txtValue = v
    fUpdating = False
End Sub

Private Sub Form_Load()
    OKButton.Enabled = False
    fOKPressed = False
    optionType(0).Enabled = False
    optionType(1).Enabled = False
    optionType(2).Enabled = False
End Sub

Private Sub CancelButton_Click()
    Hide
End Sub

Private Sub OKButton_Click()
    Dim i As Integer
    For i = 1 To frame.Fields.Count
        If rgChanged(i - 1) Then frame.Fields(i).Value = rgValues(i - 1)
    Next
    
    fOKPressed = True
    Hide
End Sub

Private Sub txtValue_Change()
    If Not fUpdating Then
        OKButton.Enabled = True
        rgChanged(comboField.ListIndex) = True
        If VarType(rgValues(comboField.ListIndex)) = vbString Then
            rgValues(comboField.ListIndex) = txtValue
        Else
            If VarType(rgValues(comboField.ListIndex)) = vbLong Then
                rgValues(comboField.ListIndex) = Val(txtValue)
            Else
    '            rgValues(comboField.ListIndex) = Val(txtValue)
            End If
        End If
    End If
End Sub
