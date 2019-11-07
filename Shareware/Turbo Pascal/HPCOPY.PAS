Unit HPCopy;

{ This unit  is designed  to dump  graphics  images produced }
{ by  Turbo  Pascal  5.0's  Graph  Unit to a Hewlett-Packard }
{ LaserJet printer (on LPT1 by default; change in Unit       }
{ initialization sequence at bottom of unit)                 }
{                                                            }
{ CAUTION: you cannot link the TP5 Prn unit in with this     }
{ unit (or any program linked with this unit) since this     }
{ program redefines Lst as its own. No problem, though just  }
{ don't use the TP Prn unit...                               }
{                                                            }
{ You can set the  Aspect Ratio to whatever you like before  }
{ drawing things on the screen. For a VGA and a LaserJet II  }
{ the default aspect ratio for 100 dpi is almost perfect.    }
{                                                            }
{ For other graphics adapters, you may need to set the       }
{ ratio to something to give a proper looking hard copy. If  }
{ the Aspect Ratio is NOT set, the image produced by this    }
{ routine *may* appear Ellipsoidal.                          }
{                                                            }
{ The basic recommended sequence is something like:          }
{     GetAspectRatio(old_xasp,old_yasp);                     }
{       this gets old aspect ratio for your video adapter    }
{     SetAspectRatio(new_xasp,new_yasp);                     }
{       this sets a new (empirically determined) aspect      }
{       ratio to get hardcopy to look right                  }
{     (do what ever graphics to the screen you wish)         }
{     HPHardCopy;                                            }
{       make the hard copy with the adjusted ratio           }
{     SetAspectRatio(old_xasp,old_yasp);                     }
{       reset back to the "correct" aspect ratio             }



Interface

Uses Crt, Dos, Graph;

Var
   LST : Text;     { MUST Redefine because Turbo's Printer }
                    { Unit does not open LST with the File }
                    { Mode as BINARY.                       }

Procedure HPHardCopy;
{ Procedure to be  called when  the desired image is on the }
{ screen.                                                   }

Implementation

Var
   Width, Height : Word; { Variables used to store settings }
   Vport : ViewPortType; { Used in the call GetViewSettings }

{$F+}
Function LSTNoFunction ( Var F : TextRec ) : Integer;
{ This  function performs a NUL  operation  for a  Reset or }
{ Rewrite on LST.                                          }

Begin
   LSTNoFunction := 0;
End;

Function LSTOutPutToPrinter( Var F : TextRec ) : Integer;
{ LSTOutPutToPrinter sends the output to the Printer port }
{ number stored in the first byte or the  UserData area of }
{ the Text Record.                                         }

Var
   Regs : Registers;
   P : Word;

Begin
   With F Do
   Begin
      P := 0;
      Regs.AH := 16;
      While( P < BufPos ) and ( ( Regs.AH And 16 ) = 16 ) Do
      Begin
         Regs.AL := Ord( BufPtr^[P] );
         Regs.AH := 0;
         Regs.DX := UserData[1];
         Intr( $17, Regs );
         Inc( P );
      End;
      BufPos := 0;
   End;
   If( ( Regs.AH And 16 ) = 16 ) Then
      LSTOutPutToPrinter := 0         { No Error           }
   Else
      If( ( Regs.AH And 32 ) = 32 ) Then
         LSTOutPutToPrinter := 159    { Out of Paper       }
      Else
         LSTOutPutToPrinter := 160;   { Device Write Fault }
End;
{$F-}

Procedure AssignLST( Port : Byte );
{ AssignLST both sets up the LST text file record as would }
{ ASSIGN, and initializes it as would a RESET.             }
{                                                          }
{ The parameter  passed to this  procedure  corresponds to }
{ DOS's LPT  number.  It is set to 1 by  default, but  can }
{ easily  be  changed to any  LPT number  by  changing the }
{ parameter  passed  to  this  procedure  in  this  unit's }
{ initialization code.                                     }

Begin
   With TextRec( LST ) Do
   Begin
      Handle := $FFF0;
      Mode := fmOutput;
      BufSize := SizeOf( Buffer );
      BufPtr := @Buffer;
      BufPos := 0;
      OpenFunc := @LSTNoFunction;
      InOutFunc := @LSTOutPutToPrinter;
      FlushFunc := @LSTOutPutToPrinter;
      CloseFunc := @LSTOutPutToPrinter;
      UserData[1] := Port - 1;
   End;
End;

Procedure HPHardCopy;
{ Produces hard copy of a graph on Hewlett-Packard Laserjet }
{ printer By Joseph J. Hansen 9-15-87                       }
{ Modified Extensively for compatibility with Version 4.0's }
{ Graph Unit By Gary Stoker                                 }
{ Modified a little more for version 5 by David Holtkamp    }
{                                                           }
{ Unlike Graphix Toolbox procedure HardCopy, this procedure }
{ has no parameters, though it could easily be rewritten to }
{ include  resolution in dots  per inch,  starting  column, }
{ inverse image, etc.                                       }
{                                                           }

Const DotsPerInch  = '100';
                    { 100 dots per inch  gives  full-screen }
                    { width of 7.2 inches for Hercules card }
                    { graphs, 6.4 inches for IBM color card }
                    { and 6.4  inches  for EGA card.  Other }
                    { allowable values are 75, 150, and 300.}
                    { 75  dots  per  inch  will  produce  a }
                    { larger full-screen graph which may be }
                    { too  large to  fit  on an  8 1/2 inch }
                    { page; 150 and 300  dots per inch will }
                    { produce smaller graphs                }

      CursorPosition = '5';
                    { Column position of left side of graph }
      Esc            = #27;
                    { Escape character                      }

Var LineHeader     : String[6];
                    { Line  Header used for each  line sent }
                    { to the LaserJet printer.              }
    LineLength     : String[2];
                    { Length  in  bytes of  the  line to be }
                    { sent to the LaserJet.                 }
    Y              : Integer;
                    { Temporary loop Varible.               }

Procedure DrawLine ( Y : Integer );
{ Draws a single line of dots.  No of Bytes sent to printer }
{ is Width + 1.  Argument of the procedure is the row no, Y }

Var GraphStr       : String[128]; { String  used for OutPut }
                                  { this is good up to 1024 }
                                  { pixels wide in x        }
    Base           : Word;        { Starting   position  of }
                                  { output byte.            }
    BitNo,                        { Bit Number worked on    }
    ByteNo,                       { Byte number worked on   }
    DataByte       : Byte;        { Data Byte being built   }

Begin
  FillChar( GraphStr, SizeOf( GraphStr ), #0 );
  GraphStr := LineHeader;
  For ByteNo := 0 to Width  Do
  Begin
    DataByte := 0;
    Base := 8 * ByteNo;

    For BitNo := 0 to 7 Do
      If (GetPixel( BitNo+Base, Y ) > 0) Then
        DataByte := DataByte + 128 Shr BitNo;

    GraphStr := GraphStr + Chr (DataByte);
  End;

  Write (LST, GraphStr)

End; {Of Drawline}

Begin {Main procedure HPCopy}
  FillChar( LineLength, SizeOf( LineLength ), #0 );
  FillChar( LineHeader, SizeOf( LineHeader ), #0 );

  GetViewSettings( Vport );
  Width := ( Vport.X2 + 1 ) - Vport.X1;
  Width := ( ( Width - 7 ) Div 8 );
  Height := Vport.Y2 - Vport.Y1;

  Write (LST, Esc + 'E');                 { Reset Printer   }
  Write (LST, Esc+'*t'+DotsPerInch+'R');  { Set density in  }
                                          { dots per inch   }
  Write (LST, Esc+'&a'+CursorPosition+'C');{ Move cursor to }
                                          { starting col    }
  Write (LST, Esc + '*r1A');        { Begin raster graphics }

  Str (Width + 1, LineLength);
  LineHeader := Esc + '*b' + LineLength + 'W';


  For Y := 0 To Height + 1 Do
    DrawLine ( Y );

  Write (LST, Esc + '*rB');           { End Raster graphics }
  Write (LST, Esc + 'E');             { Reset  printer  and }
                                      { eject page          }
End;

Begin                                 { Unit initialization }
   AssignLST( 1 );
(* Writeln( 'LST INITIALIZED!' ); *)  { diagnostic message  }
End.
