VERSION 2.00
Begin Form Form1 
   Caption         =   "Form1"
   ClientHeight    =   4755
   ClientLeft      =   1155
   ClientTop       =   1845
   ClientWidth     =   5385
   Height          =   5190
   Left            =   1080
   LinkTopic       =   "Form1"
   ScaleHeight     =   4755
   ScaleWidth      =   5385
   Top             =   1485
   Width           =   5535
   Begin CommandButton Shutdown 
      Caption         =   "Shut Down"
      Height          =   495
      Left            =   3720
      TabIndex        =   2
      Top             =   1320
      Width           =   1455
   End
   Begin CommandButton Dump 
      Caption         =   "Dump"
      Height          =   495
      Left            =   3720
      TabIndex        =   1
      Top             =   720
      Width           =   1455
   End
   Begin CommandButton Init 
      Caption         =   "Init"
      Height          =   495
      Left            =   3720
      TabIndex        =   0
      Top             =   120
      Width           =   1455
   End
End
Dim stockapp As object
Dim stocks As object
Dim stock As object
Dim stock2 As object
Dim stock3 As object
Dim stock4 As object

Sub Dump_Click ()
    Print ""
    Print "Dumping " + Str$(stocks.count) + " stocks..."
    For i = 0 To (stocks.count - 1)
        Set stock = stocks.Item(i)
        Print "  ";
        PrintAnItem stock
    Next i
End Sub

Sub Form_Load ()
    Set stockapp = CreateObject("Stocks.Document.1")
    Set stocks = stockapp.items
   
    stockapp.ShowWindow (1)
End Sub

Sub Init_Click ()
    'Set stock = stocks.add("Test1")
    'stock.Value = 98.3
    Set stock = stockapp.CreateItem("Test", 98.3)
    PrintAnItem stock
    stocks.add stock

    'Set stock = stocks.add("Test2")
    'stock.Value = 42
    Set stock = stockapp.CreateItem("Test2", 42)
    PrintAnItem stock
    stocks.add stock

    'Set stock = stocks.add("Test3")
    'stock.Value = 42
    Set stock = stockapp.CreateItem("Test3", 107.7007)
    PrintAnItem stock
    stocks.add stock
    stock.Value = 90003
    PrintAnItem stock

    Set stock = stocks.add("Test4")
    PrintAnItem stock
    stock.Value = "This is a string"
    PrintAnItem stock
    
    Set stock = stocks.Item("Test")
    PrintAnItem stock
    stock.Value = 103.3

    'Set stock2 = stocks.Item(3)
    'Print "  " + stock2.Name
    'Print "     " + stock2.Value
    
End Sub

Sub PrintAnItem (Item As Object)
    If (IsNumeric(Item.Value)) Then
        Print Item.Name + "   " + Str$(Item.Value)
    Else
        Print Item.Name + "   " + Item.Value
    End If
End Sub

Sub Shutdown_Click ()
    End
End Sub

