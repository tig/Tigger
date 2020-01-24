VERSION 2.00
Begin Form Form1 
   Caption         =   "Form1"
   ClientHeight    =   6075
   ClientLeft      =   1185
   ClientTop       =   1590
   ClientWidth     =   5385
   Height          =   6510
   Left            =   1110
   LinkTopic       =   "Form1"
   ScaleHeight     =   6075
   ScaleWidth      =   5385
   Top             =   1230
   Width           =   5535
   Begin CommandButton HideApp 
      Caption         =   "Hide App"
      Height          =   375
      Left            =   3720
      TabIndex        =   11
      Top             =   3840
      Width           =   1215
   End
   Begin CommandButton ShowApp 
      Caption         =   "Show App"
      Height          =   375
      Left            =   3720
      TabIndex        =   10
      Top             =   3480
      Width           =   1215
   End
   Begin CommandButton About 
      Caption         =   "About"
      Height          =   375
      Left            =   3720
      TabIndex        =   9
      Top             =   4320
      Width           =   1215
   End
   Begin CommandButton ActivateApp 
      Caption         =   "Activate App"
      Height          =   375
      Left            =   3720
      TabIndex        =   8
      Top             =   3120
      Width           =   1215
   End
   Begin CommandButton Activate 
      Caption         =   "Activate Doc"
      Height          =   375
      Left            =   3480
      TabIndex        =   7
      Top             =   840
      Width           =   1695
   End
   Begin CommandButton HideDoc 
      Caption         =   "Hide Doc"
      Height          =   375
      Left            =   3480
      TabIndex        =   6
      Top             =   1560
      Width           =   1695
   End
   Begin CommandButton DestroyDoc 
      Caption         =   "Destroy Document"
      Height          =   375
      Left            =   3480
      TabIndex        =   5
      Top             =   480
      Width           =   1695
   End
   Begin CommandButton ShowDoc 
      Caption         =   "Show Doc"
      Height          =   375
      Left            =   3480
      TabIndex        =   4
      Top             =   1200
      Width           =   1695
   End
   Begin CommandButton Test 
      Caption         =   "Test"
      Height          =   375
      Left            =   3720
      TabIndex        =   3
      Top             =   2160
      Width           =   1215
   End
   Begin CommandButton Shutdown 
      Caption         =   "Shut Down"
      Height          =   375
      Left            =   3720
      TabIndex        =   2
      Top             =   4800
      Width           =   1215
   End
   Begin CommandButton Dump 
      Caption         =   "Dump"
      Height          =   375
      Left            =   3720
      TabIndex        =   1
      Top             =   2520
      Width           =   1215
   End
   Begin CommandButton Init 
      Caption         =   "Create Document"
      Height          =   375
      Left            =   3480
      TabIndex        =   0
      Top             =   120
      Width           =   1695
   End
End

Dim doc As Object
Dim docitems As Object
Dim docs As Object
Dim docitem As Object
Dim doc2 As Object
Dim Appl As Object

Sub About_Click ()
    Set Appl = doc.application

    Appl.DoAbout
End Sub

Sub Activate_Click ()
    doc.activate
End Sub

Sub ActivateApp_Click ()
    Set Appl = doc.application
    Appl.activate
End Sub

Sub DestroyDoc_Click ()
    Set doc = Nothing
End Sub

Sub Dump_Click ()
    Print ""
    Print "Dumping " + Str$(docitems.count) + " docitems..."
    For i = 0 To (docitems.count - 1)
        Set docitem = docitems.Item(i)
        Print "  ";
        PrintAnItem docitem
    Next i
End Sub

Sub HideApp_Click ()
    doc.application.visible = False

End Sub

Sub HideDoc_Click ()
    doc.visible = False
End Sub

Sub Init_Click ()
    Set doc = CreateObject("GenAuto.Document.1")
End Sub

Sub PrintAnItem (Item As Object)
    If (IsNumeric(Item.Value)) Then
        Print Item.Name + "   " + Str$(Item.Value)
    Else
        Print Item.Name + "   " + Item.Value
    End If
End Sub

Sub ShowApp_Click ()
    doc.application.visible = True

End Sub

Sub ShowDoc_Click ()
    doc.visible = True
End Sub

Sub Shutdown_Click ()
    Set doc = Nothing
    Set docs = Nothing
    Set docitems = Nothing
    Set docitem = Nothing
    Set doc2 = Nothing
    Set Appl = Nothing
End Sub

Sub Test_Click ()
    Set Appl = doc.application

    Set docs = Appl.Documents
    Set doc2 = docs.Add
    doc2.Title = "Hello"
    docs.Add

    docs(0).Title = "Number1"

    Print docs.count
    For i = 0 To docs.count - 1
        Print docs(i).Title
    Next i

End Sub

