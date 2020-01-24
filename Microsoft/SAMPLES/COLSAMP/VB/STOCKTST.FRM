VERSION 2.00
Begin Form Form1 
   Caption         =   "Form1"
   ClientHeight    =   6075
   ClientLeft      =   1185
   ClientTop       =   1590
   ClientWidth     =   5385
   Height          =   6480
   Left            =   1125
   LinkTopic       =   "Form1"
   ScaleHeight     =   6075
   ScaleWidth      =   5385
   Top             =   1245
   Width           =   5505
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

Dim stockdoc As Object
Dim stocks As Object
Dim items As Object
Dim stock As Object
Dim doc2 As Object
Dim Appl As Object

Sub About_Click ()
    Set Appl = stockdoc.application

    Appl.DoAbout
End Sub

Sub Activate_Click ()
    stockdoc.activate
End Sub

Sub ActivateApp_Click ()
    Set Appl = stockdoc.application
    Appl.activate
End Sub

Sub DestroyDoc_Click ()
    Set stockdoc = Nothing
End Sub

Sub Dump_Click ()
    Print ""
    Print "Dumping " + Str$(stocks.count) + " stocks..."
    For i = 0 To (stocks.count - 1)
        Set stock = stocks.Item(i)
        Print "  ";
        PrintAnItem stock
    Next i
End Sub

Sub HideApp_Click ()
    stockdoc.application.visible = False

End Sub

Sub HideDoc_Click ()
    stockdoc.visible = False
End Sub

Sub Init_Click ()
    Set stockdoc = CreateObject("ColSamp.Document.1")
End Sub

Sub PrintAnItem (Item As Object)
    If (IsNumeric(Item.Value)) Then
        Print Item.Name + "   " + Str$(Item.Value)
    Else
        Print Item.Name + "   " + Item.Value
    End If
End Sub

Sub ShowApp_Click ()
    stockdoc.application.visible = True

End Sub

Sub ShowDoc_Click ()
    stockdoc.visible = True
End Sub

Sub Shutdown_Click ()
    Set stockdoc = Nothing
    Set stocks = Nothing
    Set stock = Nothing
    Set doc2 = Nothing
    Set Appl = Nothing
End Sub

Sub Test_Click ()

    Set stocks = stockdoc.items()

    Set stock = stockdoc.items.add()
    stockdoc.items(0).Name = "Eat me!"
    stockdoc.items(0).Value = Date
    PrintAnItem stockdoc.items(0)

    stocks.Item(0).Name = "The Date"
    PrintAnItem stocks.Item("The Date")

    
'GoTo TheEnd
  
    Set stock = stocks.add("No Value")
    'stock.Value = 98.3
    PrintAnItem stock

    'Print stocks.Item("test1").Value
    'Print stocks.Item(1)

'GoTo Theend

    Set stock = stocks.add("Far Out", "Wow")
    'stock.Value = 42
    'stock.Name = "Test2"
    PrintAnItem stock

    Set stock = stockdoc.items.add("Number 9", "Is Alive")

    Set stock = stocks.add("Time")
    stock.Value = Now
    PrintAnItem stock
    
    Set stock = stocks.add("3rd Item", 107.7007)
    PrintAnItem stock
    
    Set stock = stocks.add()
    PrintAnItem stock
    stock.Value = "The is the value of the 4th item"
    PrintAnItem stock
    
    Set items = stockdoc.items()
    Print ""
    Print "Dumping " + Str$(items.count) + " stocks..."
    For i = 0 To (items.count - 1)
        Set stock = items.Item(i)
        Print "  ";
        PrintAnItem stock
    Next i

    Set items = Nothing

GoTo Theend

    Set doc2 = CreateObject("ColSamp.Document.1")
    doc2.visible = True
    Set stock = doc2.items.add()
    doc2.items(0).Name = "First Item in doc2"
    doc2.items(0).Value = "Yowee!"

    PrintAnItem doc2.items(0)

    'Set A = doc2.application
    'A.DoAbout

    stockdoc.activate
    stockdoc.items.Remove ("Far Out")

Theend:
    
End Sub

