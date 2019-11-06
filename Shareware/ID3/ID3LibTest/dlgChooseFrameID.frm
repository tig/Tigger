VERSION 5.00
Object = "{831FDD16-0C5C-11D2-A9FC-0000F8754DA1}#2.0#0"; "mscomctl.ocx"
Begin VB.Form dlgChooseFrameID 
   BorderStyle     =   3  'Fixed Dialog
   Caption         =   "Choose Frame ID"
   ClientHeight    =   3225
   ClientLeft      =   2760
   ClientTop       =   3750
   ClientWidth     =   6030
   Icon            =   "dlgChooseFrameID.frx":0000
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   3225
   ScaleWidth      =   6030
   ShowInTaskbar   =   0   'False
   Begin MSComctlLib.ListView list 
      Height          =   3015
      Left            =   120
      TabIndex        =   2
      Top             =   120
      Width           =   4455
      _ExtentX        =   7858
      _ExtentY        =   5318
      View            =   3
      LabelEdit       =   1
      LabelWrap       =   -1  'True
      HideSelection   =   -1  'True
      FullRowSelect   =   -1  'True
      _Version        =   393217
      ForeColor       =   -2147483640
      BackColor       =   -2147483643
      BorderStyle     =   1
      Appearance      =   1
      NumItems        =   2
      BeginProperty ColumnHeader(1) {BDD1F052-858B-11D1-B16A-00C0F0283628} 
         Text            =   "Field ID"
         Object.Width           =   1587
      EndProperty
      BeginProperty ColumnHeader(2) {BDD1F052-858B-11D1-B16A-00C0F0283628} 
         SubItemIndex    =   1
         Text            =   "Description"
         Object.Width           =   10583
      EndProperty
   End
   Begin VB.CommandButton CancelButton 
      Caption         =   "Cancel"
      Height          =   375
      Left            =   4680
      TabIndex        =   1
      Top             =   600
      Width           =   1215
   End
   Begin VB.CommandButton OKButton 
      Caption         =   "OK"
      Height          =   375
      Left            =   4680
      TabIndex        =   0
      Top             =   120
      Width           =   1215
   End
End
Attribute VB_Name = "dlgChooseFrameID"
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

Private Sub CancelButton_Click()
    Hide
End Sub

Private Sub Form_Load()
    Dim item As ListItem
    Dim i As Integer
    
    fOKPressed = False
    For i = LBound(rgFrameID) To UBound(rgFrameID)
        Set item = list.ListItems.Add(Text:=rgFrameID(i))
        item.ListSubItems.Add Text:=rgFrameIDDesc(i)
    Next i

    list.ListItems(1).Selected = True
    list.ListItems(1).EnsureVisible
    
End Sub

Private Sub list_DblClick()
    OKButton_Click
End Sub

Private Sub OKButton_Click()
    fOKPressed = True
    Hide
End Sub
